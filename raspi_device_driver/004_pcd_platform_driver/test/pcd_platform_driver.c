#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include "platform.h"

int pcd_platform_driver_probe(struct platform_device*);
int pcd_platform_driver_remove(struct platform_device*);
loff_t llseek(struct file*, loff_t, int);
ssize_t read(struct file*, char __user*, size_t, loff_t*);
ssize_t write(struct file*, const char __user*, size_t, loff_t*);
int open(struct inode*, struct file*);
int release(struct inode*, struct file*);

struct platform_driver pcd_platform_driver = {
	.probe = pcd_platform_driver_probe,
	.remove = pcd_platform_driver_remove,
	.driver = {
		.name = "pcd_dev"
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

int pcd_platform_driver_probe(struct platform_device* pdev)
{
	int ret;

	struct pcd_device_private_data* pcd_dev_data;
	// struct pcd_platform_data* pdata = (struct pcd_platform_data*)dev_get_platdata(&pdev->dev);
	struct pcd_platform_data* pdata = pdev->dev.platform_data;
	if(!pdata)
	{
		pr_info("No platform data available \n");
		ret = -EINVAL;
		return ret;
	}

	pcd_dev_data = (struct pcd_device_private_data*)kzalloc(sizeof(struct pcd_device_private_data), GFP_KERNEL);
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

	pcd_dev_data->buffer = (char*)kzalloc(pcd_dev_data->pcd_pdata.size, GFP_KERNEL);
	if(!pcd_dev_data->buffer)
	{
		pr_info("Cannot allocate memory \n");
		ret = -ENOMEM;
		goto dev_data_free;
	}

	pcd_dev_data->dev_num = pcd_drv_data.base_dev_num + pdev->id;

	cdev_init(&pcd_dev_data->pcd_cdev, &f_ops);
	pcd_dev_data->pcd_cdev.owner = THIS_MODULE;

	ret = cdev_add(&pcd_dev_data->pcd_cdev, pcd_dev_data->dev_num, 1);
	if(ret < 0)
	{
		pr_info("cdev add failed \n");
		goto buffer_free;
	}

	pcd_drv_data.pcd_device = device_create(pcd_drv_data.pcd_class, NULL, pcd_dev_data->dev_num, NULL, "pcd-%d", pdev->id);
	if(IS_ERR(pcd_drv_data.pcd_device))
	{
		pr_err("Device create failed \n ");
		ret = PTR_ERR(pcd_drv_data.pcd_device);
		goto class_del;
	}
	pcd_drv_data.total_devices++;

	pr_info("a device is detected \n");	
	return 0;

class_del:
	cdev_del(&pcd_dev_data->pcd_cdev);
buffer_free:
	kfree(pcd_dev_data->buffer);

dev_data_free:
	kfree(pcd_dev_data);
	return ret;
}

int pcd_platform_driver_remove(struct platform_device* pdev)
{
	struct pcd_device_private_data* dev_data = dev_get_drvdata(&pdev->dev);
	// device
	device_destroy(pcd_drv_data.pcd_class, dev_data->dev_num);

	// cdev
	cdev_del(&dev_data->pcd_cdev);

	// buffer
	kfree(dev_data->buffer);
	kfree(dev_data);

	// device_data
	pcd_drv_data.total_devices--;

	pr_info("a device is removed \n");
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


module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("pcd_platform_driver");
MODULE_AUTHOR("EUNWOO");
