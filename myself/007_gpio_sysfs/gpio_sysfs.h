#ifndef __GPIO_SYSFS_H_
#define __GPIO_SYSFS_H_

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>

struct _SDeviceData {
	char _label[100];
	struct gpio_desc* _pGpioDesc;

};

struct _SDriverData {
	struct class* _class;

};

int platDrv_probe(struct platform_device *);
int platDrv_remove(struct platform_device *);

#endif
