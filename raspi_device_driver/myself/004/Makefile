obj-m := pcd_platform_device_setup.o pcd_platform_driver.o

KPATH=/lib/modules/$(shell uname -r)/build

all:
	make -C $(KPATH) M=$(shell pwd) modules
clean:
	make -C $(KPATH) M=$(shell pwd) clean
