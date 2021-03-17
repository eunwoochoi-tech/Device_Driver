#include "7segment.h"

DEVICE_ATTR_RW(number);

struct of_device_id compatible_id[] = {
	{ .compatible = "bone,7segment" }
};

struct _SDeviceDriver driver;
struct platform_driver platDriver = {
	.probe = platProbe,
	.remove = platRemove,
	.driver = {
		.name = "7segment_driver",
		.of_match_table = compatible_id
	}
};

struct attribute* ppAttrs[] = {
	&dev_attr_number.attr,
	NULL
};

struct attribute_group attrGroup = {
	.attrs = ppAttrs
};

const struct attribute_group* pAttrGroup[] = {
	&attrGroup, 
	NULL
};

static int __init segment_init(void)
{
	pr_info("segment init start \n");

	driver._pClass = class_create(THIS_MODULE, "7segment");
	if(IS_ERR(driver._pClass))
	{
		pr_info("class_create error \n");
		return PTR_ERR(driver._pClass);
	}

	platform_driver_register(&platDriver);

	pr_info("segment init exit \n");
	return 0;
}

static void __exit segment_exit(void)
{
	pr_info("segment exit start \n");

	platform_driver_unregister(&platDriver);
	class_destroy(driver._pClass);

	pr_info("segment exit exit \n");

	return 0;
}

int platProbe(struct platform_device* pPlatDev)
{
	int i = 0;
	int ret = 0;
	struct device* pDev = &pPlatDev->dev;

	dev_info(pDev, "platform prove start \n");

	// pass address of driver
	dev_set_drvdata(pDev, &driver);
	
	// get gpio_desc
	dev_info(pDev, "get gpio_desc \n");
	driver._pGpioDesc[A] = gpiod_get(pDev, "a", GPIOD_ASIS);
	driver._pGpioDesc[B] = gpiod_get(pDev, "b", GPIOD_ASIS);
	driver._pGpioDesc[C] = gpiod_get(pDev, "c", GPIOD_ASIS);
	driver._pGpioDesc[D] = gpiod_get(pDev, "d", GPIOD_ASIS);
	driver._pGpioDesc[E] = gpiod_get(pDev, "e", GPIOD_ASIS);
	driver._pGpioDesc[F] = gpiod_get(pDev, "f", GPIOD_ASIS);
	driver._pGpioDesc[G] = gpiod_get(pDev, "g", GPIOD_ASIS);

	if( IS_ERR(driver._pGpioDesc[A]) ||
		IS_ERR(driver._pGpioDesc[B]) || 
		IS_ERR(driver._pGpioDesc[C]) || 
		IS_ERR(driver._pGpioDesc[D]) || 
		IS_ERR(driver._pGpioDesc[E]) || 
		IS_ERR(driver._pGpioDesc[F]) || 
		IS_ERR(driver._pGpioDesc[G]) )
	{
		dev_err(pDev, "gpiod_get error \n");
		return -EINVAL;
	}

	// gpio direction setting
	dev_info(pDev, "gpio direction setting \n");
	for(i = 0; i < NUM_OF_PINS; i++)
	{
		ret = gpio_configure_dir(driver._pGpioDesc[i], OUTPUT, ACTIVE_HIGH);
		if(ret)
		{
			dev_info(pDev, "gpio_configure_dir error \n");
			return -EINVAL;
		}
	}
	// create device attribute 
	ret = create_device_attrs(pDev, pAttrGroup);
	if(ret)
	{
		dev_err(pDev, "create device attrs error \n");
		return -1;
	}

	dev_info(pDev, "platform prove end \n");

	return 0;
}

int platRemove(struct platform_device* pPlatDev)
{
	int ret = 0;
	int i = 0;
	struct device* pDev = &pPlatDev->dev;
	struct _SDeviceDriver* _pDriver = dev_get_drvdata(pDev);

	dev_info(pDev, "platform remove start \n");

	for(i = 0; i < NUM_OF_PINS; i++)
	{
		if(_pDriver->_pGpioDesc[i])
		{
			gpiod_put(_pDriver->_pGpioDesc[i]);
		}
	}

	dev_info(pDev, "platform remove end \n");
	
	return 0;
}

int create_device_attrs(struct device* pDev, const struct attribute_group** ppAttrGroups)
{
	struct _SDeviceDriver* pDriver = dev_get_drvdata(pDev);

	pDriver->_pDevice = device_create_with_groups(pDriver->_pClass, pDev, 0, pDriver, ppAttrGroups, "7segment_attrs");
	if(IS_ERR(pDriver->_pDevice))
	{
		return PTR_ERR(pDriver->_pDevice);
	}

	return 0;
}

ssize_t number_show(struct device* pDev, struct device_attribute* pAttr, char *buf)
{
	return 0;
}

ssize_t number_store(struct device* pDev, struct device_attribute* pAttr, const char *buf, size_t count)
{
	return 0;
}

module_init(segment_init);
module_exit(segment_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("7segment device driver");
