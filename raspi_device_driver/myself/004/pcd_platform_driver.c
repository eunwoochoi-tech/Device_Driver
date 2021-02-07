#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "pcd_platform.h"

<<<<<<< HEAD
loff_t llseek (struct file* filp, loff_t f_pos, int count);
ssize_t read (struct file* filp, char __user* buf, size_t count, loff_t* f_pos);
ssize_t write (struct file* filp, const char __user* buf, size_t count, loff_t* f_pos);
int open (struct inode* inode, struct file* filp);
int release (struct inode* inode, struct file* filp);

struct file_operations f_ops = {
	.open = open,
	.read = read,
	.write = write,
	.release = release,
	.llseek = llseek,
	.owner = THIS_MODULE
};
=======
int pcd_platform_probe(struct platform_device* device);
int pcd_platform_remove(struct platform_device* device);
>>>>>>> 32ea72da566c3ef9bce1b71d94ff28d8ccdae91a

struct pcd_driver_private_data pcd_driver_data;

struct platform_driver pcd_platform_driver = {
	.probe = pcd_platform_probe,
	.remove = pcd_platform_remove,
	.driver = {
		.name = "pcd_dev_platform"
	}
};

int pcd_platform_probe(struct platform_device* device)
{
	struct pcd_platform_data* pdata;
	struct pcd_device_private_data* pdev;

	// 1. Get platform device data
	pdata = device->dev.platform_data;

	// 2. Alloc device private data
	pdev = (struct pcd_device_private_data*)kzalloc(sizeof(struct pcd_device_private_data), GFP_KERNEL);

	// 3. set device private data
	pdev->perm = pdata->perm;
	pdev->buf_size = pdata->buf_size;
	pdev->serial_num = pdata->serial_num;
	pdev->dev_num = pcd_driver_data.dev_num + device->id;

	pdev->buf = (char*)kzalloc(pdev->buf_size, GFP_KERNEL);

	// 4. cdev 초기화
	cdev_init(&pdev->cdev, f_ops);
	pdev->cdev.owner = THIS_MODULE;

	cdev_add(&pdev->cdev, pdev->dev_num, NUM_OF_DEV);

	// 5. device file 생성
	pcd_driver_data.pcd_device = device_create(pcd_driver_data.pcd_class, NULL, pdev->dev_num, NULL, "pcd-%d", device->id);

	// 6. 다른 함수에서 사용할 수 있도록 플랫폼 디바이스 변수에 device private data를 넣어주기
	device->dev.driver_data = pdev;

	return 0;
}

int pcd_platform_remove(struct platform_device* device)
{
	struct device_private_data* dev_data = device->dev.driver_data;

	device_destroy(pcd_driver_data.pcd_device, dev_data->dev_num);
	
	return 0;
}

static int __init pcd_platform_driver_init(void)
{
	int ret;

<<<<<<< HEAD
	// alloc dev number
	ret = alloc_chrdev_region()
=======
	// 1. alloc device number
	ret = alloc_chrdev_region(&pcd_driver_data.dev_num, 0, NUM_OF_DEV, "pcd_dev");
	if(ret < 0)
	{
		pr_info("alloc_chrdev_region() error \n");
		return ret;
	}

	// 2. create class
	pcd_driver_data.pcd_class = class_create(THIS_MODULE, "pcd_class");
	if(IS_ERR(pcd_driver_data.pcd_class))
	{
		pr_info("class_create() error \n");
		unregister_chrdev_region(pcd_driver_data.dev_num, NUM_OF_DEV);
		ret = PTR_ERR(pcd_driver_data.pcd_class);
		return ret;
	}

	// 3. Register Platform driver
	ret = platform_driver_register(&pcd_platform_driver);
>>>>>>> 32ea72da566c3ef9bce1b71d94ff28d8ccdae91a

	return 0;
}

static void __exit pcd_platform_driver_exit(void)
{

}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("pcd platform driver");
MODULE_AUTHOR("EUNWOO");
