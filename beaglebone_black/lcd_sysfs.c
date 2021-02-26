#include "lcd_sysfs.h"

static DEVICE_ATTR_RW(direction);
static DEVICE_ATTR_RW(value);
static DEVICE_ATTR_RO(label);

static struct attribute* attrs[] = {
	&dev_attr_direction.attr,
	&dev_attr_value.attr,
	&dev_attr_label.attr
};

static struct attribute_group attrGroup = {
	.attrs = attrs
};

static const struct attribute_group* pAttrGroups[] = {
	&attrGroup
};

extern struct _SLcdDrvData lcdDrvData;
struct _SLcdDevData* pLcdDevData;

int platDrvProbe(struct platform_device* pPlatDev)
{
	int i = 0;
	int ret;
	struct device* pDev = &pPlatDev->dev;
	struct device* pChildDev = NULL;
	struct device_node* parent = NULL;
	struct device_node* child = NULL;

	if(!pDev)
	{
		pr_err("platform device's device is null \n");
		return -EINVAL;
	}

	parent = pDev->of_node;
	if(!parent)
	{
		dev_err(pDev, "device tree is not detected \n");
		return -EINVAL;
	}

	dev_info(pDev, "platform Driver Probe start \n");

	lcdDrvData._totalDevices = of_get_child_count(parent);
	if(!lcdDrvData._totalDevices)
	{
		dev_err(pDev, "There is no child device node \n");
		return -EINVAL;
	}

	lcdDrvData._ppDev = devm_kzalloc(pDev, sizeof(struct device*) * lcdDrvData._totalDevices, GFP_KERNEL);
	if(!lcdDrvData._ppDev)
	{
		dev_err(pDev, "devm_kzalloc error \n");
		return -ENOMEM;
	}
	dev_info(pDev, "%d devices detected \n", lcdDrvData._totalDevices);

	for_each_available_child_of_node(parent, child)
	{
		pLcdDevData = devm_kzalloc(pDev, sizeof(struct _SLcdDevData), GFP_KERNEL);
		if(!pLcdDevData)
		{
			dev_err(pDev, "devm_kzalloc error \n");
			return -ENOMEM;
		}

		ret = of_property_read_string(child, "label", &pLcdDevData->_label);
		if(ret)
		{
			dev_err(pDev, "of_property_read_string error \n");
			return ret;
		}
		dev_info(pDev, "read label : %s \n", pLcdDevData->_label);
		pLcdDevData->_pGpioDesc = devm_fwnode_get_gpiod_from_child(pDev, "lcd", &child->fwnode, GPIOD_ASIS, pLcdDevData->_label);
		if(!pLcdDevData->_pGpioDesc)
		{
			dev_err(pDev, "devm_fwnode_get_gpiod_from_child error \n");
			return -ENOMEM;
		}

		ret = gpiod_direction_output(pLcdDevData->_pGpioDesc, GPIOD_OUT_HIGH);
		if(ret)
		{
			dev_err(pDev, "gpiod_direction_output error \n");
			return ret;
		}
	
		lcdDrvData._ppDev[i] = device_create_with_groups(lcdDrvData._class, pDev, 0, pLcdDevData, pAttrGroups, "attr-%d", i);
		if(IS_ERR(pChildDev))
		{
			dev_err(pDev, "device_create_with_groups error \n");
			return PTR_ERR(pChildDev);
		}

		i++;
	}
	
	dev_info(pDev, "platform Driver Probe end \n");
	return 0;
}

int platDrvRemove(struct platform_device* pPlatDev)
{
	int i;

	for(i = 0; i < lcdDrvData._totalDevices; i++)
	{
		device_unregister(lcdDrvData._ppDev[i]);
	}
	class_destroy(lcdDrvData._class);
	
}

ssize_t direction_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	dev_info(dev, "direction_show");
	return 0;
}

ssize_t direction_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	dev_info(dev, "direction_store");
	return 0;

}

ssize_t value_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	dev_info(dev, "value_show");
	return 0;

}

ssize_t value_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	dev_info(dev, "value_store");
	return 0;
}

ssize_t label_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	dev_info(dev, "label_show \n");
	return 0;
}

