#ifndef _LCD_SYSFS_H_
#define _LCD_SYSFS_H_

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

int platDrvProbe(struct platform_device *);
int platDrvRemove(struct platform_device *);
ssize_t direction_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t direction_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
ssize_t value_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t value_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
ssize_t label_show(struct device *dev, struct device_attribute *attr, char *buf);

struct _SLcdDrvData {
	int _totalDevices;
	struct class* _class;
	struct device** _ppDev;
}; 

struct _SLcdDevData {
	const char* _label;
	struct gpio_desc* _pGpioDesc;
};


#endif
