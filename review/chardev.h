#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define BUF_SIZE	1024
#define NUM_OF_DEV	5

struct _SCharDev {
	dev_t _devNum;
	char _buf[BUF_SIZE];
	struct cdev _cdev;
};

struct _SCharDri {
	int _totalDevices;
	dev_t _baseDevNum;
	struct class* _class;
	struct device* _device[NUM_OF_DEV];
};
