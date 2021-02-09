obj-m := main.o

#ARCH=arm
#CROSS_COMPILE=arm-linux-gnueabihf-
KPATH=/lib/modules/$(shell uname -r)/build

all:
	make -C $(KPATH) M=$(shell pwd) modules
