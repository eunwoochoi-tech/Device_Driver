#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>

#define DEV_MEM_SIZE	512

/* pseudo device's memory */
char device_buffer[DEV_MEM_SIZE];

/* This holds the device number */
dev_t dev_num;

/* file operations variable */
struct file_operations pcd_fops;

struct class* class_pcd; 
struct device* device_pcd;

loff_t pcd_lseek(struct file * filp, loff_t offest, int when)
{
	return 0;
}

ssize_t pcd_read(struct file * filp, char __user * buf, size_t count, loff_t * f_pos)
{

	return 0;
}

ssize_t pcd_write(struct file * filp, const char __user * buf, size_t count, loff_t * f_pos)
{
	return 0;

}

int pcd_open(struct inode * inode, struct file * filp)
{
	return 0;

}

int pcd_release(struct inode * inode, struct file * filp)
{
	return 0;

}

/* cdev variable */
struct cdev pcd_cdev =
{
	.llseek = pcd_lseek,
	.read = pcd_read,
	.write = pcd_write,
	.open = pcd_open,
	.release = pcd_release,
	.owner = THIS_MODULE
};


static int __init pcd_driver_init(void)
{
	/* Dynamically register a range of char device numbers */
	alloc_chrdev_region(&dev_num, 0, 1, "pcd");

	/* Initialize the cdev structure with fopts */
	cdev_init(&pcd_cdev, &pcd_fops);
	pcd_cdev.owner = THIS_MODULE;

	/* register cdev structure to VFS */
	cdev_add(&pcd_cdev, dev_num, 1);

	class_pcd = class_create(THIS_MODULE,  "pcd_class");
	device_pcd = device_create(class_pcd, NULL, dev_num, NULL, "pcd");
	
	pr_info("Hello World! \n");


	return 0;
}

static void __exit pcd_driver_cleanup(void)
{

}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);
