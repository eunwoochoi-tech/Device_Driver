#include "common.h"

void gpio_configure_direction(GPIO_IDX idx, int direction, struct device* pDev)
{
	struct _SLcdPrivateData* privateData = dev_get_drvdata(pDev);

	gpiod_direction_output(privateData->_ppGpioDesc[idx], OUTPUT);
}

void gpio_write_value(GPIO_IDX idx, int value, struct device* pDev)
{
	struct _SLcdPrivateData* privateData = dev_get_drvdata(pDev);

	gpiod_set_value(privateData->_ppGpioDesc[idx], value);
}
