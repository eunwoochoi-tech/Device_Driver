obj-m := pcd_sysfs.o
pcd_sysfs-objs += pcd_platform_driver_dt_sysfs.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf- 
#KPATH=/lib/modules/$(shell uname -r)/build
KPATH=/root/kernel_source/BBB-linux
KPATH_PI=/root/kernel_source/raspberrypi-linux

all:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C $(KPATH) M=$(shell pwd) modules
pi:
	make -C $(KPATH_PI) M=$(shell pwd) modules
clean:
	make -C $(KPATH) M=$(shell pwd) clean
