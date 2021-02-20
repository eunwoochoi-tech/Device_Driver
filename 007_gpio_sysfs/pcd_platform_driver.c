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
	[0] = { .compatible = "org,bone-gpio-sysfs" },
};

struct platform_driver pDrv =
{
	.probe = pdrv_probe,
	.remove = pdrv_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "bone-gpio-sysfs",
		.of_match_table = ofDevId
	}
};

static DEVICE_ATTR_RW(direction);
static DEVICE_ATTR_RW(value);
static DEVICE_ATTR_RO(label);
static DEVICE_ATTR(max_size, S_IWUSR | S_IRUGO, max_size_show, max_size_store);

static struct attribute* pAttrs[] = {
	&dev_attr_direction.attr,
	&dev_attr_value.attr,
	&dev_attr_label.attr,
	NULL
};

static struct attribute_group attrGroupp = {
	.attrs = pAttrs
};

const struct attribute_group* pAttrGroup[] = {
	&attrGroupp,
	NULL
};

SDriverData drvData;
SDeviceData* pDevData;

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
	/*
	struct _SDeviceData* pDevData = (struct _SDeviceData*)pDev->parent->driver_data;

	if(!pDevData)
	{
		dev_err(pDev, "Error occured while getting device data... \n");
		return -EINVAL;
	}
	
	return sprintf(buf, "%d\n", pDevData->_size);
	*/

	return 0;
}

ssize_t direction_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf)
{
	struct _SDeviceData* pDevData = pDev->driver_data;
	int direction;
	char* arrDirection;

	direction = gpiod_get_direction(pDevData->_pGpioDesc);
	if(direction < 0)
	{
		dev_err(pDev, "gpiod_get_direction erorr \n");
		return direction;
	}

	// if dir == 0, then show "out, if dir == 1, then show "in" */

	arrDirection = (direction == 0) ? "out" : "in";

	return sprintf(buf, "%s\n", arrDirection);
}

ssize_t value_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf)
{
	int value;
	struct _SDeviceData* pDevData = pDev->driver_data;

	value = gpiod_get_value(pDevData->_pGpioDesc);

	return sprintf(buf, "%d\n", value);
}

ssize_t label_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf)
{
	struct _SDeviceData* pDevData = pDev->driver_data;

	return sprintf(buf, "%s\n", pDevData->_label);
}

ssize_t max_size_store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count)
{
	/*
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
	*/
	return 0;
	// return count;
}

ssize_t direction_store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count)
{
	struct _SDeviceData* pDevData = pDev->driver_data;
	int ret;

	if(sysfs_streq(buf, "in"))
	{
		ret = gpiod_direction_input(pDevData->_pGpioDesc);
	}
	else if(sysfs_streq(buf, "out"))
	{
		ret = gpiod_direction_output(pDevData->_pGpioDesc, 0);
	}
	else
	{
		ret = -EINVAL;
	}

	return ret ? ret : count;
}

ssize_t value_store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count)
{
	struct _SDeviceData* pDevData = pDev->driver_data;
	int ret;
	long value;

	ret = kstrtol(buf, 0, &value);
	if(ret)
	{
		dev_err(pDev, "kstrtol error \n");
		return ret;
	}

	gpiod_set_value(pDevData->_pGpioDesc, value);

	return count;
}

int get_data_from_dt(struct device* pDev)
{
	/*
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

	*/
	return 0;
}

int pdrv_probe(struct platform_device* pPlatDev)
{
	int i = 0;
	int ret;
	const char* name;
	struct device* pDev = &pPlatDev->dev;
	struct _SDeviceData* pDevData = NULL;
	struct device_node* pDevNode = pPlatDev->dev.of_node;
	struct device_node* child = NULL;
	
	drvData._totalDevices = of_get_child_count(pDevNode);
	dev_info(pDev, "Total Devices : %d \n", drvData._totalDevices);

	if(!drvData._totalDevices)
	{
		dev_err(pDev, "No Devices found \n");
		return -EINVAL;
	}

	drvData._ppDev = devm_kzalloc(pDev, sizeof(struct device*) * drvData._totalDevices, GFP_KERNEL);
	if(IS_ERR(drvData._ppDev))
	{
		dev_err(pDev, "devm_kzallic error \n");
		ret = PTR_ERR(*drvData._ppDev);
		return ret;
	}

	for_each_available_child_of_node(pDevNode, child)
	{
		pDevData = devm_kzalloc(pDev, sizeof(struct _SDeviceData), GFP_KERNEL);
		if(!pDevData)
		{
			dev_err(pDev, "devm_kzalloc failed \n");
			return -ENOMEM;
		}

		ret = of_property_read_string(child, "label", &name);
		if(ret)
		{
			dev_warn(pDev, "Missing label info \n");
			snprintf(pDevData->_label, sizeof(pDevData->_label), "unkngpio%d", i);
		}
		else
		{
			strcpy(pDevData->_label, name);
			dev_info(pDev, "GPIO _label is %s\n", pDevData->_label);
		}

		dev_info(pDev, "devm_fwnode_get_gpiod_from_child start");
		pDevData->_pGpioDesc = devm_fwnode_get_gpiod_from_child(pDev, "bone", &child->fwnode, GPIOD_ASIS, pDevData->_label);
		dev_info(pDev, "devm_fwnode_get_gpiod_from_child end");

		if(IS_ERR(pDevData->_pGpioDesc))
		{
			dev_err(pDev, "devm_fwnode_get_gpiod_from_child error \n");
			ret = PTR_ERR(pDevData->_pGpioDesc);
			if(ret == -ENOENT)
			{
				dev_err(pDev, "NO GPIO has been assigned to the requested function or idx \n");
			}
			return ret;
		}

		dev_info(pDev, "gpiod_direction_output start \n");
		/* set the gpio direction to output */
		ret = gpiod_direction_output(pDevData->_pGpioDesc, 0); 
		if(ret)
		{
			dev_err(pDev, "gpiod_direction_output error \n");
			return ret;
		}
		dev_info(pDev, "gpiod_direction_output end \n");

		drvData._ppDev[i] = device_create_with_groups(drvData._pClass, pDev, 0, pDevData, pAttrGroup, pDevData->_label);
		
		if(IS_ERR(drvData._ppDev[i]))
		{
			dev_err(pDev, "device_create_with_groups error \n");
			ret = PTR_ERR(drvData._ppDev[i]);
			return ret;
		}

		i++;
	}


	return 0;
}

int pdrv_remove(struct platform_device* pPlatDev)
{
	int i;

	for(i = 0; i < drvData._totalDevices; i++)
	{
		device_unregister(drvData._ppDev[i]);
	}

	return 0;
}

// extern struct SDeviceData* pDevData;
// extern struct SDriverData drvData;
