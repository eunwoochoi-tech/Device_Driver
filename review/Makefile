obj-m := chardev.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
#KERN_PATH=/root/kernel_source/BBB-linux
KERN_PATH=/lib/modules/$(shell uname -r)/build

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_PATH) M=$(shell pwd)
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_PATH) M=$(shell pwd) clean
