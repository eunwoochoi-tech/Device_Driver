obj-m := pcd_platform_device_setup.o

KPATH=/lib/modules/$(shell uname -r)/build

all:
	make -C $(KPATH) M=$(shell pwd) modules
clean:
	make -C $(KPATH) M=$(shell pwd) clean
