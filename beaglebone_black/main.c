#include "lcd_sysfs.h"

struct of_device_id of_match_compatible = { .compatible = "lcd_16x2_pins" };

struct platform_driver platDrv = {
	.probe = platDrvProbe,
	.remove = platDrvRemove,
	.driver = {
		.name = "lcd_16x2_pins",
		.of_match_table = &of_match_compatible
	}
};

struct _SLcdDrvData lcdDrvData;

static int __init lcd_sysfs_init(void)
{
	pr_info("lcd_sysfs_init start \n");

	// create class under /sys/class
	pr_info("class_create func \n");
	lcdDrvData._class = class_create(THIS_MODULE, "lcd_16x2");
	if(IS_ERR(lcdDrvData._class))
	{
		pr_err("class_create error \n");
		return PTR_ERR(lcdDrvData._class);
	}

	pr_info("platform_driver_register func \n");
	platform_driver_register(&platDrv);

	pr_info("lcd_sysfs_init end \n");

	return 0;
}

static void __exit lcd_sysfs_exit(void)
{
	pr_info("lcd_sysfs_exit start \n");

	pr_info("driver_unregister func \n");
	platform_driver_unregister(&platDrv);

	pr_info("class_destroy func \n");
	class_destroy(lcdDrvData._class);

	pr_info("lcd_sysfs_exit end \n");
}

module_init(lcd_sysfs_init);
module_exit(lcd_sysfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("this is test device driver for lcd 16x2"); 
