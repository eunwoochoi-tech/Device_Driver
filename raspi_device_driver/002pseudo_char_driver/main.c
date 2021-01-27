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

loff_t pcd_lseek(struct file * filp, loff_t offset, int when)
{
	loff_t temp;

	pr_info("lseek requested \n");
	pr_info("current file position = %lld \n", filp->f_pos);

	switch(when)
	{
		case SEEK_SET:
			if((offset > DEV_MEM_SIZE) || (offset < 0))
			{
				return -EINVAL;
			}
			filp->f_pos = offset;
			break;
		case SEEK_CUR:
			temp = filp->f_pos + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0))
			{
				return -EINVAL;
			}
			filp->f_pos = temp;
			break;
		case SEEK_END:
			temp = DEV_MEM_SIZE + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0))
			{
				return -EINVAL;
			}
			filp->f_pos = DEV_MEM_SIZE;
			break;
		default:
			return -EINVAL;
	}

	pr_info("new value if the file position = %lld \n", filp->f_pos);
	
	return filp->f_pos;
}

ssize_t pcd_read(struct file * filp, char __user * buf, size_t count, loff_t * f_pos)
{
	pr_info("read requested for %zu bytes \n", count);
	pr_info("current file position : %lld \n", *f_pos);

	/* Adjust count */
	if((*f_pos + count) > DEV_MEM_SIZE)
	{
		count = DEV_MEM_SIZE - *f_pos;
	}	

	/* copy to user buf */
	if(copy_to_user(buf, &device_buffer[*f_pos], count))
	{
		return -EFAULT;
	}

	/* update the current file position */
	*f_pos += count;

	pr_info("Number of bytes successfully read = %zu \n", count);
	pr_info("Updated file position = %lld \n", *f_pos);

	/* return number of bytes which have benn successfully read */
	return count;
}

ssize_t pcd_write(struct file * filp, const char __user * buf, size_t count, loff_t * f_pos)
{
	pr_info("write requested for %zu bytes \n", count);
	pr_info("current file position : %lld \n", *f_pos);

	/* Adjus count */
	if((*f_pos + count) > DEV_MEM_SIZE)
	{
		count = DEV_MEM_SIZE - *f_pos;
	}

	if(!count)
	{
		pr_err("No space left on the device \n");
		return -ENOMEM;
	}

	/* copy from user buf */
	if(copy_from_user(&device_buffer[*f_pos], buf, count))
	{
		return -EFAULT;
	}

	/* update the current file position */
	*f_pos += count;

	pr_info("Number of bytes successfully write = %zu \n", count);
	pr_info("Updated file position = %lld \n", *f_pos);

	/* return number of bytes which have benn successfully write */
	return count;
}

int pcd_open(struct inode * inode, struct file * filp)
{
	pr_info("open was successful \n");
	return 0;
}

int pcd_release(struct inode * inode, struct file * filp)
{
	pr_info("release was successful \n");
	return 0;
}

/* cdev variable */

static int __init pcd_driver_init(void)
{
	int ret;
	/* Dynamically register a range of char device numbers */
	ret = alloc_chrdev_region(&dev_num, 0, 1, "pcd");
	if(ret < 0)
	{
		pr_err("Alloc chrdev failed\n");
		goto alloc_chrdev_region_error;
	}
	pr_info("Device number <major>:<minor> = %d:%d \n", MAJOR(dev_num), MINOR(dev_num));

	/* Initialize the cdev structure with fopts */
	cdev_init(&pcd_cdev, &pcd_fops);
	pcd_cdev.owner = THIS_MODULE;

	/* register cdev structure to VFS */
	ret = cdev_add(&pcd_cdev, dev_num, 1);
	if(ret < 0)
	{	
		pr_err("cdev add failed\n");
		goto cdev_add_error;
	}
	/* Create a directory in /sys/class ex: /sys/class/class_pcd*/
	class_pcd = class_create(THIS_MODULE,  "pcd_class");
	if(IS_ERR(class_pcd))
	{
		pr_err("Class creation failed! \n");
		ret = PTR_ERR(class_pcd);
		goto class_create_error;
	}

	/* Create Subdirectory under /sys/class/my_class_name and generate uevent: /sys/class/class_pcd/my_device_name/dev */
	device_pcd = device_create(class_pcd, NULL, dev_num, NULL, "pcd");
	if(IS_ERR(device_pcd))
	{
		pr_err("Device create failed! \n");
		ret = PTR_ERR(device_pcd);
		goto device_create_error;
	}	
	pr_info("Module Init Success! \n");

	return 0;

device_create_error:
	class_destroy(class_pcd);

class_create_error:
	cdev_del(&pcd_cdev);	

cdev_add_error:
	unregister_chrdev_region(dev_num, 1);

alloc_chrdev_region_error:
	return ret;
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
