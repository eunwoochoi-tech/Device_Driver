ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
#KPATH=/root/kernel_module/BBB_Linux_5.4
KPATH=/root/kernel_source/BBB-linux

obj-m := lcd.o
lcd-objs = main.o lcd_sysfs.o

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) clean

send:
	scp lcd.ko debian@192.168.219.107:/home/debian
