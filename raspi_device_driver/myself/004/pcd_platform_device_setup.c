#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "pcd_platform.h"

int pcd_platform_dev_alloc(void);
int pcd_platform_dev_register(void);
void pcd_platform_dev_unregister(void);
void pcd_platform_dev_free(void);

struct pcd_platform_data pcd_platform_data[NUM_OF_DEV] = {
	[0] = { .buf_size = 1024, .perm = WRONLY, .serial_num = "PCD_DEV_0" },
	[1] = { .buf_size = 1024, .perm = RDONLY, .serial_num = "PCD_DEV_1" },
	[2] = { .buf_size = 512, .perm = RDWR, .serial_num = "PCD_DEV_2" },
	[3] = { .buf_size = 512, .perm = RDWR, .serial_num = "PCD_DEV_3" },
};
struct platform_device* pcd_platform_dev[NUM_OF_DEV];

void pcd_platform_dev_release(struct device* dev)
{
	pr_info("platform->dev.release function \n");
}

int pcd_platform_dev_alloc(void)
{
	int i;
	pr_info("pcd_platform_dev_alloc start \n");
	
	for(i = 0; i < NUM_OF_DEV; i++)
	{
		pcd_platform_dev[i] = (struct platform_device*)kzalloc(sizeof(struct platform_device), GFP_KERNEL);
		if(!pcd_platform_dev[i])
		{
			pr_info("pcd_platform_dev_alloc failed  \n");
			return -ENOMEM;
		}
		pcd_platform_dev[i]->name = "pcd_dev_platform";
		pcd_platform_dev[i]->id = i;
		pcd_platform_dev[i]->dev.platform_data = &pcd_platform_data[i];
		pcd_platform_dev[i]->dev.release = pcd_platform_dev_release;
	}
	pr_info("pcd_platform_dev_alloc success \n");
	return 0;
}

int pcd_platform_dev_register(void)
{
	int i;
	pr_info("pcd_platform_dev_register start \n");

	for(i = 0; i < NUM_OF_DEV; i++)
	{
		if(!pcd_platform_dev[i])
		{
			pr_info("Invaild platform device num : %d \n", i);
			pcd_platform_dev_unregister();
			return -EINVAL;			
		}
		platform_device_register(pcd_platform_dev[i]);
	}
	pr_info("pcd_platform_dev_register success \n");
	return 0; 
}

void pcd_platform_dev_unregister(void)
{
	int i;
	pr_info("pcd_platform_dev_unregister start \n");

	for(i = 0; i < NUM_OF_DEV; i++)
	{
		platform_device_unregister(pcd_platform_dev[i]);
	}
	pr_info("pcd_platform_dev_unregister success \n");
}

void pcd_platform_dev_free(void)
{
	int i;
	pr_info("pcd_platform_dev_free start \n");

	for(i = 0; i < NUM_OF_DEV; i++)
	{
		kzfree(pcd_platform_dev[i]);
	}
	pr_info("pcd_platform_dev_free success \n");
}

static int __init pcd_platform_dev_init(void)
{
	int ret;

	// 1. Allocate struct device
	ret = pcd_platform_dev_alloc();
	if(ret < 0)
	{
		pr_info("pcd_platform_dev_alloc error \n");
		return ret;
	}

	// 2. Register struct deivce
	ret = pcd_platform_dev_register();
	if(ret < 0)
	{
		pr_info("pcd_platform_dev_register error \n");
		return ret;
	}
	return 0;
}

static void __exit pcd_platform_dev_exit(void)
{
	pcd_platform_dev_unregister();
	pcd_platform_dev_free();
}

module_init(pcd_platform_dev_init);
module_exit(pcd_platform_dev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("my self test");
MODULE_AUTHOR("EUNWOO");
