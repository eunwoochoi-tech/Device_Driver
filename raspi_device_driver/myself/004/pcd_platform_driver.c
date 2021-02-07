#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "pcd_platform.h"

loff_t llseek (struct file* filp, loff_t f_pos, int count);
ssize_t read (struct file* filp, char __user* buf, size_t count, loff_t* f_pos);
ssize_t write (struct file* filp, const char __user* buf, size_t count, loff_t* f_pos);
int open (struct inode* inode, struct file* filp);
int release (struct inode* inode, struct file* filp);

struct file_operations f_ops = {
	.open = open,
	.read = read,
	.write = write,
	.release = release,
	.llseek = llseek,
	.owner = THIS_MODULE
};

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
	int ret;

	// alloc dev number
	ret = alloc_chrdev_region()

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
