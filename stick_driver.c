#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>           // Para manipulação de arquivos
#include <linux/uaccess.h>      // Para conversão de ponteiros de usuários
#include <linux/delay.h>        // Para uso de mdelay
#include <linux/dirent.h>       // Para manipulação de diretórios

// Função de callback para listar arquivos
static int list_file_callback(struct dir_context *ctx, const char *name, int namelen, loff_t offset, u64 ino, unsigned int d_type) {
    printk(KERN_INFO "[*] Luiz: Arquivo encontrado: %s\n", name);
    return 0; // Retorna 0 para continuar a iteração
}

// Função para listar arquivos no diretório
void list_files(const char *dir_path) {
    struct file *dir_file;
    struct dir_context ctx;
    int err = 0;

    printk(KERN_INFO "[*] Luiz: Listando arquivos no diretório: %s\n", dir_path);

    // Abrindo o diretório
    dir_file = filp_open(dir_path, O_RDONLY, 0);
    if (IS_ERR(dir_file)) {
        printk(KERN_ERR "[*] Luiz: Não foi possível abrir o diretório %s\n", dir_path);
        return;
    }

    // Preparando o contexto para iteração
    memset(&ctx, 0, sizeof(struct dir_context));
    
    // Definindo a função de callback para a iteração
    ctx.actor = (filldir_t)list_file_callback;

    // Iterando sobre o diretório
    err = iterate_dir(dir_file, &ctx);
    if (err < 0) {
        printk(KERN_ERR "[*] Luiz: Erro ao iterar sobre o diretório: %d\n", err);
    }

    // Fechando o diretório usando filp_close
    filp_close(dir_file, NULL);  // Substituindo fput por filp_close
}

// Função probe
static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    printk(KERN_INFO "[*] Luiz: O pendrive (%04X:%04X) conectado\n", id->idVendor, id->idProduct);

    // Verificando se a interface é válida e exibindo detalhes do dispositivo
    if (!interface) {
        printk(KERN_ERR "[*] Luiz: Interface USB inválida.\n");
        return -ENODEV;
    }

    // Mostrando o número da interface e o número do ponto de terminação
    printk(KERN_INFO "[*] Luiz: Número da interface: %d\n", interface->cur_altsetting->desc.bInterfaceNumber);

    // Chame a função para listar arquivos do pendrive
    list_files("/media/luiz/ESD-USB");  // Caminho correto para o pendrive

    return 0; // Sucesso
}

// Função disconnect
static void pen_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "[*] Luiz: Pendrive removido\n");
}

// Tabela de IDs suportados (use lsusb para obter os valores corretos)
static struct usb_device_id pen_table[] = {
    { USB_DEVICE(0x0781, 0x5571) }, // Este é um exemplo de ID; substitua pelo ID real do seu dispositivo
    {} // Termina a lista
};
MODULE_DEVICE_TABLE(usb, pen_table);

// Estrutura usb_driver
static struct usb_driver pen_driver = {
    .name = "Luiz-USB",
    .id_table = pen_table,
    .probe = pen_probe,
    .disconnect = pen_disconnect,
};

// Inicialização do driver
static int __init pen_init(void) {
    int ret = -1;
    printk(KERN_INFO "[*] Luiz: Construtor do driver\n");
    printk(KERN_INFO "\tRegistrando driver com o kernel\n");
    ret = usb_register(&pen_driver);
    if (ret == 0) {
        printk(KERN_INFO "\tRegistro concluído\n");
    } else {
        printk(KERN_ERR "\tFalha ao registrar driver\n");
    }
    return ret;
}

// Finalização do driver
static void __exit pen_exit(void) {
    printk(KERN_INFO "[*] Luiz: Destrutor do driver\n");
    usb_deregister(&pen_driver);
    printk(KERN_INFO "\tDesregistro completo!\n");
}

module_init(pen_init);
module_exit(pen_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LUIZ");
MODULE_DESCRIPTION("Driver de registro de pendrive USB");

