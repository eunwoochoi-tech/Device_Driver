#include "platform_driver.h"

int pdrv_probe(struct platform_device*);
int pdrv_remove(struct platform_device*);

loff_t llseek_fops(struct file*, loff_t, int);
ssize_t read_fops(struct file*, char __user*, size_t, loff_t*);
ssize_t write_fops(struct file*, const char __user*, size_t, loff_t*);
int open_fops(struct inode*, struct file*);
int release_fops(struct inode*, struct file*);

struct file_operations f_ops = {
	.owner = THIS_MODULE,
	.open = open_fops,
	.read = read_fops,
	.write = write_fops,
	.llseek = llseek_fops,
	.release = release_fops
};

struct of_device_id pdev_of_dev_id[] = {
	[0] = { .compatible = "pcd_dev-A1x", .data = (void*)0 },
	[1] = { .compatible = "pcd_dev-B1x", .data = (void*)1 },
	[2] = { .compatible = "pcd_dev-C1x", .data = (void*)2 },
	[3] = { .compatible = "pcd_dev-D1x", .data = (void*)3 }
};

struct platform_driver pdrv = {
	.probe = pdrv_probe,
	.remove = pdrv_remove,
	.driver = {
		.name = "psudo",
		.of_match_table = pdev_of_dev_id
	}
};

struct driver_data drv_data;
struct device_data* dev_data;

static int __init platform_driver_dt_init(void)
{
	pr_info("platform-driver_dt_init start \n");

	// 1. device number register
	alloc_chrdev_region(&drv_data.base_dev_num, 0, NUM_OF_DEV, "pcdev");

	// 2. define class
	drv_data.class = class_create(THIS_MODULE, "pcdev_cass");

	// 3. register platform driver
	platform_driver_register(&pdrv);

	pr_info("platform-driver_dt_init end \n");
	return 0;
}

static void __exit platform_driver_dt_exit(void)
{
	pr_info("platform-driver_dt_exit start \n");

	platform_driver_unregister(&pdrv);
	
	pr_info("platform-driver_dt_exit end \n");
}

struct platform_device_data* get_platform_device_data_from_dt(struct platform_device* pdev)
{
	struct device* dev;
	struct platform_device_data* pdev_data;

	dev = &pdev->dev;
	pdev_data = devm_kzalloc(dev, sizeof(struct platform_device_data), GFP_KERNEL);
	if(!pdev_data)
	{
		dev_info(dev, "devm_kzalloc error \n");
		return ERR_PTR(-ENOMEM);
	}

	of_property_read_string(dev->of_node, "org,serial-number", &pdev_data->serial_number);
	of_property_read_u32(dev->of_node, "org,size", &pdev_data->size);
	of_property_read_u32(dev->of_node, "org,perm", &pdev_data->perm);

	return pdev_data;
}

loff_t llseek_fops(struct file* filp, loff_t offset, int whence)
{
	return 0;
}

ssize_t read_fops(struct file* filp, char __user* buf, size_t count, loff_t* offset)
{
	return 0;
}

ssize_t write_fops(struct file* filp, const char __user* buf, size_t count, loff_t* offset)
{
	return -EFAULT;
}

int open_fops(struct inode* inode, struct file* filp)
{
	return 0;
}

int release_fops(struct inode* inode, struct file* filp)
{
	return 0;
}

int pdrv_probe(struct platform_device* pdev)
{
	int ret;
	struct device* dev;
	struct platform_device_data* pdev_data;

	dev_info(dev, "pdrv_probe start \n");

	dev = &pdev->dev;
	if(!dev)
	{
		pr_err("device structure is null \n");
		ret = -EINVAL;
		return ret;
	}

	dev_data = (struct device_data*)devm_kzalloc(dev, sizeof(struct device_data), GFP_KERNEL);
	if(!dev_data)
	{
		pr_err("devm_kzalloc error \n");
		ret = -ENOMEM;
		return ret;
	}

	// if dt is declared
	if(dev->of_node)
	{
		pdev_data = get_platform_device_data_from_dt(pdev);
	}
	else if(dev->platform_data)
	{
		pdev_data = (struct platform_data*)dev->platform_data;
		driver_data = pdev->id_entry->driver_data;
	}
	else
	{

	}

	dev_info(dev, "pdrv_probe end \n");

	return 0;
}

int pdrv_remove(struct platform_device* pdev)
{
	struct device* dev;

	dev = &pdev->dev;
	
	dev_info(dev, "pdrv_remove start \n");

	dev_info(dev, "pdrv_remove end \n");
	return 0;
}

module_init(platform_driver_dt_init);
module_exit(platform_driver_dt_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("platform driver device tree test");
