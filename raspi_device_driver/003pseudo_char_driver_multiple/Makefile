obj-m := main.o
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KPATH=/root/kernel_source/raspberrypi-linux

CC=${CROSS_COMPILE}gcc

all:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C ${KPATH} M=$(shell pwd) modules
test:
	${CC} -o ./test_module/test ./test_module/test.c
clean:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C ${KPATH} M=$(shell pwd) clean
