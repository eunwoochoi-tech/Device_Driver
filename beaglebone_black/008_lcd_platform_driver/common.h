#ifndef __COMMON_H_
#define __COMMON_H_

#include <linux/device.h>
#include <linux/gpio/consumer.h>

#define	TRUE	1
#define FALSE	0

#define OUTPUT	0
#define INPUT	1

#define HIGH	1
#define LOW		0

#define NUM_OF_GPIO	7

typedef enum {
	GPIO_RS_IDX,
	GPIO_RW_IDX,
	GPIO_EN_IDX,
	GPIO_D4_IDX,
	GPIO_D5_IDX,
	GPIO_D6_IDX,
	GPIO_D7_IDX
} GPIO_IDX;

typedef enum {
	GPIO_D4_SHIFT,
	GPIO_D5_SHIFT,
	GPIO_D6_SHIFT,
	GPIO_D7_SHIFT
}GPIO_SHIFT_COUNT ;

typedef struct _SLcdPrivateData {
	struct device* _pDev;
	struct gpio_desc* _ppGpioDesc[NUM_OF_GPIO];
	int xy;
};

#endif
