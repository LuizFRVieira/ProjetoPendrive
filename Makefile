# Definir o módulo e o diretório do kernel
obj-m := stick_driver.o

# Diretório dos cabeçalhos do kernel
KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Alvo padrão - Compilação do módulo
all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

# Alvo para limpar os arquivos de compilação
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~

# Alvo para insmod e rmmod (carregar e descarregar o módulo)
load:
	sudo insmod stick_driver.ko

unload:
	sudo rmmod stick_driver

# Para visualizar as mensagens do driver
dmesg_logs:
	dmesg | grep Luiz
