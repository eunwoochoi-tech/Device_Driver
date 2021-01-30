#include <linux/module.h>
#include <linux/platform_device.h>
#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

void pcd_dev_release(struct device* dev);

// create 2 platform data
struct pcd_dev_platform_data pcd_dev_pdata[2] = {
	[0] = { .size = 512, .permission = RDWR, .serial_number = "PCD_DEV_ABC_111"},
	[1] = { .size = 1024, .permission = RDWR, .serial_number = "PCD_DEV_ABC_222"}
};

// 1. create 2 platform devices
// /sys/devices/platform 하위에 "name.id" 형식으로 플랫폼 디바이스 디렉터리가 생성된다

struct platform_device platform_pcd_dev_1 = {
	.name = "pseudo_char_device",
	.id = 0,
	.dev = {
			.platform_data = &pcd_dev_pdata[0],
			.release = pcd_dev_release
	}
};

struct platform_device platform_pcd_dev_2 = {
	.name = "pesudo_char_device",
	.id = 1,
	.dev = {
			.platform_data = &pcd_dev_pdata[1],
			.release = pcd_dev_release
	}
};

static int __init pcd_dev_platform_init(void)
{
	// register platform device
	platform_device_register(&platform_pcd_dev_1);
	platform_device_register(&platform_pcd_dev_2);

	pr_info("Device setup module inserted \n");

	return 0;
}

static void __exit pcd_dev_platform_exit(void)
{
	platform_device_unregister(&platform_pcd_dev_1);
	platform_device_unregister(&platform_pcd_dev_2);

	pr_info("Device setup module removed \n");
}

module_init(pcd_dev_platform_init);
module_exit(pcd_dev_platform_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module platform device");

void pcd_dev_release(struct device* dev)
{
	pr_info("release \n");
}
