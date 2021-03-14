#include "lcd_driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("LCD16x2 Driver");

DEVICE_ATTR_WO(lcdcmd);
DEVICE_ATTR_WO(lcdtext);
DEVICE_ATTR_RW(lcdxy);

struct attribute* ppAttr[] = {
	&dev_attr_lcdcmd.attr,
	&dev_attr_lcdtext.attr,
	&dev_attr_lcdxy.attr,
	NULL
};

struct attribute_group attrGroup = {
	.attrs = ppAttr
};

const struct attribute_group* ppAttrGroup[] = {
	&attrGroup, 
	NULL
};

struct of_device_id devNode[] = {
	{ .compatible = "lcd16x2" }
};

struct _SLcdPrivateData lcdPriData = {
	._pDev= NULL, 
	._ppGpioDesc = NULL,
	.xy = 0
};
struct _SLcdDriver lcdDrv= { ._pClass = NULL };

struct platform_driver platDrv = {
	
	.probe = lcd_driver_probe,
	.remove = lcd_driver_remove,
	.driver = {
		.name = "lcd_platform_driver",
		.of_match_table = devNode
	}
};

static int __init lcd_init(void)
{
	pr_info("lcd_init start \n");

	lcdDriver._pClass = class_create(THIS_MODULE, "lcd_class");
	if(IS_ERR(lcdDriver._pClass))
	{
		pr_err("class_create error \n");
		return PTR_ERR(lcdDriver._pClass);
	}
	
	platform_driver_register(&platDrv);
	pr_info("lcd_init end \n");
	return 0;
}

static void __exit lcd_exit(void)
{
	pr_info("lcd_exit start \n");

	class_destroy(lcdDriver._pClass);
	platform_driver_unregister(&platDrv);

	pr_info("lcd_exit end \n");
}

void init_gpio(struct device* pDev)
{
	int i;
	struct gpio_desc** _ppGpioDesc = ((struct _SLcdPrivateData*)dev_get_drvdata(pDev))->_ppGpioDesc;
	dev_info(pDev, "gpio init start \n");

	for(i = 0; i < NUM_OF_GPIO; i++)
	{
		gpiod_direction_output(_ppGpioDesc[i], 0);
	}
	dev_info(pDev, "gpio init end \n");
}

int create_device_files(struct device* pDev)
{
	struct _SLcdPrivateData* privateData = dev_get_drvdata(pDev);

	dev_info(pDev, "create_device_files start \n");

	privateData->_pDev = device_create_with_groups(lcdDrv._pClass, pDev, 0, privateData, ppAttrGroup, "LCD16x2");
	if(IS_ERR(privateData->_pDev))
	{
		dev_err(pDev, "device_create_with_groups error \n");
		return PTR_ERR(privateData->_pDev);
	}

	dev_info(pDev, "create_device_files end \n");
}

ssize_t lcdxy_show(struct device* pDev, struct device_attribute* pDevAttr, char* buf)
{
	return 0;
}

ssize_t lcdxy_store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count)
{

	return 0;
}

ssize_t lcdcmd_store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count)
{

	return 0;
}

ssize_t lcdtext_store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count)
{
	
	return 0;
}
ssize_t store(struct device* pDev, struct device_attribute* pDevAttr, const char* buf, size_t count)
{

	return 0;
}

int lcd_driver_probe(struct platform_device* pPlatDev)
{
	int ret;
	struct device* pDev = &pPlatDev->dev;

	dev_info(pDev, "lcd_driver_probe function start \n");

	dev_set_drvdata(pDev, &lcdPriData);

	// 1. Get struct gpio_desc
	lcdPriData._ppGpioDesc[GPIO_RS_IDX] = gpiod_get(pDev, "rs", GPIOD_ASIS);
	lcdPriData._ppGpioDesc[GPIO_RW_IDX] = gpiod_get(pDev, "rw", GPIOD_ASIS);
	lcdPriData._ppGpioDesc[GPIO_EN_IDX] = gpiod_get(pDev, "en", GPIOD_ASIS);
	lcdPriData._ppGpioDesc[GPIO_D4_IDX] = gpiod_get(pDev, "d4", GPIOD_ASIS);
	lcdPriData._ppGpioDesc[GPIO_D5_IDX] = gpiod_get(pDev, "d5", GPIOD_ASIS);
	lcdPriData._ppGpioDesc[GPIO_D6_IDX] = gpiod_get(pDev, "d6", GPIOD_ASIS);
	lcdPriData._ppGpioDesc[GPIO_D7_IDX] = gpiod_get(pDev, "d7", GPIOD_ASIS);
	if( IS_ERR(lcdPriData._ppGpioDesc[GPIO_RS_IDX]) || \
		IS_ERR(lcdPriData._ppGpioDesc[GPIO_RW_IDX]) || \
		IS_ERR(lcdPriData._ppGpioDesc[GPIO_EN_IDX]) || \
		IS_ERR(lcdPriData._ppGpioDesc[GPIO_D4_IDX]) || \
		IS_ERR(lcdPriData._ppGpioDesc[GPIO_D5_IDX]) || \
		IS_ERR(lcdPriData._ppGpioDesc[GPIO_D6_IDX]) || \
		IS_ERR(lcdPriData._ppGpioDesc[GPIO_D7_IDX]) )
	{
		dev_err(pDev, "gpiod_get error \n");
		return -EINVAL;
	}

	// 2. initialize gpio pins
	init_gpio(pDev);

	// 3. device files create
	ret = create_device_files(pDev);
	if(!ret)
	{
		dev_err(pDev, "create_device_files error \n");
		return ret;
	}
 
	// 4. print "hello world!" to lcd
	init_lcd(pDev);
	print_to_lcd(pDev, "Hello World!");

	dev_info(pDev, "lcd_driver_probe function end \n");
	
	return 0;
}

int lcd_driver_remove(struct platform_device* pPlatDev)
{
	int i;

	for(i = 0; i < NUM_OF_GPIO; i++)
	{
		gpiod_put(lcdPriData._ppGpioDesc[i]);
	}

	return 0;
}

module_init(lcd_init);
module_exit(lcd_exit);
