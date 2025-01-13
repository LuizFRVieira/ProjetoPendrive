#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

// Funcao probe
// Chamada na insercao do dispositivo, se e somente se nenhum outro driver o gerenciar
static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    printk(KERN_INFO "[*] Luiz: O pendrive (%04X:%04X) conectado\n", id->idVendor, id->idProduct);
    return 0; // Retornar 0 indica que gerenciaremos este dispositivo
}

// Funcao disconnect
static void pen_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "[*] Luiz: Pendrive removido\n");
}

// Tabela de IDs suportados
static struct usb_device_id pen_table[] = {
    { USB_DEVICE(0x0781, 0x5571) }, // Substitua pelos IDs corretos (lsusb)
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

// Inicializacao do driver
static int __init pen_init(void) {
    int ret = -1;
    printk(KERN_INFO "[*] Luiz: Construtor do driver\n");
    printk(KERN_INFO "\tRegistrando driver com o kernel\n");
    ret = usb_register(&pen_driver);
    if (ret == 0) {
        printk(KERN_INFO "\tRegistro concluido\n");
    } else {
        printk(KERN_ERR "\tFalha ao registrar driver\n");
    }
    return ret;
}

// Finalizacao do driver
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

