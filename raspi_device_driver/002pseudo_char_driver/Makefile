obj-m := main.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KPATH=/lib/modules/$(shell uname -r)/build
#KPATH=/root/kernel_source/raspberrypi-linux
PWD=$(pwd)

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) clean
