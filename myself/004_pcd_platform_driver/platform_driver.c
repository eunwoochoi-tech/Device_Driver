#include "platform.h"

int drv_probe(struct platform_device*);
int drv_remove(struct platform_device*);
loff_t llseek(struct file *, loff_t, int);
ssize_t read(struct file *, char __user *, size_t, loff_t *);
ssize_t write(struct file *, const char __user *, size_t, loff_t *);
int open(struct inode *, struct file *);
int release(struct inode *, struct file *);

struct platform_driver pdrv = {
	.probe = drv_probe,
	.remove = drv_remove,
	.driver.name = "my_pcd_device"
};

struct file_operations f_ops = {
	.owner = THIS_MODULE,
	.read = read,
	.write =  write,
	.open = open,
	.llseek = llseek,
	.release = release
};

struct driver_data drv_data;

int static __init platform_driver_init(void)
{
	int ret;
	ret = alloc_chrdev_region(&drv_data.base_dev_num, 0, NUM_OF_DEV, "pcdev");
	if(ret < 0)
	{
		pr_info("alloc_chrdev_region error! \n");
		return ret;
	}

	drv_data.drv_class = class_create(THIS_MODULE, "pcdev_class");
	if(ret < 0)
	{
		pr_info("class_create error! \n");
		unregister_chrdev_region(drv_data.base_dev_num, NUM_OF_DEV);
		return ret;
	}

	platform_driver_register(&pdrv);

	return 0;
}

void static __exit platform_driver_exit(void)
{
	platform_driver_unregister(&pdrv);

	class_destroy(drv_data.drv_class);

	unregister_chrdev_region(drv_data.base_dev_num, NUM_OF_DEV);
}

int drv_probe(struct platform_device* pdev)
{
	int ret;
	struct device_data* dev_data;
	struct device* dev;
	struct platform_device_data* pdev_data;

	pr_info("platform driver probe function start \n");

	dev = &pdev->dev;
	pdev_data = (struct platform_device_data*)dev->platform_data;
	if(!pdev_data)
	{
		pr_info("There is no platform device data \n");
		return -EINVAL;
	}
	dev_data = (struct device_data*)devm_kzalloc(dev, sizeof(struct device_data), GFP_KERNEL);
	if(!dev)
	{
		pr_info("There is no device data \n");
		return -EINVAL;
	}
	
	pr_info("dev : %p \n", dev);
	pr_info("dev_data : %p \n", dev_data);
	pr_info("pdev_data: %p \n", pdev_data);
	pr_info("pdev_data->size : %d \n", pdev_data->size);
	pr_info("pdev_data->perm : %d \n", pdev_data->perm);
	pr_info("pdev_data->serial_name : %s \n", pdev_data->serial_name);

	pr_info("[+] debug 1 \n");
	dev_data->pdev_data.serial_name = pdev_data->serial_name;
	dev_data->pdev_data.size = pdev_data->size;
	dev_data->pdev_data.perm = pdev_data->perm;
	dev_data->dev_num = drv_data.base_dev_num + pdev->id;
	dev_data->buf = (char*)devm_kzalloc(dev, dev_data->pdev_data.size, GFP_KERNEL);
	if(!dev_data->buf)
	{
		pr_info("devm_kzalloc error \n");
		ret = -ENOMEM;
		return ret;
	}	
	pr_info("[+] debug 2 \n");

	cdev_init(&dev_data->cdev, &f_ops);
	dev_data->cdev.owner = THIS_MODULE;

	pr_info("[+] debug 3 \n");
	ret = cdev_add(&dev_data->cdev, dev_data->dev_num, 1);
	if(ret < 0)
	{
		pr_info("cdev_add error \n");
		return ret;
	}
	pr_info("[+] debug 4 \n");

	dev->driver_data = dev_data;
	
	pr_info("platform driver probe function end \n");

	return 0;
}

int drv_remove(struct platform_device* pdev)
{
	struct device* dev = &pdev->dev;
	struct device_data* dev_data = (struct device_data*)dev->driver_data;
	
	cdev_del(&dev_data->cdev);

	pr_info("platform driver remove function start \n");
	pr_info("platform driver remove function end \n");
	return 0;
}

loff_t llseek(struct file* filp, loff_t offset, int whence)
{

	return 0;
}

ssize_t read(struct file* filp, char __user* buf, size_t count, loff_t* offset)
{

	return 0;
}

ssize_t write(struct file* filp, const char __user* buf, size_t count, loff_t* offest)
{

	return 0;
}

int open(struct inode* inode, struct file* filp)
{
	return 0;
}

int release(struct inode* inode, struct file* filp)
{
	return 0;
}



module_init(platform_driver_init);
module_exit(platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("my platform driver test");
MODULE_AUTHOR("EUNWOO");
