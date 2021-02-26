#include "lcd_sysfs.h"

DEVICE_ATTR(

extern struct _SLcdDrvData lcdDrvData;
struct _SLcdDevData* pLcdDevData;

int platDrvProbe(struct platform_device* pPlatDev)
{
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
	
		pChildDev = device_create_with_groups(&lcdDrvData._class, parent, 0, NULL, attribute_group, fmt, ...);
		if(!pChildDev)
		{
			dev_err(pDev, "device_create_with_groups error \n");
			return PTR_ERR(pChildDev);
		}
	}
	
	dev_info(pDev, "platform Driver Probe end \n");
	return 0;
}

int platDrvRemove(struct platform_device* pPlatDev)
{

}
