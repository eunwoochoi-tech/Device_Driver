#include "gpio_sysfs.h"

extern struct _SDriverData drvData;

int platDrv_probe(struct platform_device* pPlatDev)
{
	int i = 0;
	int ret = 0;
	const char* label;
	struct device* pDev = &pPlatDev->dev;
	struct device_node* child = NULL;
	struct device* pDevChild = NULL;
	struct _SDeviceData* pDevData;

	if(!pDev)
	{
		pr_err("platform device's device is null \n");
		return -EINVAL;
	}

	dev_info(pDev, "platform device %s is detected \n", pPlatDev->name);

	drvData._totalDevices = of_get_child_count(pDev->of_node);
	if(!drvData._totalDevices)
	{
		dev_err(pDev, "NO devices found \n");
		return -EINVAL;
	}

	dev_info(pDev, "Devices found : %d \n", drvData._totalDevices);
	drvData._ppDev = devm_kzalloc(pDev, sizeof(struct device*) * drvData._totalDevices, GFP_KERNEL);

	for_each_available_child_of_node(pDev->of_node, child)
	{
		pDevData = devm_kzalloc(pDev, sizeof(struct _SDeviceData), GFP_KERNEL);
		if(!pDevData)
		{
			dev_err(pDev, "devm_kzalloc error \n");
			ret = -ENOMEM;
			return ret;
		}

		ret = of_property_read_string(child, "label", &label);
		if(!ret)
		{
			dev_info(pDev, "label is detected : %s \n", label);
			strcpy(pDevData->_label, label);
		}
		else
		{
			dev_info(pDev, "label property isn't detected \n");
			dev_info(pDev, "Set default label property... \n");
			snprintf(pDevData->_label, sizeof(pDevData->_label), "unknowngpio-%d", i);
		}

		pDevData->_pGpioDesc = devm_fwnode_get_gpiod_from_child(pDev, "bone", &child->fwnode, GPIOD_ASIS, pDevData->_label);
		if(IS_ERR(pDevData->_pGpioDesc))
		{
			dev_err(pDev, "devm_fwnode_get_gpiod_from_child error \n");
			ret = PTR_ERR(pDevData->_pGpioDesc);
			return ret;
		}

		ret = gpiod_direction_output(pDevData->_pGpioDesc, LOW);
		if(ret)
		{
			dev_err(pDev, "gpiod_direction_output error \n");
			return ret;
		}

		drvData._ppDev[i] = device_create_with_groups(drvData._class, pDev, 0, pDevData, pAttrGroup, "gpio-%d", i);
		if(IS_ERR(drvData._ppDev[i]))
		{
			dev_err(pDev, "device_create_with_groups error \n");
			ret = PTR_ERR(pDevChild);
			return ret;
		}
	}

}

int platDrv_remove(struct platform_device* pPlatDev)
{
	int i = 0;

	for(i = 0; i < drvData._totalDevices; i++)
	{
		device_unregister(drvData._ppDev[i]);
	}
	return 0;
}

ssize_t direction_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf)
{
	int ret = 0;
	char* direction;
	
	// direction의 값을 반환 "out" 또는 "in"
	struct _SDeviceData* pDevData = pDev->driver_data;

	ret = gpiod_get_direction(pDevData->_pGpioDesc);
	
	if(ret < 0)
	{
		dev_err(pDev, "gpiod_get_direction error \n");
		return ret;
	}
	
	direction = (ret == 0) ? "out":"in";

	return sprintf(buf, "%s\n", direction);
}

ssize_t value_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf)
{
	struct _SDeviceData* pDevData = pDev->driver_data;
	int ret;

	ret = gpiod_get_value(pDevData->_pGpioDesc);

	return sprintf(buf, "%d\n", ret);
}

ssize_t label_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf)
{
	struct _SDeviceData* pDevData = pDev->driver_data;
	return sprintf(buf, "%s\n", pDevData->_label);
}

ssize_t direction_store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count)
{
	struct _SDeviceData* pDevData = pDev->driver_data;
	int ret;
	char* direction = NULL;

	if(sysfs_streq(buf, "out"))
	{
		ret = gpiod_direction_output(pDevData->_pGpioDesc, 0);
	}
	else if(sysfs_streq(buf, "in"))
	{
		ret = gpiod_direction_input(pDevData->_pGpioDesc);
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
	long value;
	int ret;

	ret = kstrtol(buf, 0, &value);
	if(ret)
	{
		dev_err(pDev, "kstrtol error \n");
		return ret;
	}

	gpiod_set_value(pDevData->_pGpioDesc, value);

	return count;
}
