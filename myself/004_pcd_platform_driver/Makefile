obj-m := platform_device.o platform_driver.o

KPATH=/root/kernel_source/BBB-linux

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KPATH) M=$(shell pwd) modules
send:
	scp platform_driver.ko platform_device.ko debian@192.168.219.107:/home/debian
clean:
	make -C $(KPATH) M=$(shell pwd) clean
