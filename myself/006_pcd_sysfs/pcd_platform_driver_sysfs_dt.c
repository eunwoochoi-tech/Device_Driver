#include "pcd_platform_driver.h"

extern struct of_device_id ofDevId[NUM_OF_DEV];
extern struct platform_driver pDrv;
extern struct _SDriverData drvData;
extern struct device_attribute device_attr_max_size;

static int __init pcd_platform_driver_init(void)
{
	int ret;
	/* 1. alloc base device number  */
	ret = alloc_chrdev_region(&drvData._baseDevNum, 0, NUM_OF_DEV, "pcd_dev");
	if(ret < 0)
	{
		pr_err("alloc_chrdev_region error \n");
		return ret;
	}

	/* 2. alloc class */
	drvData._class = class_create(THIS_MODULE, "pcd_class");
	if(IS_ERR(drvData._class))
	{
		pr_err("class_create error \n");
		unregister_chrdev_region(drvData._baseDevNum, NUM_OF_DEV);
	}	
	
	/* 3. register platform driver */
	platform_driver_register(&pDrv);

	return 0;
}

static void __exit pcd_platform_driver_exit(void)
{
	pr_info("pcd_platform_driver_exit start \n");

	/* 1. unregister platform driver */
	platform_driver_unregister(&pDrv);
	pr_info("platform_driver_unregister Done \n");

	/* 2. destroy class */
	class_destroy(drvData._class);
	pr_info("class_destroy Done \n");

	/* 3. unregister device number */
	unregister_chrdev_region(drvData._baseDevNum, NUM_OF_DEV);
	pr_info("unregister_chrdev_region Done \n");

}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EW");
MODULE_DESCRIPTION("TEST");
