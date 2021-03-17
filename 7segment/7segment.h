#ifndef __7SEGMENT_H_
#define __7SEGMENT_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/gpio/driver.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/gpiolib.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/delay.h>

#define NUM_OF_PINS	7

#define ACTIVE_LOW	1
#define ACTIVE_HIGH	0

#define INPUT		0
#define OUTPUT		1

enum { A, B, C, D, E, F, G };

struct _SDeviceDriver {
	struct class* _pClass;
	struct device* _pDevice;
	struct gpio_desc* _pGpioDesc[NUM_OF_PINS];
};

int platProbe(struct platform_device*);
int platRemove(struct platform_device*);
extern int gpio_configure_dir(struct gpio_desc*, int mode, int value);
int create_device_attrs(struct device*, const struct attribute_group**);

ssize_t number_show(struct device*, struct device_attribute*, char*);
ssize_t number_store(struct device*, struct device_attribute*, const char*, size_t);

#endif
