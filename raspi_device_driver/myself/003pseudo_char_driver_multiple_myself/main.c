#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define NUM_OF_DEV		4

#define RDONLY			0x01
#define WRONLY			0x10
#define RDWR			0x11

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
			.permission = RDONLY
		},
		[1] = {
			.buf = pcd_dev2_buf,
			.buf_size = PCD_DEV2_BUF_SIZE,
			.permission = RDWR
		},
		[2] = {
			.buf = pcd_dev3_buf,
			.buf_size = PCD_DEV3_BUF_SIZE,
			.permission = WRONLY
		},
		[3] =
		{
			.buf = pcd_dev4_buf,
			.buf_size = PCD_DEV4_BUF_SIZE,
			.permission = RDWR
		}
	}
};

static int __init pcd_init(void)
{
	pr_info("init start\n");

	int ret, i;

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
	pr_info("open function \n");
	return 0;
}

ssize_t read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos)
{
	pr_info("read function \n");
	return -ENOMEM;
}

ssize_t write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos)
{
	pr_info("write function \n");
	return -ENOMEM;
}

loff_t llseek(struct file* filp, loff_t f_pos, int whence)
{
	pr_info("llseek function \n");
	return -ENOMEM;
}

int release(struct inode* inode, struct file* filp)
{
	pr_info("release function \n");
	return 0;
}


module_init(pcd_init);
module_exit(pcd_exit);

MODULE_AUTHOR ("milo");
MODULE_DESCRIPTION ("alloc_chrdev_troubleshooting");
MODULE_LICENSE("GPL");
