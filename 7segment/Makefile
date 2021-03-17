ARCH = arm
CROSS_COMPILE = arm-linux-gnueabihf-
DTSI_FILE = am335x-boneblack-7segment.dtsi
KERN_PATH = /root/kernel_source/BBB-linux/

obj-m := segment.o
segment-objs = gpio.o 7segment.o

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_PATH) M=$(shell pwd) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_PATH) M=$(shell pwd) clean

dtb:
	make -C $(KERN_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) am335x-boneblack.dtb
cp:
	cp $(DTSI_FILE) $(KERN_PATH)/arch/arm/boot/dts/
send:
	scp $(KERN_PATH)/arch/arm/boot/dts/am335x-boneblack.dtb segment.ko debian@192.168.219.107:/home/debian
