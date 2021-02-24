TARGET_OBJ = gpio.o
OBJS = gpio_sysfs.o gpio_main.o

obj-m := $(TARGET_OBJ)
$(basename $(TARGET_OBJ))-objs = $(OBJS)

#KPATH=/root/kernel_source/BBB-linux
KPATH=/root/kernel_module/BBB_Linux_5.4
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KPATH) M=$(shell pwd) clean
send:
	scp -P 7777 gpio.ko debian@112.155.213.28:/home/debian
