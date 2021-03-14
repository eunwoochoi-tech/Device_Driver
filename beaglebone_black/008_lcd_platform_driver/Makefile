ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

OBJS=lcd.o gpio.o lcd_driver.o
SRCS=lcd_driver.c lcd.c gpio.c

TARGET=lcd_driver

KERN_PATH=/root/kernel_source/BBB-linux

obj-m := $(TARGET).o
$(TARGET)-objs := gpio.o lcd.o

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_PATH)  M=$(shell pwd) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_PATH)  M=$(shell pwd) clean
send:
	scp $(TARGET).ko $(KERN_PATH)/arch/arm/boot/dts/am335x-boneblack.dtb debian@192.168.219.107:/home/debian
dt:
	cp am335x-boneblack-lcd.dtsi $(KERN_PATH)/arch/arm/boot/dts
	make -C $(KERN_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) am335x-boneblack.dtb
