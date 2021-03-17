#include "gpio.h"

int gpio_configure_dir(struct gpio_desc* pGpioDesc, int mode, int value)
{
	int ret = 0;
	switch(mode)
	{
		case INPUT:
			ret = gpiod_direction_input(pGpioDesc);
			break;
		case OUTPUT:
		default:
			ret = gpiod_direction_output(pGpioDesc, value);
			break;
	}
	return ret;
}
