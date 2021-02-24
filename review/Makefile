obj-m := kernel_module.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERN_PATH=/root/kernel_source/BBB-linux

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_PATH) M=$(shell pwd)
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_PATH) M=$(shell pwd) clean
