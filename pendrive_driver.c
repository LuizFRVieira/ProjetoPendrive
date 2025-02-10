#include <linux/module.h>          // Para macros de módulo (module_init, module_exit, etc.)
#include <linux/kernel.h>          // Para funções do kernel (pr_info, pr_err, etc.)
#include <linux/fs.h>              // Para operações de sistema de arquivos (file_operations, etc.)
#include <linux/uaccess.h>         // Para funções de acesso ao espaço do usuário (copy_from_user)
#include <linux/slab.h>            // Para alocação de memória no kernel (kzalloc, kfree)
#include <linux/file.h>            // Para manipulação de arquivos no kernel (filp_open, filp_close)
#include <linux/dcache.h>          // Para manipulação de diretórios e dentries
#include <linux/namei.h>           // Para manipulação de caminhos no kernel (kern_path)
#include <linux/blkdev.h>          // Para operações de dispositivos de bloco (não utilizado neste código)
#include <linux/mnt_idmapping.h>   // Para manipulação de mnt_idmap (vfs_unlink)

#define DEVICE_NAME "pendrive_driver"  // Nome do dispositivo
#define CLASS_NAME "pendrive"          // Nome da classe do dispositivo
#define BUFFER_SIZE 512                // Tamanho do buffer para operações de leitura/escrita

static int major_number;               // Número major do dispositivo
static struct class *driver_class = NULL;  // Classe do dispositivo
static struct device *driver_device = NULL; // Dispositivo
static char *path;                     // Caminho do pendrive

// Protótipos das funções
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);
static int listar_arquivos_da_pasta(const char *path);
static int copiar_arquivo(const char *origem, const char *destino);
static int excluir_arquivo(const char *caminho);

// Estrutura para armazenar o contexto do diretório
struct contexto_ {
    struct dir_context context;  // Contexto para iteração de diretórios
};

// Callback para listar arquivos
static bool meu_callback(struct dir_context *ctx, const char *name, int namelen, loff_t offset, u64 ino, unsigned int d_type) {
    pr_info("File: %.*s\n", namelen, name);  // Exibe o nome do arquivo no log do kernel
    return true;  // Continua a listagem
}

// Função para listar arquivos
static int listar_arquivos_da_pasta(const char *path) {
    struct file *dir_file;
    struct contexto_ contexto_da_pasta = {
        .context.actor = meu_callback,  // Define o callback para listar arquivos
    };

    dir_file = filp_open(path, O_RDONLY | O_DIRECTORY, 0);  // Abre o diretório
    if (IS_ERR(dir_file)) {
        pr_err("Erro ao abrir o diretório: %ld\n", PTR_ERR(dir_file));  // Log de erro
        return PTR_ERR(dir_file);
    }

    iterate_dir(dir_file, &contexto_da_pasta.context);  // Itera sobre os arquivos do diretório
    filp_close(dir_file, NULL);  // Fecha o diretório
    return 0;
}

// Função para copiar arquivos
static int copiar_arquivo(const char *origem, const char *destino) {
    struct file *arquivo_origem, *arquivo_destino;
    char *buffer;
    ssize_t bytes_lidos, bytes_escritos;
    loff_t pos = 0;

    // Abrir arquivo de origem
    arquivo_origem = filp_open(origem, O_RDONLY, 0);
    if (IS_ERR(arquivo_origem)) {
        pr_err("Erro ao abrir arquivo de origem: %ld\n", PTR_ERR(arquivo_origem));
        return PTR_ERR(arquivo_origem);
    }

    // Abrir arquivo de destino
    arquivo_destino = filp_open(destino, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (IS_ERR(arquivo_destino)) {
        pr_err("Erro ao abrir arquivo de destino: %ld\n", PTR_ERR(arquivo_destino));
        filp_close(arquivo_origem, NULL);
        return PTR_ERR(arquivo_destino);
    }

    // Alocar buffer
    buffer = kzalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!buffer) {
        pr_err("Falha ao alocar memória para o buffer\n");
        filp_close(arquivo_origem, NULL);
        filp_close(arquivo_destino, NULL);
        return -ENOMEM;
    }

    // Copiar dados
    while ((bytes_lidos = kernel_read(arquivo_origem, buffer, BUFFER_SIZE, &pos)) > 0) {
        bytes_escritos = kernel_write(arquivo_destino, buffer, bytes_lidos, &pos);
        if (bytes_escritos != bytes_lidos) {
            pr_err("Erro ao escrever no arquivo de destino\n");
            kfree(buffer);
            filp_close(arquivo_origem, NULL);
            filp_close(arquivo_destino, NULL);
            return -EIO;
        }
    }

    // Liberar recursos
    kfree(buffer);
    filp_close(arquivo_origem, NULL);
    filp_close(arquivo_destino, NULL);

    pr_info("Arquivo copiado de %s para %s\n", origem, destino);  // Log de sucesso
    return 0;
}

// Função para excluir arquivos
static int excluir_arquivo(const char *caminho) {
    struct path path;
    int ret;
    struct mnt_idmap *idmap;

    ret = kern_path(caminho, LOOKUP_FOLLOW, &path);  // Resolve o caminho
    if (ret) {
        pr_err("Erro ao encontrar o arquivo: %d\n", ret);  // Log de erro
        return ret;
    }

    // Obter o mnt_idmap diretamente do vfsmount
    idmap = mnt_idmap(path.mnt);

    // Chamar vfs_unlink com os argumentos corretos
    ret = vfs_unlink(idmap, d_inode(path.dentry->d_parent), path.dentry, NULL);
    if (ret) {
        pr_err("Erro ao excluir o arquivo: %d\n", ret);  // Log de erro
        path_put(&path);
        return ret;
    }

    path_put(&path);  // Liberar o path
    pr_info("Arquivo excluído: %s\n", caminho);  // Log de sucesso
    return 0;
}

// Função write
static ssize_t device_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    char *comando;
    char *origem, *destino;
    int ret = 0;

    if (len > PATH_MAX) {
        pr_err("Caminho muito longo\n");  // Log de erro
        return -ENAMETOOLONG;
    }

    comando = kzalloc(len + 1, GFP_KERNEL);  // Alocar memória para o comando
    if (!comando) {
        pr_err("Falha ao alocar memória\n");  // Log de erro
        return -ENOMEM;
    }

    if (copy_from_user(comando, buffer, len)) {  // Copiar dados do espaço do usuário
        pr_err("Falha ao copiar dados do usuário\n");  // Log de erro
        kfree(comando);
        return -EFAULT;
    }

    comando[len] = '\0';  // Finalizar a string

    if (strncmp(comando, "LIST_FILES", 10) == 0) {
        pr_info("Listando arquivos da pasta %s\n\n", path);  // Log de listagem
        listar_arquivos_da_pasta(path);
    } else if (strncmp(comando, "COPY:", 5) == 0) {
        origem = strchr(comando, ':') + 1;
        destino = strchr(origem, ':');
        if (!destino) {
            pr_err("Formato inválido para cópia\n");  // Log de erro
            ret = -EINVAL;
            goto out;
        }
        *destino = '\0';
        destino++;
        ret = copiar_arquivo(origem, destino);  // Copiar arquivo
    } else if (strncmp(comando, "DELETE:", 7) == 0) {
        origem = strchr(comando, ':') + 1;
        ret = excluir_arquivo(origem);  // Excluir arquivo
    } else {
        strncpy(path, comando, 200);  // Copiar caminho para a variável global
        path[199] = '\0';  // Garantir que a string seja terminada
        pr_info("Caminho recebido: %s\n", path);  // Log de caminho recebido
    }

out:
    kfree(comando);  // Liberar memória alocada
    return ret ? ret : len;  // Retornar o número de bytes processados ou erro
}

// Função open
static int device_open(struct inode *inodep, struct file *filep) {
    pr_info("Device aberto %s\n", path);  // Log de abertura do dispositivo
    return 0;
}

// Função release
static int device_release(struct inode *inodep, struct file *filep) {
    pr_info("Device fechado\n");  // Log de fechamento do dispositivo
    return 0;
}

// Estrutura file_operations
static struct file_operations fops = {
    .open = device_open,      // Função chamada ao abrir o dispositivo
    .release = device_release, // Função chamada ao fechar o dispositivo
    .write = device_write,    // Função chamada ao escrever no dispositivo
};

// Inicialização do driver
static int __init pendrive_driver_init(void) {
    int ret = 0;

    pr_info("Inicializando o módulo %s\n", DEVICE_NAME);  // Log de inicialização

    path = kzalloc(200, GFP_KERNEL);  // Alocar memória para o caminho
    if (!path) {
        pr_err("Falha ao alocar memória para o caminho\n");  // Log de erro
        return -ENOMEM;
    }

    // Registrar número major
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_err("Falha ao registrar char device\n");  // Log de erro
        ret = major_number;
        goto fail_path;
    }

    // Criar classe do dispositivo
    driver_class = class_create(CLASS_NAME);
    if (IS_ERR(driver_class)) {
        pr_err("Falha ao criar classe do dispositivo\n");  // Log de erro
        ret = PTR_ERR(driver_class);
        goto fail_chrdev;
    }

    // Criar dispositivo
    driver_device = device_create(driver_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(driver_device)) {
        pr_err("Falha ao criar dispositivo\n");  // Log de erro
        ret = PTR_ERR(driver_device);
        goto fail_class;
    }

    pr_info("Dispositivo %s criado com sucesso\n", DEVICE_NAME);  // Log de sucesso
    return 0;

fail_class:
    class_destroy(driver_class);  // Destruir classe em caso de erro
fail_chrdev:
    unregister_chrdev(major_number, DEVICE_NAME);  // Desregistrar dispositivo em caso de erro
fail_path:
    kfree(path);  // Liberar memória alocada para o caminho
    return ret;
}

// Finalização do driver
static void __exit pendrive_driver_exit(void) {
    kfree(path);  // Liberar memória alocada para o caminho
    device_destroy(driver_class, MKDEV(major_number, 0));  // Destruir dispositivo
    class_destroy(driver_class);  // Destruir classe
    unregister_chrdev(major_number, DEVICE_NAME);  // Desregistrar dispositivo
    pr_info("Módulo %s descarregado\n", DEVICE_NAME);  // Log de descarregamento
}

module_init(pendrive_driver_init);  // Função de inicialização do módulo
module_exit(pendrive_driver_exit);  // Função de finalização do módulo

MODULE_LICENSE("GPL");  // Licença do módulo
MODULE_AUTHOR("SOP NEW MODULE");  // Autor do módulo
MODULE_DESCRIPTION("Driver de char device para listar arquivos de pendrive");  // Descrição do módulo
