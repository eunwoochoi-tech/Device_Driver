#ifndef _PCD_PLATFORM_DRIVER_H_
#define _PCD_PLATFORM_DRIVER_H_

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

#define NUM_OF_DEV	4
#define RDONLY		0x01
#define WRONLY		0x10
#define RDWR		0x11

loff_t llseek(struct file*, loff_t, int);
ssize_t read(struct file*, char __user*, size_t, loff_t*);
ssize_t write(struct file*, const char __user*, size_t, loff_t*);
int open(struct inode*, struct file*);
int release(struct inode*, struct file*);
ssize_t max_size_show(struct device*, struct device_attribute*, char*);
ssize_t max_size_store(struct device*, struct device_attribute*, const char*, size_t);
int get_data_from_dt(struct device*);
int pdrv_probe(struct platform_device*);
int pdrv_remove(struct platform_device*);

typedef struct _SDeviceData
{
	char _label[20];
} SDeviceData;

typedef struct _SDriverData
{
	int _totalDevices;
	struct class* _pClass;
} SDriverData;
#endif

