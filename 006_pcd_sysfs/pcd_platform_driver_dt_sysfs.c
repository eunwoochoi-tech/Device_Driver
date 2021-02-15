#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include "platform.h"

int pcd_platform_driver_probe(struct platform_device*);
int pcd_platform_driver_remove(struct platform_device*);
loff_t llseek(struct file*, loff_t, int);
ssize_t read(struct file*, char __user*, size_t, loff_t*);
ssize_t write(struct file*, const char __user*, size_t, loff_t*);
int open(struct inode*, struct file*);
int release(struct inode*, struct file*);
ssize_t max_size_show(struct device*, struct device_attribute*, char*);
ssize_t max_size_store(struct device*, struct device_attribute*, const char*, size_t);
ssize_t serial_number_show(struct device*, struct device_attribute*, char*);


enum PCD_DEV_NAMES
{
	PCD_DEV_A1X,
	PCD_DEV_B1X,
	PCD_DEV_C1X,
	PCD_DEV_D1X
};

struct of_device_id pcd_dev_dt_match[] = 
{
	{.compatible = "pcd_dev-A1x", .data = (void*)PCD_DEV_A1X},
	{.compatible = "pcd_dev-B1x", .data = (void*)PCD_DEV_B1X},
	{.compatible = "pcd_dev-C1x", .data = (void*)PCD_DEV_C1X},
	{.compatible = "pcd_dev-D1x", .data = (void*)PCD_DEV_D1X},
};

struct platform_device_id pcd_dev_ids[] = 
{
	[PCD_DEV_A1X] = { .name = "pcd_dev-A1x", .driver_data = PCD_DEV_A1X },
	[PCD_DEV_B1X] = { .name = "pcd_dev-B1x", .driver_data = PCD_DEV_B1X },
	[PCD_DEV_C1X] = { .name = "pcd_dev-C1x", .driver_data = PCD_DEV_C1X },
	[PCD_DEV_D1X] = { .name = "pcd_dev-D1x", .driver_data = PCD_DEV_D1X }
};
struct platform_driver pcd_platform_driver = {
	.probe = pcd_platform_driver_probe,
	.remove = pcd_platform_driver_remove,
	.id_table = pcd_dev_ids,
	.driver = {
		.name = "psudo",
		.of_match_table = of_match_ptr(pcd_dev_dt_match)
	}
};

struct file_operations f_ops = {
	.owner = THIS_MODULE,
	.open = open,
	.write = write,
	.read = read,
	.llseek = llseek,
	.release = release
};

struct pcd_driver_private_data pcd_drv_data;

struct device_config pcd_dev_config[] = {
	[0] = {.config_item1 = 60, .config_item2 = 21},
	[1] = {.config_item1 = 50, .config_item2 = 22},
	[2] = {.config_item1 = 40, .config_item2 = 23},
	[3] = {.config_item1 = 30, .config_item2 = 24}
};

static DEVICE_ATTR(max_size, S_IRUGO | S_IWUSR, max_size_show, max_size_store);
static DEVICE_ATTR(serial_number, S_IRUGO, serial_number_show, NULL);

static int __init pcd_platform_driver_init(void)
{
	int ret;

	// 1. allocate device number
	ret = alloc_chrdev_region(&pcd_drv_data.base_dev_num, 0, 3, "pcd_drv");
	if(ret < 0)
 	{
		pr_info("Alloc chrdev failed \n");
		return ret;
	}

	// 2. Create device class
	pcd_drv_data.pcd_class = class_create(THIS_MODULE, "pcd_class");
	if(IS_ERR(pcd_drv_data.pcd_class))
	{
		pr_info("class_create error \n");
		ret = PTR_ERR(pcd_drv_data.pcd_class);
		unregister_chrdev_region(pcd_drv_data.base_dev_num, 3);
		return ret;
	}

	ret = platform_driver_register(&pcd_platform_driver);

	pr_info("pcd platform driver load \n");
	return 0;

}

static void __exit pcd_platform_driver_cleanup(void)
{
	platform_driver_unregister(&pcd_platform_driver);

	class_destroy(pcd_drv_data.pcd_class);

	unregister_chrdev_region(pcd_drv_data.base_dev_num, 3);

	pr_info("pcd platform driver unload \n");
}

struct pcd_platform_data* pcd_dev_get_platdata_from_dt(struct device* dev)
{
	struct pcd_platform_data *pdata;
	struct device_node* dev_node = dev->of_node; // device_node present DT
	if(!dev_node)
	{
		return NULL;
	}

	pdata = devm_kzalloc(dev, sizeof(struct pcd_platform_data), GFP_KERNEL);
	if(!pdata)
	{
		dev_info(dev, "Cannot allocate memory \n");
		return ERR_PTR(-ENOMEM);
	}
	if(of_property_read_string(dev_node, "org,serial-number", &pdata->serial_number))
	{
		dev_info(dev, "Missing serial number property \n");
		return ERR_PTR(-EINVAL);
	}

	if(of_property_read_u32(dev_node, "org,size", &pdata->size))
	{
		dev_info(dev, "Missing size operation \n");
		return ERR_PTR(-EINVAL);
	}
	if(of_property_read_u32(dev_node, "org,perm", &pdata->perm))
	{
		dev_info(dev, "Missing perm operation \n");
		return ERR_PTR(-EINVAL);
	}

	return pdata;
}

int pcd_sysfs_create_files(struct device* pcd_dev)
{
	int ret;
	ret = sysfs_create_file(&pcd_dev->kobj, &dev_attr_max_size.attr);
	if(ret)
		return ret;
	return sysfs_create_file(&pcd_dev->kobj, &dev_attr_serial_number.attr);
	
}

int pcd_platform_driver_probe(struct platform_device* pdev)
{
	int ret;
	int driver_data;
	struct of_device_id* match;
	struct pcd_device_private_data* pcd_dev_data;
	struct pcd_platform_data* pdata;
	struct device* dev = &pdev->dev;
	// struct pcd_platform_data* pdata = (struct pcd_platform_data*)dev_get_platdata(&pdev->dev);

	match = of_match_device(of_match_ptr(pcd_dev_dt_match), dev);

	if(match)
	{
		pdata = pcd_dev_get_platdata_from_dt(dev);
		if(IS_ERR(pdata))
		{
			return PTR_ERR(pdata);
		}
		driver_data = pdev->id_entry->driver_data;
	}
	else
	{
		driver_data = pdev->id_entry->driver_data;
	}

	if(!pdata)
	{
		dev_info(dev, "No platform data available \n");
		ret = -EINVAL;
		return ret;
	}
	else
	{
		// to get driver_data(index) from device(dt), we must use of_match_device function
		// driver_data = (int)of_device_get_match_data(&pdev->dev);
		match = of_match_device(pdev->dev.driver->of_match_table, dev);
		driver_data = (int)match->data;
	}

	pcd_dev_data = devm_kzalloc(&pdev->dev, sizeof(struct pcd_device_private_data), GFP_KERNEL);

	if(!pcd_dev_data)
	{
		pr_info("Kernel alloc failed! \n" );
		ret = -ENOMEM;
		return ret;
	}
	// pdev->dev.driver_data = dev_set_drvdata(&pdev->dev, pcd_dev_data);
	pdev->dev.driver_data = pcd_dev_data;

	// memcpy(pcd_dev_data->pdata, pdata, sizeof(struct pcd_platform_data));
	pcd_dev_data->pcd_pdata.size = pdata->size;	
	pcd_dev_data->pcd_pdata.perm = pdata->perm;	
	pcd_dev_data->pcd_pdata.serial_number = pdata->serial_number;	

	pr_info("Device Serial number : %s \n", pcd_dev_data->pcd_pdata.serial_number);
	pr_info("Device perm : %d \n", pcd_dev_data->pcd_pdata.perm);
	pr_info("Device size : %d \n", pcd_dev_data->pcd_pdata.size);

	pr_info("Config item 1 = %d \n", pcd_dev_config[driver_data].config_item1);
	pr_info("Config item 2 = %d \n", pcd_dev_config[driver_data].config_item2);

	pcd_dev_data->buffer = devm_kzalloc(&pdev->dev, pcd_dev_data->pcd_pdata.size, GFP_KERNEL);
	if(!pcd_dev_data->buffer)
	{
		pr_info("Cannot allocate memory \n");
		ret = -ENOMEM;
		goto dev_data_free;
	}

	pcd_dev_data->dev_num = pcd_drv_data.base_dev_num + pcd_drv_data.total_devices;

	cdev_init(&pcd_dev_data->pcd_cdev, &f_ops);
	pcd_dev_data->pcd_cdev.owner = THIS_MODULE;

	ret = cdev_add(&pcd_dev_data->pcd_cdev, pcd_dev_data->dev_num, 1);
	if(ret < 0)
	{
		pr_info("cdev add failed \n");
		goto buffer_free;
	}

	pcd_drv_data.pcd_device = device_create(pcd_drv_data.pcd_class, dev, pcd_dev_data->dev_num, NULL, "pcd-%d", pcd_drv_data.total_devices);
	if(IS_ERR(pcd_drv_data.pcd_device))
	{
		pr_err("Device create failed \n ");
		ret = PTR_ERR(pcd_drv_data.pcd_device);
		goto class_del;
	}
	pcd_drv_data.total_devices++;

	ret = pcd_sysfs_create_files(pcd_drv_data.pcd_device);
	if(ret)
	{
		//device_destroy(pcd_drv_data.pcd_class, dev_data->dev_num);
		return ret;
	}

	pr_info("a device is detected \n");	
	return 0;

class_del:
	cdev_del(&pcd_dev_data->pcd_cdev);
buffer_free:
	devm_kfree(&pdev->dev, pcd_dev_data->buffer);
dev_data_free:
	devm_kfree(&pdev->dev, pcd_dev_data);
	return ret;
}

int pcd_platform_driver_remove(struct platform_device* pdev)
{
	struct pcd_device_private_data* dev_data = dev_get_drvdata(&pdev->dev);
	// device
	device_destroy(pcd_drv_data.pcd_class, dev_data->dev_num);

	// cdev
	cdev_del(&dev_data->pcd_cdev);

	// device_data
	pcd_drv_data.total_devices--;
	dev_info(&pdev->dev, "a device is removed \n");
	return 0;
}

loff_t llseek(struct file* filp, loff_t count, int offset)
{

	return 0;
}

ssize_t read(struct file* filp, char __user* buf, size_t count, loff_t* offset)
{

	return 0;
}

ssize_t write(struct file* filp, const char __user* buf, size_t count, loff_t* offset)
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

ssize_t max_size_show(struct device* dev, struct device_attribute* attr, char* buf)
{

}

ssize_t max_size_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{

}

ssize_t serial_number_show(struct device* dev, struct device_attribute* attr, char* buf)
{

}


module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("pcd_platform_driver");
MODULE_AUTHOR("EUNWOO");
