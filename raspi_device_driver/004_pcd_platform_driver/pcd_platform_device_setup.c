#include <linux/module.h>
#include <linux/platform_device.h>
#include "platform.h"

void pcd_device_release(struct device* dev);

struct pcd_dev_platform_data pcd_private_data[DEV_NUM] = {
	[0] = {
		.size = 512,
		.perm = RDWR,
		.serial_number = "PCDDEV1"
	},

	[1] = {
		.size = 1024,
		.perm = RDONLY,
		.serial_number = "PCDDEV2"
	},

	[2] = {
		.size = 1024,
		.perm = WRONLY,
		.serial_number = "PCDDEV3"
	}
};

struct platform_device pcd_platform_device[DEV_NUM] = {
	[0] = {
		.name = "pcd-device",
		.id = 0,
		.dev = {
			.platform_data = &pcd_private_data[0],
			.release = pcd_device_release
		}
	},

	[1] = {
		.name = "pcd-device",
		.id = 1,
		.dev = {
			.platform_data = &pcd_private_data[1],
			.release = pcd_device_release
		}
	},

	[2] = {
		.name = "pcd-device",
		.id = 2,
		.dev = {
			.platform_data = &pcd_private_data[0],
			.release = pcd_device_release
		}

	}
};

static int __init pcd_platform_init(void)
{
	platform_device_register(&pcd_platform_device[0]);	
	platform_device_register(&pcd_platform_device[1]);	
	platform_device_register(&pcd_platform_device[2]);	

	pr_info("Device setup module inserted \n");
	return 0;
}

static void __exit pcd_platform_exit(void)
{
	platform_device_unregister(&pcd_platform_device[0]);
	platform_device_unregister(&pcd_platform_device[1]);
	platform_device_unregister(&pcd_platform_device[2]);
	
	pr_info("Device setup module exitd \n");
}

void pcd_device_release(struct device* dev)
{
	pr_info("pcd_device_release is called with %d device \n", dev->id);
}

module_init(pcd_platform_init);
module_exit(pcd_platform_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module which registers platform devices");
