obj-m := pcd_platform_driver_dt.o

KPATH=/lib/modules/$(shell uname -r)/build
KPATH_PI=/root/kernel_source/raspberrypi-linux

all:
	make -C $(KPATH) M=$(shell pwd) modules
pi:
	make -C $(KPATH_PI) M=$(shell pwd) modules
clean:
	make -C $(KPATH) M=$(shell pwd) clean
