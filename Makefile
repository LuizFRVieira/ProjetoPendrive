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
remover:
	@if lsmod | grep -q $(MODULE_NAME); then \
		echo "Removendo módulo $(MODULE_NAME)..."; \
		sudo rmmod $(MODULE_NAME); \
	else \
		echo "Módulo $(MODULE_NAME) não está carregado."; \
	fi

# Inserir o módulo
inserir:
	@if [ -f $(MODULE_FILE) ]; then \
		echo "Inserindo módulo $(MODULE_FILE)..."; \
		sudo insmod $(MODULE_FILE); \
		sleep 1;  # Aguardar 1 segundo para o dispositivo ser criado \
	else \
		echo "Arquivo $(MODULE_FILE) não encontrado!"; \
		exit 1; \
	fi

# Alterar permissões e executar echo

#trocar a permissao de leitura pra publico
setup:
	@if [ -e /dev/$(MODULE_NAME) ]; then \
		sudo chmod 644 /dev/$(MODULE_NAME); \
		echo "Configurando módulo com echo..."; \
		echo "valor_para_escrever" > /dev/$(MODULE_NAME); \
	else \
		echo "Dispositivo /dev/$(MODULE_NAME) não encontrado!"; \
		exit 1; \
	fi

# Executar interface gráfica
interface:
	python3 gui_send_path.py

# Executar todas as etapas
run: all remover inserir setup interface
	@echo "Módulo $(MODULE_NAME) compilado, removido, inserido e configurado com sucesso."

.PHONY: all clean remover inserir setup interface run
