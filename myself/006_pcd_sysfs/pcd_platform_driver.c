#include "pcd_platform_driver.h"

struct file_operations fOps = {
	.open = open,
	.read = read,
	.write = write,
	.llseek = llseek,
	.release = release
};

struct of_device_id ofDevId[NUM_OF_DEV] =
{
	[0] = { .compatible = "pcd1" },
	[1] = { .compatible = "pcd2" },
	[2] = { .compatible = "pcd3" },
	[3] = { .compatible = "pcd4" }
};

struct platform_driver pDrv =
{
	.probe = pdrv_probe,
	.remove = pdrv_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "pcd_platform_driver",
		.of_match_table = ofDevId
	}
};

SDriverData drvData;
SDeviceData* pDevData;

DEVICE_ATTR(max_size, S_IWUSR | S_IRUGO, max_size_show, max_size_store);
struct attribute* pArrAttr[] =
{
	&dev_attr_max_size.attr
};
struct attribute_group attrGroup =
{
	.attrs = pArrAttr
};

loff_t llseek(struct file* filp, loff_t offest, int count)
{

	return 0;
}

ssize_t read(struct file* filp, char __user* buf, size_t count, loff_t* offset)
{

	return 0;
}

ssize_t write(struct file* filp, const char __user* buf, size_t count, loff_t* offset)
{

	return -EINVAL;
}

int open(struct inode* inode, struct file* filp)
{

	return 0;
}

int release(struct inode* inode, struct file* filp)
{
	return 0;
}

ssize_t max_size_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf)
{
	struct _SDeviceData* pDevData = (struct _SDeviceData*)pDev->parent->driver_data;

	if(!pDevData)
	{
		dev_err(pDev, "Error occured while getting device data... \n");
		return -EINVAL;
	}
	
	return sprintf(buf, "%d\n", pDevData->_size);
}

ssize_t max_size_store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count)
{
	long result;
	int ret;
	struct _SDeviceData* pDevData = (struct _SDeviceData*)pDev->parent->driver_data;
	
	ret = kstrtol(buf, 10, &result);
	if(ret < 0)
	{
		return ret;
	}
	pDevData->_size = result;	

	pDevData->_buf = krealloc(pDevData->_buf, result, GFP_KERNEL);

	return count;
}

int get_data_from_dt(struct device* pDev)
{
	int ret;
	struct device_node* pDevNode = pDev->of_node;


	pDevData->_devNum = drvData._baseDevNum + drvData._totalDevices;
	ret = of_property_read_u32(pDevNode, "org,size", &pDevData->_size);
	if(ret < 0)
	{
		dev_err(pDev, "of_property_read_u32 size error \n");
		return ret;
	}
	ret = of_property_read_u32(pDevNode, "org,perm", &pDevData->_perm);
	if(ret < 0)
	{
		dev_err(pDev, "of_property_read_u32 perm error \n");
		return ret;
	}
	ret = of_property_read_string(pDevNode, "org,serial-number", &pDevData->_serialNumber);
	if(ret < 0)
	{
		dev_err(pDev, "of_property_read_string serial-number error \n");
		return ret;
	}
	pDevData->_buf = devm_kzalloc(pDev, pDevData->_size, GFP_KERNEL);
	if(!pDevData->_buf)
	{
		dev_err(pDev, "devm_kzalloc buffer error \n");
		return -ENOMEM;
	}

	pr_info("Device Number : %d \n", pDevData->_devNum);
	pr_info("Device Size : %d \n", pDevData->_size);
	pr_info("Device Perm : %d \n", pDevData->_perm);
	pr_info("Device Serial number : %s \n\n", pDevData->_serialNumber);

	return 0;
}

int pdrv_probe(struct platform_device* pPlatDev)
{
	int ret;
	struct device* pDev = &pPlatDev->dev;
	struct device_node* pDevNode = pDev->of_node;
	
	dev_info(pDev, "platform drvier probe start \n");
	// if device is detected because of not DT
	if(!pDevNode)
	{
		dev_err(pDev, "DT is not present \n");
		return -EINVAL;
	}

	dev_info(pDev, "DT is detected \n");

	pDevData = devm_kzalloc(pDev, sizeof(SDeviceData), GFP_KERNEL);
	if(!pDevData)
	{
		dev_err(pDev, "devm_kzalloc error \n");
		return -ENOMEM;
	}
	pDev->driver_data = pDevData;

	ret = get_data_from_dt(pDev);	
	if(ret)
	{
		dev_err(pDev, "get_data_from_dt error \n");
		return ret;
	}

	cdev_init(&pDevData->_cdev, &fOps);
	pDevData->_cdev.owner = THIS_MODULE;

	cdev_add(&pDevData->_cdev, pDevData->_devNum, 1);

	drvData._dev = device_create(drvData._class, pDev, pDevData->_devNum, NULL, "pcd-%d", drvData._totalDevices);
	if(IS_ERR(drvData._dev))
	{
		dev_err(pDev, "device_create error \n");
		cdev_del(&pDevData->_cdev);
		ret = PTR_ERR(drvData._dev);
		return ret;
	}

	ret = sysfs_create_file(&drvData._dev->kobj, &dev_attr_max_size.attr);
	if(ret < 0)
	{
		dev_err(pDev, "sysfs_create_file error \n");
		device_destroy(drvData._class, pDevData->_devNum);
		cdev_del(&pDevData->_cdev);
	}

	drvData._totalDevices++;

	dev_info(pDev, "platform drvier probe end \n");
	return 0;
}

int pdrv_remove(struct platform_device* pPlatDev)
{
	struct device* pDev = &pPlatDev->dev;
	struct _SDeviceData* pDevData = (struct _SDeviceData*)pDev->driver_data;

	dev_info(pDev, "platform driver remove start \n");

	device_destroy(drvData._class, pDevData->_devNum);

	cdev_del(&pDevData->_cdev);
	
	drvData._totalDevices--;

	dev_info(pDev, "platform driver remove end \n");
	return 0;
}

// extern struct SDeviceData* pDevData;
// extern struct SDriverData drvData;
