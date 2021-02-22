#include "gpio_sysfs.h"

int platDrv_probe(struct platform_device* pPlatDev)
{
	int i;
	int ret;
	const char* label;
	struct device* pDev = &pPlatDev->dev;
	struct device_node* child = NULL;
	struct _SDeviceData* pDevData;

	if(!pDev)
	{
		pr_err("platform device's device is null \n");
		return -EINVAL;
	}

	dev_info(pDev, "platform device %s is detected \n", pPlatDev->name);

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

		devm_fwnode_get_gpiod_from_child(pDev, "bone", GPIO_ASIS);
	}

}

int platDrv_remove(struct platform_device* pPlatDev)
{
	return 0;
}
