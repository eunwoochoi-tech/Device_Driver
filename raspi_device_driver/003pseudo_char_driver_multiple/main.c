#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt, __func__

#define DEVICE_NUM				4
#define MEM_SIZE_MAX_PCDDEV1	1024
#define MEM_SIZE_MAX_PCDDEV2	512
#define MEM_SIZE_MAX_PCDDEV3	1024
#define MEM_SIZE_MAX_PCDDEV4	512

loff_t pcd_lseek(struct file * filp, loff_t offest, int when);
ssize_t pcd_read(struct file * filp, char __user * buf, size_t count, loff_t * f_pos);
ssize_t pcd_write(struct file * filp, const char __user * buf, size_t count, loff_t * f_pos);
int pcd_open(struct inode * inode, struct file * filp);
int pcd_release(struct inode * inode, struct file * filp);

/* pseudo device's memory */
char device_buffer_pcddev1[MEM_SIZE_MAX_PCDDEV1];
char device_buffer_pcddev2[MEM_SIZE_MAX_PCDDEV2];
char device_buffer_pcddev3[MEM_SIZE_MAX_PCDDEV3];
char device_buffer_pcddev4[MEM_SIZE_MAX_PCDDEV4];

/* device private struct */
struct pcd_dev_private_data
{
	char* buffer;
	unsigned int size;
	const char* serial_number;
	int perm;
	struct cdev cdev;
};

/* driver private struct */
struct pcd_drv_private_data
{
	dev_t dev_num;
	int total_devices_num;
	struct class* class_pcd; 
	struct device* device_pcd;
	struct pcd_dev_private_data pcd_dev_data[DEVICE_NUM];
};

/* device private struct create */

struct pcd_drv_private_data pcd_drv_data = 
{
	.total_devices_num = DEVICE_NUM,
	.pcd_dev_data = {
		[0] = {
				.buffer = device_buffer_pcddev1,
				.size = MEM_SIZE_MAX_PCDDEV1,
				.serial_number = "pcd_dev_1",
				.perm = 0x01 /* READ ONLY */
		}, 
		[1] = {
				.buffer = device_buffer_pcddev2,
				.size = MEM_SIZE_MAX_PCDDEV2,
				.serial_number = "pcd_dev_2",
				.perm = 0x10 /* WRITE ONLY */
		}, 
		[2] = {
				.buffer = device_buffer_pcddev3,
				.size = MEM_SIZE_MAX_PCDDEV3,
				.serial_number = "pcd_dev_3",
				.perm = 0x11 /* READ WRITE */
		}, 
		[3] = {
				.buffer = device_buffer_pcddev4,
				.size = MEM_SIZE_MAX_PCDDEV4,
				.serial_number = "pcd_dev_4",
				.perm = 0x11 /* READ WRITE */
		} 
	}
};


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


loff_t pcd_lseek(struct file * filp, loff_t offset, int when)
{
	#if 0
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
	#endif
	return 0;
}

ssize_t pcd_read(struct file * filp, char __user * buf, size_t count, loff_t * f_pos)
{
	#if 0
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
	#endif
	return 0;
}

ssize_t pcd_write(struct file * filp, const char __user * buf, size_t count, loff_t * f_pos)
{
	#if 0
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
	#endif
	return -ENOMEM;
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
	int i;
	int ret;

	/* Dynamically register a range of char device numbers */
	ret = alloc_chrdev_region(&pcd_drv_data.dev_num, 0, DEVICE_NUM, "pcd");
	if(ret < 0)
	{
		pr_err("Alloc chrdev failed\n");
		goto alloc_chrdev_region_error;
	}

	for(i = 0; i < DEVICE_NUM; i++)
	{
		pr_info("Device number <major>:<minor> = %d:%d \n", MAJOR(pcd_drv_data.dev_num+i), MINOR(pcd_drv_data.dev_num+i));
	}

	/* Create a directory in /sys/class ex: /sys/class/class_pcd*/
	pcd_drv_data.class_pcd = class_create(THIS_MODULE,  "pcd_class");
	if(IS_ERR(pcd_drv_data.class_pcd))
	{
		pr_err("Class creation failed! \n");
		ret = PTR_ERR(pcd_drv_data.class_pcd);
		goto class_create_error;
	}

	/* Initialize the cdev structure with fopts */
	for(i = 0; i < DEVICE_NUM; i++)
	{
		cdev_init(&pcd_drv_data.pcd_dev_data[i].cdev, &pcd_fops);
		pcd_drv_data.pcd_dev_data[i].cdev.owner = THIS_MODULE;
	
		/* register cdev structure to VFS */
		ret = cdev_add(&pcd_drv_data.pcd_dev_data[i].cdev, pcd_drv_data.dev_num + i, 1);
		if(ret < 0)
		{	
			pr_err("cdev add failed\n");
			goto cdev_add_error;
		}
	
		/* Create Subdirectory under /sys/class/my_class_name and generate uevent: /sys/class/class_pcd/my_device_name/dev */
		pcd_drv_data.device_pcd = device_create(pcd_drv_data.class_pcd, NULL, pcd_drv_data.dev_num + i, NULL, "pcd-%d", i+1);
		if(IS_ERR(pcd_drv_data.device_pcd))
		{
			pr_err("Device create failed! \n");
			ret = PTR_ERR(pcd_drv_data.device_pcd);
			goto device_create_error;
		}	
	}
	pr_info("Module Init Success! \n");

	return 0;

device_create_error:
	class_destroy(pcd_drv_data.class_pcd);

class_create_error:
	for(i = 0; i < DEVICE_NUM; i++)
	{
		cdev_del(&pcd_drv_data.pcd_dev_data[i].cdev);	
	}

cdev_add_error:
	unregister_chrdev_region(pcd_drv_data.dev_num, DEVICE_NUM);

alloc_chrdev_region_error:
	return ret;
}

static void __exit pcd_driver_cleanup(void)
{
	#if 0
	device_destroy(class_pcd, dev_num);
	class_destroy(class_pcd);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("Module unloaded");
	#endif
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("A pseudo character driver which handles a device");
