#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define NUM_OF_DEV		4

#define FMODE_READ			0x01
#define FMODE_WRITE			0x10
#define FMODE_RDWR			0x11

#define PCD_DEV1_BUF_SIZE	1024
#define PCD_DEV2_BUF_SIZE	512
#define PCD_DEV3_BUF_SIZE	1024
#define PCD_DEV4_BUF_SIZE	512

char pcd_dev1_buf[PCD_DEV1_BUF_SIZE];
char pcd_dev2_buf[PCD_DEV2_BUF_SIZE];
char pcd_dev3_buf[PCD_DEV3_BUF_SIZE];
char pcd_dev4_buf[PCD_DEV4_BUF_SIZE];

int open(struct inode* inode, struct file* filp);
ssize_t read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos);
ssize_t write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos);
loff_t llseek(struct file* filp, loff_t f_pos, int whence);
int release(struct inode* inode, struct file* filp);
int check_permission(int, int);

struct pcd_dev_private_data
{
	char* buf;
	int buf_size;
	int permission;
	struct cdev cdev;
};

struct pcd_drv_private_data
{
	dev_t dev_num;
	struct class* class;
	struct device* device;
	struct pcd_dev_private_data pcd_dev_data[NUM_OF_DEV];
};

struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = open,
	.read = read,
	.write = write,
	.llseek = llseek,
	.release = release
};

struct pcd_drv_private_data pcd_drv_data = 
{
	.pcd_dev_data =	{
		[0] = {
			.buf = pcd_dev1_buf,
			.buf_size = PCD_DEV1_BUF_SIZE,
			.permission = FMODE_READ
		},
		[1] = {
			.buf = pcd_dev2_buf,
			.buf_size = PCD_DEV2_BUF_SIZE,
			.permission = FMODE_RDWR
		},
		[2] = {
			.buf = pcd_dev3_buf,
			.buf_size = PCD_DEV3_BUF_SIZE,
			.permission = FMODE_WRITE
		},
		[3] =
		{
			.buf = pcd_dev4_buf,
			.buf_size = PCD_DEV4_BUF_SIZE,
			.permission = FMODE_RDWR
		}
	}
};

static int __init pcd_init(void)
{
	pr_info("init start\n");

	int ret;
	int i;

	pr_info("alloc_chrdev_region start\n");
	ret = alloc_chrdev_region(&pcd_drv_data.dev_num, 0, NUM_OF_DEV, "pcd");
	if(ret < 0)
	{
		pr_err("alloc_chrdev_region error\n");
		goto alloc_chrdev_region_error;
	}
	pr_info("alloc_chrdev_region end\n");

	pr_info("class_creat start\n");
	pcd_drv_data.class = class_create(THIS_MODULE, "pcd_class");
	if(IS_ERR(pcd_drv_data.class))
	{
		pr_err("class_create error\n");
		ret = PTR_ERR(pcd_drv_data.class);
		goto class_create_error;
	}	
	pr_info("class_creat end\n");

	for(i = 0; i < NUM_OF_DEV; i++)
	{
		cdev_init(&pcd_drv_data.pcd_dev_data[i].cdev, &fops);
		pcd_drv_data.pcd_dev_data[i].cdev.owner = THIS_MODULE;

		ret = cdev_add(&pcd_drv_data.pcd_dev_data[i].cdev, pcd_drv_data.dev_num + i, 1);
		if(ret < 0)
		{
			pr_err("cdev_add error\n");
			goto cdev_add_error;
		}

		pcd_drv_data.device = device_create(pcd_drv_data.class, NULL, pcd_drv_data.dev_num + i, NULL, "pcd-%d", i+1);
		if(IS_ERR(pcd_drv_data.device))
		{
			pr_err("device_create error\n");
			ret = PTR_ERR(pcd_drv_data.device);
			goto device_create_error;
		}
	}

	pr_info("init success \n");
	return 0;

device_create_error:
	class_destroy(pcd_drv_data.class);

class_create_error:
	for(i = 0; i < NUM_OF_DEV; i++)
	{
		cdev_del(&pcd_drv_data.pcd_dev_data[i].cdev);
	}

cdev_add_error:
	unregister_chrdev_region(pcd_drv_data.dev_num, NUM_OF_DEV);

alloc_chrdev_region_error:
	return ret;
}

static void __exit pcd_exit(void)
{
	pr_info("cleanup start \n");

	int i;
	for(i = 0; i < NUM_OF_DEV; i++)
	{
		device_destroy(pcd_drv_data.class, pcd_drv_data.dev_num + i);
		cdev_del(&pcd_drv_data.pcd_dev_data[i].cdev);
	}
	class_destroy(pcd_drv_data.class);
	unregister_chrdev_region(pcd_drv_data.dev_num, NUM_OF_DEV);
	
	pr_info("cleanup end \n");
}

int open(struct inode* inode, struct file* filp)
{
	int ret;
	struct pcd_dev_private_data* pcd_dev_data;
	pr_info("open function\n");

	pcd_dev_data = container_of(inode->i_cdev, struct pcd_dev_private_data, cdev);
	filp->private_data = pcd_dev_data;

	ret = check_permission(pcd_dev_data->permission, filp->f_mode);

	(!ret)?pr_info("open was successful\n"):pr_info("open was unsuccessful\n");

	return ret;
}

ssize_t read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos)
{
	pr_info("read function\n");

	int ret;
	struct pcd_dev_private_data* pcd_dev_data = (struct pcd_dev_private_data*)filp->private_data;

	pr_info("request to read %zu bytes\n", count);
	pr_info("current file pos is %llu \n", *f_pos);

	if((count + *f_pos) > pcd_dev_data->buf_size)
	{
		count = pcd_dev_data->buf_size - *f_pos;
	}

	ret = copy_to_user(buf, pcd_dev_data->buf, count);
	if(ret < 0)
	{
		pr_info("no more read data\n");
		return -EFAULT;
	}	

	*f_pos += count;

	pr_info("Number of bytes successfully read : %zu \n", count);
	pr_info("Updated file position : %lld \n", *f_pos);

	return count;
}

ssize_t write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos)
{
	pr_info("write function \n");

	int ret;
	struct pcd_dev_private_data* pcd_dev_data = (struct pcd_dev_private_data*)filp->private_data;

	pr_info("request to wite %zu bytes\n", count);
	pr_info("current file pos is %llu \n", *f_pos);

	if(count + *f_pos > pcd_dev_data->buf_size)
	{
		count = pcd_dev_data->buf_size - *f_pos;
	}

	ret = copy_from_user(pcd_dev_data->buf, buf, count);
	if(ret < 0)
	{
		return -EFAULT;
	}

	*f_pos += count;

	pr_info("Number of bytes successfully read : %zu \n", count);
	pr_info("Updated file position : %lld \n", *f_pos);

	return count;
}

loff_t llseek(struct file* filp, loff_t offset, int whence)
{
	pr_info("llseek function \n");

	int temp;
	struct pcd_dev_private_data* pcd_dev_data = (struct pcd_dev_private_data*)filp->private_data;

	switch(whence)
	{
		case SEEK_SET:
			if((offset > pcd_dev_data->buf_size) || (offset < 0))
			{
				return -EINVAL;
			}
			filp->f_pos = offset;
			break;
		case SEEK_CUR:
			temp = offset + filp->f_pos;
			if((temp > pcd_dev_data->buf_size) || (temp < 0))
			{
				return -EINVAL;
			}
			filp->f_pos = temp;
			break;
		case SEEK_END:
			temp = offset + pcd_dev_data->buf_size;
			if(temp > pcd_dev_data->buf_size || (temp < 0))
			{
				return -EINVAL;
			}
			filp->f_pos = temp;
			break;
		default:
			return -EINVAL;
	}
}

int release(struct inode* inode, struct file* filp)
{
	pr_info("release function \n");
	return 0;
}

int check_permission(int dev_perm, int acc_mode)
{
	if(dev_perm == FMODE_RDWR)
	{
		return 0;
	}

	if((dev_perm == FMODE_READ) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE)))
	{
		return 0;
	}

	if((dev_perm == FMODE_WRITE) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ)))
	{
		return 0;
	}

	return -EPERM;
}	

module_init(pcd_init);
module_exit(pcd_exit);

MODULE_AUTHOR ("milo");
MODULE_DESCRIPTION ("alloc_chrdev_troubleshooting");
MODULE_LICENSE("GPL");
