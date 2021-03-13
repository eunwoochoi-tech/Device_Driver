ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

OBJS=test_driver.o
SRCS=$(basename $(OBJS)).c

TARGET=test_driver

KERN_PATH=/root/kernel_source/BBB-linux

obj-m := $(TARGET).o

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_PATH)  M=$(shell pwd) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_PATH)  M=$(shell pwd) clean
send:
	scp test_driver.ko $(KERN_PATH)/arch/arm/boot/dts/am335x-boneblack.dtb debian@192.168.219.107:/home/debian
dt:
	cp test.dtsi $(KERN_PATH)/arch/arm/boot/dts
	make -C $(KERN_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) am335x-boneblack.dtb
