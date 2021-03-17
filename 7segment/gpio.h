#ifndef __GPIO_H_
#define __GPIO_H_

#include "7segment.h"

#define OUTPUT	1
#define INPUT	0

int gpio_configure_dir(struct gpio_desc*, int mode, int value);

#endif
