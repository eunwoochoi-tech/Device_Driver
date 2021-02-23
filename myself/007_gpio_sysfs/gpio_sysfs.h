#ifndef _GPIO_SYSFS_H_
#define _GPIO_SYSFS_H_

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

#define HIGH	1
#define LOW		0

int platDrv_probe(struct platform_device *);
int platDrv_remove(struct platform_device *);
ssize_t direction_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf);
ssize_t value_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf);
ssize_t label_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf);
ssize_t direction_store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count);
ssize_t value_store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count);

struct _SDeviceData {
	char _label[100];
	struct gpio_desc* _pGpioDesc;
};

struct _SDriverData {
	int _totalDevices;
	struct class* _class;
	struct device** _ppDev;
};

static DEVICE_ATTR_RW(direction);
static DEVICE_ATTR_RW(value);
static DEVICE_ATTR_RO(label);

static struct  attribute* pAttr[] = {
	&dev_attr_direction.attr,
	&dev_attr_value.attr,
	&dev_attr_label.attr
};

static struct attribute_group attrGroup = {
	.attrs = pAttr
};

static const struct attribute_group* pAttrGroup[] = {
	&attrGroup
};

#endif
