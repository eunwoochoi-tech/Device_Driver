#include "pcd_platform_driver.h"

extern struct of_device_id ofDevId[NUM_OF_DEV];
extern struct platform_driver pDrv;
extern struct _SDriverData drvData;
extern struct device_attribute device_attr_max_size;

static int __init pcd_platform_driver_init(void)
{
	int ret;
	pr_info("init start \n");
	/* 1. Create structure class */
	drvData._pClass = class_create(THIS_MODULE, "bone_gpios");
	if(IS_ERR(drvData._pClass))
	{
		pr_err("class_create error \n");
		return PTR_ERR(drvData._pClass);
	}

	platform_driver_register(&pDrv);
	
	pr_info("init end \n");

	return 0;
}

static void __exit pcd_platform_driver_exit(void)
{
	pr_info("pcd_platform_driver_exit start \n");

	/* 1. unregister platform driver */
	platform_driver_unregister(&pDrv);
	pr_info("platform_driver_unregister Done \n");

	/* 2. destroy class */
	class_destroy(drvData._pClass);
	pr_info("class_destroy Done \n");

	pr_info("pcd_platform_driver_exit \n");
}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EW");
MODULE_DESCRIPTION("TEST");
