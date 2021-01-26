#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>

#undef pr_fmt
#define DEV_MEM_SIZE	512
#define pr_fmt(fmt) "%s :" fmt, __func__

loff_t pcd_lseek(struct file * filp, loff_t offest, int when);
ssize_t pcd_read(struct file * filp, char __user * buf, size_t count, loff_t * f_pos);
ssize_t pcd_write(struct file * filp, const char __user * buf, size_t count, loff_t * f_pos);
int pcd_open(struct inode * inode, struct file * filp);
int pcd_release(struct inode * inode, struct file * filp);

/* pseudo device's memory */
char device_buffer[DEV_MEM_SIZE];

/* This holds the device number */
dev_t dev_num;

/* cdev structure */
struct cdev pcd_cdev;

/* file operations variable */
struct file_operations pcd_fops =
{
	.llseek = pcd_lseek,
	.read = pcd_read,
	.write = pcd_write,
	.open = pcd_open,
	.release = pcd_release,
	.owner = THIS_MODULE
};

struct class* class_pcd; 

struct device* device_pcd;

loff_t pcd_lseek(struct file * filp, loff_t offest, int when)
{
	pr_info("lseek requested \n");
	return 0;
}

ssize_t pcd_read(struct file * filp, char __user * buf, size_t count, loff_t * f_pos)
{
	pr_info("read requested for %zu bytes \n", count);
	return 0;
}

ssize_t pcd_write(struct file * filp, const char __user * buf, size_t count, loff_t * f_pos)
{
	pr_info("write requested for %zu bytes \n", count);
	return 0;
}

int pcd_open(struct inode * inode, struct file * filp)
{
	pr_info("open was successful \n");
	return 0;
}

int pcd_release(struct inode * inode, struct file * filp)
{
	pr_info("close was successful \n");
	return 0;
}

/* cdev variable */

static int __init pcd_driver_init(void)
{
	/* Dynamically register a range of char device numbers */
	alloc_chrdev_region(&dev_num, 0, 1, "pcd");

	pr_info("Device number <major>:<minor> = %d:%d \n", MAJOR(dev_num), MINOR(dev_num));

	/* Initialize the cdev structure with fopts */
	cdev_init(&pcd_cdev, &pcd_fops);
	pcd_cdev.owner = THIS_MODULE;

	/* register cdev structure to VFS */
	cdev_add(&pcd_cdev, dev_num, 1);

	/* Create a directory in /sys/class ex: /sys/class/class_pcd*/
	class_pcd = class_create(THIS_MODULE,  "pcd_class");

	/* Create Subdirectory under /sys/class/my_class_name and generate uevent: /sys/class/class_pcd/my_device_name/dev */
	device_pcd = device_create(class_pcd, NULL, dev_num, NULL, "pcd");
	
	pr_info("Module Init Success! \n");

	return 0;
}

static void __exit pcd_driver_cleanup(void)
{
	device_destroy(class_pcd, dev_num);
	class_destroy(class_pcd);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("Module unloaded");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("A pseudo character driver");
