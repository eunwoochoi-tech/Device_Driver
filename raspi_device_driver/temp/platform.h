#define RDWR	0x11
#define RDONLY	0x01
#define WRONLY	0x10

struct pcd_dev_platform_data
{
	int size;
	int permission;
	const char* serial_number;
};
