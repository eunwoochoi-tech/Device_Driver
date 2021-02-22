#include "gpio_sysfs.h"

struct of_device_id ofDevId[] = {
	{ .compatible = "org,bone-gpio-sysfs" }
};

struct _SDriverData drvData;

struct platform_driver platDrv = {
	.probe = platDrv_probe,
	.remove = platDrv_remove,
	.driver = {
		.name = "gpio_sysfs_pDrv",
		.of_match_table = ofDevId
	}
};

static int __init gpio_sysfs_init(void)
{
	// create class under /sys/class
	drvData._class = class_create(THIS_MODULE, "sysfs_gpios");
	if(IS_ERR(drvData._class))
	{
		return PTR_ERR(drvData._class);
	}

	platform_driver_register(&platDrv);

}

static void __exit gpio_sysfs_exit(void)
{

}

module_init(gpio_sysfs_init);
module_exit(gpio_sysfs_exit);
