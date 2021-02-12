#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>

#define NUM_OF_DEV	4


struct device_data
{
	const char* buf;
	dev_t dev_num;
	struct cdev cdev;
};

struct driver_data
{
	int total_devices;
	dev_t base_dev_num;
	struct class* class;
	struct device* device;
};

struct platform_device_data
{
	const char* serial_number;
	int size;
	int perm;
};
