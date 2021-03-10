#include "test_driver.h"

struct of_device_id of_test_table[] = {
	{ .compatible = "test-leds" }
};

struct platform_driver platDrv = {
	.probe = platDrv_probe,
	.remove = platDrv_remove,
	.driver = {
		.name = "test_driver",
		.of_match_table = of_test_table
	}
};

static int __init test_init(void)
{
	pr_info("test_init start \n");

	platform_driver_register(&platDrv);

	pr_info("test_init end \n");
	return 0;
}

static void __exit test_exit(void)
{
	pr_info("test_exit start \n");

	platform_driver_unregister(&platDrv);

	pr_info("test_exit end \n");
	return 0;

}

int platDrv_probe(struct platform_device* pPlatDev)
{
	int i = 0;
	int ret = 0;
	int childCount = 0;
	const char* label = NULL;
	struct device* pDev = &pPlatDev->dev;
	struct device_node* parent = pDev->of_node;
	struct device_node* child = NULL;
	struct gpio_desc* pGpioDesc = NULL;

	dev_info(pDev, "platDrv_probe start \n");

	dev_info(pDev, "platform_device's name is %s \n", pPlatDev->name);
	childCount = of_get_child_count(pDev->of_node);

	if(childCount <= 0)
	{
		dev_info(pDev, "It has no child node \n");
	}
	else
	{
		dev_info(pDev, "%d child nodes detected \n", childCount);
		for_each_available_child_of_node(parent, child)
		{
			of_property_read_string(child, "label", &label);
			dev_info(pDev, "%d's info \n", i++);
			dev_info(pDev, "name : %s \n", child->name);
			dev_info(pDev, "label: %s \n", label);
			
			pGpioDesc = devm_fwnode_get_gpiod_from_child(pDev, "led", &child->fwnode, GPIOD_ASIS, label);
			dev_info(pDev, "devm_fwnode_get_gpiod_from_child done \n");
			if(IS_ERR(pGpioDesc))
			{
				dev_err(pDev, "devm_fwnode_get_gpiod_from_child error \n");
			}
			else
			{
				gpiod_direction_output(pGpioDesc, LOW);
				dev_info(pDev,"gpiod_direction_output done \n");
				gpiod_set_value(pGpioDesc, HIGH);
				dev_info(pDev,"gpiod_set_value done \n");
			}
		}
	}

	dev_info(pDev, "platDrv_probe end \n");
	return 0;
}

int platDrv_remove(struct platform_device* pPlatDev)
{
	int ret;
	struct device* pDev = &pPlatDev->dev;
	
	dev_info(pDev, "platDrv_remove start \n");

	dev_info(pDev, "platDrv_remove end \n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EUNWOO");
MODULE_DESCRIPTION("test driver");
