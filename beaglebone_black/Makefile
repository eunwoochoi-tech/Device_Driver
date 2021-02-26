ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KPATH=/root/kernel_module/BBB_Linux_5.4

obj-m := lcd.o
lcd-objs = main.o lcd_sysfs.o

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) clean
