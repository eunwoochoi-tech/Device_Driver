obj-m := pcd_device_setup.o
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KPATH=/root/kernel_source/raspberrypi-linux

all:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C ${KPATH} M=$(shell pwd) modules
host:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
clean:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C ${KPATH} M=$(shell pwd) clean
