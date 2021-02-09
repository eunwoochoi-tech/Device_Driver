#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>

#define NUM_OF_DEV		1
#define DEV_MEM_SIZE	1024

loff_t llseek(struct file* filp, loff_t offset, int whence);
ssize_t read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos);
ssize_t write(struct file* filp, const char __user*, size_t count, loff_t* f_pos);
int open(struct inode* inode, struct file* filp);
int release(struct inode* inode, struct file* filp);

char pcd_buf[DEV_MEM_SIZE];

/* device number */
dev_t dev_num;

/* cdev struct */
struct cdev pcd_cdev;

/* file_operations struct */
struct file_operations pcd_fops = 
{
	.open = open,
	.read = read,
	.write = write,
	.llseek = llseek,
	.release = release
};

/* class for /sys/class/... */
struct class* pcd_class;

/* device file */
struct device* pcd_device;

loff_t llseek(struct file* filp, loff_t offset, int whence)
{
	int temp;
	switch(whence)
	{
		case SEEK_SET:
			if(offset > DEV_MEM_SIZE || offset < 0)
			{
				return -EINVAL;
			}
			filp->f_pos = offset;
			break;
		case SEEK_CUR:
			temp = filp->f_pos + offset;
			if(temp > DEV_MEM_SIZE || temp < 0)
			{
				return -EINVAL;
			}
			filp->f_pos = temp;
			break;
		case SEEK_END:
			temp = DEV_MEM_SIZE + offset;
			if(temp > DEV_MEM_SIZE || temp < 0)
			{
				return -EINVAL;
			}
			filp->f_pos = temp;
			break;
		default:
			return -EINVAL;
	}
	
	return filp->f_pos;
}

ssize_t read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos)
{
	if(*f_pos + count > DEV_MEM_SIZE)
	{
		count = DEV_MEM_SIZE - *f_pos;
	}

	if(copy_to_user(buf, &pcd_buf[filp->f_pos], count))
	{
		return -EFAULT;
	}

	*f_pos += count;
	
	return count;
}

ssize_t write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos)
{
	if(*f_pos + count > DEV_MEM_SIZE)
	{
		count = DEV_MEM_SIZE - *f_pos;
	}

	if(!count)
	{
		pr_err("No space left \n");
		return -ENOMEM;
	}

	if(copy_from_user(&pcd_buf[filp->f_pos], buf, count))
	{
		return -EFAULT;
	}

	*f_pos += count;

	return count;
}

int open(struct inode* inode, struct file* filp)
{
	pr_info("open success \n");
	return 0;
}

int release(struct inode* inode, struct file* filp)
{
	pr_info("release success \n");
	return 0;
}

static int __init pcd_init(void)
{
	/* device number allocation */
	int ret = alloc_chrdev_region(&dev_num, 0, NUM_OF_DEV, "pcd_dev");
	if(ret < 0)
	{
		pr_err("alloc_chrdev_region() error \n");
		goto alloc_chrdev_region_error;
	}
	

	/* cdev init  */
	cdev_init(&pcd_cdev, &pcd_fops);
	
	/* register cdev to VFS */
	ret = cdev_add(&pcd_cdev, dev_num, NUM_OF_DEV);
	if(ret < 0)
	{
		pr_err("cdev_add() error \n");
		goto cdev_add_error;
	}
	
	/* create device file */
	pcd_class = class_create(THIS_MODULE, "pcd_class");
	if(IS_ERR(pcd_class))
	{
		pr_err("class_create() error \n");
		ret = PTR_ERR(pcd_class);
		goto class_create_error;
	}
	
	/* request to generate device file */
	pcd_device = device_create(pcd_class, NULL, dev_num, NULL, "pcd");
	if(IS_ERR(pcd_device))
	{
		pr_err("device_create() error \n");
		ret = PTR_ERR(pcd_device);
		goto device_create_error;
	}

	return 0;

device_create_error:
	class_destroy(pcd_class);

class_create_error:
	cdev_del(&pcd_cdev);

cdev_add_error:
	unregister_chrdev_region(dev_num, NUM_OF_DEV);

alloc_chrdev_region_error:
	return ret;
}

static void __exit pcd_cleanup(void)
{
	device_destroy(pcd_class, dev_num);
	class_destroy(pcd_class);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(dev_num, NUM_OF_DEV);
}


module_init(pcd_init);
module_exit(pcd_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("device driver pcd");

