#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include "platform.h"

void release(struct device* dev);

struct platform_device pdev[NUM_OF_DEV];
struct platform_device_data pdev_data[NUM_OF_DEV] = {
	[0] = {
		.serial_name = "platform_device_1",
		.size = 512,
		.perm = RDONLY
	},
	[1] = {
		.serial_name = "platform_device_2",
		.size = 512,
		.perm = WRONLY
	},
	[2] = {
		.serial_name = "platform_device_3",
		.size = 1024,
		.perm = RDWR
	}
};

static int __init platform_device_init(void)
{
	int i;
	pr_info("platform_device_init start \n");

	for(i = 0; i < NUM_OF_DEV; i++)
	{
		pdev[i].name = pdev_name;
		pdev[i].id = i;
		pdev[i].dev.platform_data = &pdev_data[i];
		pdev[i].dev.release = release;
		platform_device_register(&pdev[i]);
	}

	pr_info("platform_device_init end \n");
	return 0;
}

static void __exit platform_device_exit(void)
{
	int i;
	pr_info("platform_device_exit start \n");

	for(i = 0; i < NUM_OF_DEV; i++)
	{
		platform_device_unregister(&pdev[i]);
	}

	pr_info("platform_device_exit end \n");
}

void release(struct device* dev)
{
	pr_info("release function start \n");
	
	pr_info("\"%s\" dev released \n", ((struct platform_device_data*)dev->platform_data)->serial_name);

	pr_info("release function end \n");
}

module_init(platform_device_init);
module_exit(platform_device_exit);

MODULE_AUTHOR("EUNWOO");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("my_self_study");
