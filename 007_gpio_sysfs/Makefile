target_obj=gpio_sysfs.o
objs=pcd_platform_driver.o pcd_platform_driver_sysfs_dt.o

obj-m := gpio_sysfs.o
gpio_sysfs-objs := pcd_platform_driver.o pcd_platform_driver_sysfs_dt.o

#KPATH=/root/kernel_module/BBB_Linux_5.4
KPATH=/root/kernel_source/BBB-linux
KPATH_PI=/lib/modules/$(shell uname -r)/build
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) modules
pi:
	make -C $(KPATH_PI) M=$(shell pwd) modules
send:
	scp $(basename $(target_obj)).ko debian@192.168.219.107:/home/debian
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) clean 
	
