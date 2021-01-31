#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__
 
#define MAX_DEVICES	3

struct pcd_dev_private_data
{
	struct pcd_dev_platform_data pdata;
	char* buf;
	dev_t dev_num;
	struct cdev cdev;
};

struct pcd_drv_private_data
{
	int total_devices;
	dev_t device_num_base;
	struct class* class_pcd;
	struct device* device_pcd;
};

struct pcd_drv_private_data pcd_drv_data;

loff_t llseek(struct file* filp, loff_t f_pos, int count);
ssize_t read(struct file*, char __user*, size_t, loff_t*);
ssize_t write(struct file*, const char __user*, size_t, loff_t*);
int open(struct inode*, struct file*);
int release(struct inode*, struct file*);
int check_permission(int dev_perm, int acc_mode);

int pcd_platform_driver_probe(struct platform_device* pdev)
{
	pr_info("a device is detected \n");	
	return 0;
}

int pcd_platform_driver_remove(struct platform_device* pdev)
{
	pr_info("a device is removed \n");	
	return 0;
}

struct file_operations f_ops = {
	.open = open,
	.release = release,
	.read = read,
	.write = write,
	.llseek = llseek,
	.owner = THIS_MODULE
};

struct platform_driver pcd_dev_platform_driver = {
	.probe = pcd_platform_driver_probe,
	.remove = pcd_platform_driver_remove,
	.driver = {
		.name = "pcd-device"
	}
};


static int __init pcd_platform_driver_init(void)
{
	int ret;

	/* 1. Dynamically allocate a device number for MAX_DEVICES */
	ret = alloc_chrdev_region(&pcd_drv_data.device_num_base,0,MAX_DEVICES,"pcd_devs");	
	if(ret < 0)
	{
		pr_err("alloc_chrdev_region() error! \n");
		return ret;
	}

	/* 2. Create device class under /sys/class */
	pcd_drv_data.class_pcd = class_create(THIS_MODULE, "pcd_class");
	if(IS_ERR(pcd_drv_data.class_pcd))
	{
		pr_err("class_create() error!\n");
		ret = PTR_ERR(pcd_drv_data.class_pcd);
		unregister_chrdev_region(pcd_drv_data.device_num_base, MAX_DEVICES);
		return ret;
	}
	
	/* 3. Register a platform driver */
	platform_driver_register(&pcd_dev_platform_driver);
	pr_info("pcd platform driver loaded \n");
	return 0;
}

static void __exit pcd_platform_driver_exit(void)
{
	/* 1. Unregister the platform driver */
	platform_driver_unregister(&pcd_dev_platform_driver);

	/* 2. Destroy class */
	class_destroy(pcd_drv_data.class_pcd);

	/* 3. Unregister device numbers */
	unregister_chrdev_region(pcd_drv_data.device_num_base, MAX_DEVICES);

	pr_info("pcd platform driver unloaded \n");
}

loff_t llseek(struct file* filp, loff_t f_pos, int count)
{
	return 0;
}

ssize_t read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos)
{
	return 0;
}

ssize_t write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos)
{
	return -ENOMEM;
}

int open(struct inode* inode, struct file* filp)
{
	return 0;
}

int release(struct inode* inode, struct file* filp)
{
	return 0;
}

int check_permission(int dev_perm, int acc_mode)
{
	if(dev_perm == RDWR)
		return 0;
	if(dev_perm == RDONLY && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE)))
		return 0;
	if(dev_perm == WRONLY && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ)))
		return 0;

	return -EPERM;	
}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("pcd platfrom driver");
