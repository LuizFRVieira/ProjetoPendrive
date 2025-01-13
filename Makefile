obj-m := stick_driver.o

# Diretório do kernel
KERNEL_DIR := /usr/src/linux-headers-6.8.0-51-generic
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~
