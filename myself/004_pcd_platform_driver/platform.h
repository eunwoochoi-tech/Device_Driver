#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>

#define NUM_OF_DEV	3
#define RDONLY		0x01
#define WRONLY		0x10
#define RDWR		0x11

const char* pdev_name = "my_pcd_device";

struct platform_device_data
{
	const char* serial_name;
	int size;
	int perm;
};

struct driver_data
{
	int total_devices;
	int base_dev_num;
	struct class* drv_class;
	struct device* drv_device;
};

struct device_data
{
	struct platform_device_data pdev_data;
	dev_t dev_num;
	const char* buf;
	struct cdev cdev;
};

