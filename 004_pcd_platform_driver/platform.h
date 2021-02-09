#define RDWR	0x11
#define RDONLY	0x01
#define WRONLY	0x10

#define DEV_NUM	3

struct pcd_dev_platform_data
{
	int size;
	int perm;
	const char* serial_number;
};
