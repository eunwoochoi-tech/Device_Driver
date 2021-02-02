#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "pcd_platform.h"

struct pcd_device_private_data pcd_private_data;
struct pcd_platform_device 

struct platform_driver* pcd_platform_driver = {
	.probe = pcd_platform_probe,
	.remove = pcd_platform_remove,
	.driver = {
		.name = "pcd_driver"
	}
};

int pcd_platform_probe(struct platform_device* device)
{

}

int pcd_platform_remove(struct platform_device* device)
{

}

static int __init pcd_platform_driver_init(void)
{

	return 0;
}

static void __exit pcd_platform_driver_exit(void)
{

}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("pcd platform driver");
MODULE_AUTHOR("EUNWOO");
