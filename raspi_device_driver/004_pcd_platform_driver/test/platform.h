#include <linux/cdev.h>

#define RDONLY	0x01
#define WRONLY	0x10
#define RDWR	0x11

struct pcd_platform_data
{
	int size;
	int perm;
	const char* serial_number;
};


struct pcd_device_private_data
{
	struct pcd_platform_data pcd_pdata;
	dev_t dev_num;
	struct cdev pcd_cdev;
	char* buffer;
};


struct pcd_driver_private_data
{
	int total_devices;
	dev_t base_dev_num;
	struct class* pcd_class;
	struct device* pcd_device;
};
