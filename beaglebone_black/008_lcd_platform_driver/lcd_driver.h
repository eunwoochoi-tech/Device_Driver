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
#include<linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/delay.h>
#include "common.h"

#undef	pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

struct _SLcdDriver{
	struct class* _pClass;
} lcdDriver;

int lcd_driver_probe(struct platform_device*);
int lcd_driver_remove(struct platform_device*);
struct attribute attr;
ssize_t lcdxy_show(struct device*, struct device_attribute*, char*);
ssize_t lcdxy_store(struct device*, struct device_attribute*, const char*, size_t);
ssize_t lcdcmd_store(struct device*, struct device_attribute*, const char*, size_t);
ssize_t lcdtext_store(struct device*, struct device_attribute*, const char*, size_t);

