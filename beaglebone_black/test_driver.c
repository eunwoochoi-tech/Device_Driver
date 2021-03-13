#include "test_driver.h"

DEVICE_ATTR_RW(direction);
DEVICE_ATTR_RW(value);
DEVICE_ATTR_RO(label);

struct attribute* attrs[] = { &dev_attr_direction.attr, &dev_attr_value.attr, &dev_attr_label.attr, NULL };
struct attribute_group attrGroup = { .attrs = attrs };
const struct attribute_group* attrGroups[] = { &attrGroup, NULL };

struct of_device_id of_test_table[] = {
	{ .compatible = "custom_lcd" }
};

struct platform_driver platDrv = {
	.probe = platDrv_probe,
	.remove = platDrv_remove,
	.driver = {
		.name = "test_driver",
		.of_match_table = of_test_table
	}
};

struct _SDeviceDriver devDriver = {
	._totalDevices = 0,
	._pClass = NULL,
	._ppDevs = NULL
};

static int __init test_init(void)
{
	pr_info("test_init start \n");

	devDriver._pClass = class_create(THIS_MODULE, "lcd_class");
	if(IS_ERR(devDriver._pClass))
	{
		pr_err("class_create error \n");
		return -EINVAL;
	}

	platform_driver_register(&platDrv);

	pr_info("test_init end \n");
	return 0;
}

static void __exit test_exit(void)
{
	pr_info("test_exit start \n");

	platform_driver_unregister(&platDrv);
	class_destroy(devDriver._pClass);

	pr_info("test_exit end \n");
	return 0;

}

int platDrv_probe(struct platform_device* pPlatDev)
{
	int i = 0;
	int ret = 0;
	const char* label = NULL;
	struct device* pDev = &pPlatDev->dev;
	struct device_node* parent = pDev->of_node;
	struct device_node* child = NULL;
	struct gpio_desc* pGpioDesc = NULL;

	dev_info(pDev, "platDrv_probe start \n");

	dev_info(pDev, "platform_device's name is %s \n", pPlatDev->name);
	devDriver._totalDevices = of_get_child_count(pDev->of_node);
	devDriver._ppDevs = devm_kzalloc(pDev, sizeof(struct device*) * devDriver._totalDevices, GFP_KERNEL);
	if(IS_ERR(devDriver._ppDevs))
	{
		dev_err(pDev, "devm_kzalloc error \n");
		ret = -ENOMEM;
		return ret;
	}

	if(devDriver._totalDevices <= 0)
	{
		dev_info(pDev, "It has no child node \n");
	}
	else
	{
		dev_info(pDev, "%d child nodes detected \n", devDriver._totalDevices);
		for_each_available_child_of_node(parent, child)
		{
			of_property_read_string(child, "label", &label);
			dev_info(pDev, "%d's info \n", i);
			dev_info(pDev, "name : %s \n", child->name);
			dev_info(pDev, "label: %s \n", label);
			
			pGpioDesc = devm_fwnode_get_gpiod_from_child(pDev, "lcd", &child->fwnode, GPIOD_ASIS, label);
			dev_info(pDev, "devm_fwnode_get_gpiod_from_child done \n");
			if(IS_ERR(pGpioDesc))
			{
				dev_err(pDev, "devm_fwnode_get_gpiod_from_child error \n");
			}
			else
			{
				gpiod_direction_output(pGpioDesc, LOW);
				dev_info(pDev,"gpiod_direction_output done \n");
			}

			dev_info(pDev, "i = %d \n", i);
			devDriver._ppDevs[i] = device_create_with_groups(devDriver._pClass, pDev, 0, pGpioDesc, attrGroups, label);
			if(IS_ERR(devDriver._ppDevs[i]))
			{
				dev_err(pDev, "device_create_with_group error \n");
				ret = PTR_ERR(devDriver._ppDevs[i]);
				return ret;
			}

			i++;
		}
	}

	dev_info(pDev, "platDrv_probe end \n");
	return 0;
}

int platDrv_remove(struct platform_device* pPlatDev)
{
	int i;
	int ret;
	struct device* pDev = &pPlatDev->dev;
	
	dev_info(pDev, "platDrv_remove start \n");

	dev_info(pDev, "total devices : %d \n", devDriver._totalDevices);
	for(i = 0; i < devDriver._totalDevices; i++)
	{
		if(devDriver._ppDevs[i] == NULL)
		{
			dev_info(pDev, "%d's device is NULL \n", i);
			continue;
		}
		device_unregister(devDriver._ppDevs[i]);
	}

	dev_info(pDev, "platDrv_remove end \n");
}
ssize_t direction_show(struct device* pDev, struct device_attribute* attr, char* buf)
{
	int ret;
	struct gpio_desc* pGpioDesc = (struct gpio_desc*)pDev->driver_data;
	if(!pGpioDesc)
	{
		dev_err(pDev, "err, driver_data is NULL \n");
		return -EINVAL;
	}

	ret = gpiod_get_direction(pGpioDesc);
	return scnprintf(buf, 1024, "%s\n", ret ? "input" : "output");
} 

ssize_t value_show(struct device* pDev, struct device_attribute* attr, char* buf)
{
	int ret;
	struct gpio_desc* pGpioDesc = (struct gpio_desc*)pDev->driver_data;
	if(!pGpioDesc)
	{
		dev_err(pDev, "err, driver_data is NULL \n");
		return -EINVAL;
	}
	
	ret = gpiod_get_value(pGpioDesc);
	return scnprintf(buf, 1024, "%s\n", ret ? "HIGH" : "LOW");
}

ssize_t label_show(struct device* pDev, struct device_attribute* attr, char* buf)
{
	struct gpio_desc* pGpioDesc = (struct gpio_desc*)pDev->driver_data;
	if(!pGpioDesc)
	{
		dev_err(pDev, "err, driver_data is NULL \n");
		return -EINVAL;
	}
	return scnprintf(buf, 1024, "%s\n", pGpioDesc->label);
}

ssize_t direction_store(struct device* pDev, struct device_attribute* attr, const char* buf, size_t count)
{
	int ret;

	struct gpio_desc* pGpioDesc = (struct gpio_desc*)pDev->driver_data;
	if(sysfs_streq(buf, "OUT"))
	{
		ret = gpiod_direction_output(pGpioDesc, 0);
	}
	else if(sysfs_streq(buf, "IN"))
	{
		ret = gpiod_direction_input(pGpioDesc);
	}
	else
	{
		ret = -EINVAL;
	}

	return ret ? ret : count;
}

ssize_t value_store(struct device* pDev, struct device_attribute* attr, const char* buf, size_t count)
{
	int ret;
	long value;
	struct gpio_desc* pGpioDesc = (struct gpio_desc*)pDev->driver_data;

	ret = kstrtol(buf, 0, &value);
	if(ret)
	{
		return ret;
	}

	gpiod_set_value(pGpioDesc, value);

	return count;
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("test driver");
