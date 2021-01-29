obj-m := main.o
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KPATH=/root/kernel_source/raspberrypi-linux

all:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C ${KPATH} M=$(shell pwd) modules
