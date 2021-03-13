#include <linux/module.h>
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
#include <linux/gpio/gpiolib.h>

#define TRUE	1
#define FALSE	0

#define HIGH	1
#define LOW		0

typedef struct _SDeviceDriver {
	int _totalDevices;
	struct class* _pClass;
	struct device** _ppDevs;
} deviceDriver;

int platDrv_probe(struct platform_device*);
int platDrv_remove(struct platform_device*);
ssize_t direction_show(struct device* dev, struct device_attribute* attr, char* buf);
ssize_t value_show(struct device* dev, struct device_attribute* attr, char* buf);
ssize_t label_show(struct device* dev, struct device_attribute* attr, char* buf);
ssize_t direction_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);
ssize_t value_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);
