obj_file=platform_driver_dt.o
target=platform_driver_dt.ko

obj-m := $(obj_file) 

KPATH_BBB=/root/kernel_source/BBB-linux

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KPATH_BBB) M=$(shell pwd) modules
send:
	scp -P 7777 $(target) debian@112.155.213.28:/home/debian
clean : 
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KPATH_BBB) M=$(shell pwd) clean
