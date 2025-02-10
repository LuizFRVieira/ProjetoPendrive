obj-m := pendrive_driver.o

# Nome do módulo (sem a extensão .ko)
MODULE_NAME := pendrive_driver

# Arquivo do módulo (com extensão .ko)
MODULE_FILE := $(MODULE_NAME).ko

# Diretório do kernel
KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Alvo padrão (compilar o módulo)
all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

# Limpeza
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~

# Remover o módulo
remove:
	@if lsmod | grep -q $(MODULE_NAME); then \
		echo "Removendo módulo $(MODULE_NAME)..."; \
		sudo rmmod $(MODULE_NAME); \
	else \
		echo "Módulo $(MODULE_NAME) não está carregado."; \
	fi

# Inserir o módulo
insert:
	@if [ -f $(MODULE_FILE) ]; then \
		echo "Inserindo módulo $(MODULE_FILE)..."; \
		sudo insmod $(MODULE_FILE); \
	else \
		echo "Arquivo $(MODULE_FILE) não encontrado!"; \
		exit 1; \
	fi

# Alterar permissões e executar echo
setup:
	@chmod 644 /dev/$(MODULE_NAME)
	@echo "Configurando módulo com echo..."
	@echo "valor_para_escrever" > /dev/$(MODULE_NAME)

# Executar interface gráfica
interface:
	python3 gui_send_path.py

# Executar todas as etapas
run: all remove insert setup interface
	@echo "Módulo $(MODULE_NAME) compilado, removido, inserido e configurado com sucesso."

.PHONY: all clean remove insert setup interface run
