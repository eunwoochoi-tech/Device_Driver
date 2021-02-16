obj-m := platform_device.o platform_driver.o

#KPATH=/srv/nfs/BBB_linux
KPATH=/root/kernel_module/BBB_Linux_5.4
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) modules
send:
	scp platform_driver.ko platform_device.ko debian@192.168.219.107:/home/debian
clean:
	make -C $(KPATH) M=$(shell pwd) clean
