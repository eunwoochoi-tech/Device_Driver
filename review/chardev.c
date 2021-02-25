#include "chardev.h"

int open(struct inode* pINode, struct file* pFile)
{

	return 0;
}

ssize_t read(struct file* pFile, char __user* buf, size_t count, loff_t* pOffset)
{

	return 0;
}

ssize_t write(struct file* pFile, const char __user* buf, size_t count, loff_t* pOffset)
{

	return -EINVAL;
}

loff_t llseek(struct file* pFile, loff_t offset, int whence)
{
	return 0;

}

int release(struct inode* pINode, struct file* pFile)
{
	return 0;
}

struct _SCharDri charDri;
struct _SCharDev* charDev;

struct file_operations fOps = {
	.open = open,
	.read = read,
	.write = write,
	.llseek = llseek,
	.release = release
};


static int __init chardev_init(void)
{
	int ret;
	int i = 0;

	ret = alloc_chrdev_region(&charDri._baseDevNum, 0, NUM_OF_DEV, "charDev");
	if(ret)
	{
		pr_err("alloc_chrdev_region error \n");
		return -EINVAL;
		//goto free;
	}

	charDri._class = class_create(THIS_MODULE, "chrdev_class");
	if(IS_ERR(charDri._class))
	{
		pr_err("class_create error \n");
		return -EINVAL;
	//	goto cdev_destroy;
	}

	for(i = 0; i < NUM_OF_DEV; i++)
	{
		charDev = kzalloc(sizeof(struct _SCharDev), GFP_KERNEL);
		if(!charDev)
		{
			pr_err("kzalloc error \n");
			return -ENOMEM;
		}
	
		charDev->_devNum = charDri._baseDevNum + i;

		cdev_init(&charDev->_cdev, &fOps);
		charDev->_cdev.owner = THIS_MODULE;
		ret = cdev_add(&charDev->_cdev, charDev->_devNum, 1);
		if(ret)
		{
			pr_err("cdev_add error \n");
			return -EINVAL;
			// goto unregister_chrdev;
		}
		charDri._device[i] = device_create(charDri._class, NULL, charDev->_devNum, charDev, "charDev-%d", i);
		
	}


	return 0;

};

static void __exit chardev_exit(void)
{
	int i;
	for(i = 0; i < NUM_OF_DEV; i++)
	{
		device_destroy(charDri._class, charDri._baseDevNum + i);
		cdev_del(&((struct _SCharDev*)(charDri._device[i]->driver_data))->_cdev);
	}
	class_destroy(charDri._class);

	unregister_chrdev_region(charDri._baseDevNum, NUM_OF_DEV);
};

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("hello");
