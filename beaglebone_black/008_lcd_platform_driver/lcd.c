#include "lcd.h"
#include "common.h"

void init_lcd(struct device* pDev)
{
	gpio_configure_direction(GPIO_RS_IDX, OUTPUT, pDev);
	gpio_configure_direction(GPIO_RW_IDX, OUTPUT, pDev);
	gpio_configure_direction(GPIO_EN_IDX, OUTPUT, pDev);
	gpio_configure_direction(GPIO_D4_IDX, OUTPUT, pDev);
	gpio_configure_direction(GPIO_D5_IDX, OUTPUT, pDev);
	gpio_configure_direction(GPIO_D6_IDX, OUTPUT, pDev);
	gpio_configure_direction(GPIO_D7_IDX, OUTPUT, pDev);
	gpio_write_value(GPIO_RS_IDX, LOW, pDev);
	gpio_write_value(GPIO_RW_IDX, LOW, pDev);
	gpio_write_value(GPIO_EN_IDX, LOW, pDev);

	mdelay(40);

	// function set
	write4bit(0x02, pDev);
	write4bit(0x02, pDev);
	write4bit(0x08, pDev);

	mdelay(40);

	// Display On/Off
	write_cmd(0x00, pDev);
	write_cmd(0x0E, pDev);

	mdelay(40);

	// Entry Mode set
	write_cmd(0x00, pDev);
	write_cmd(0x06, pDev);
	
}

void print_to_lcd(struct device* pDev, const char* buf)
{
	while(buf)
	{
		write_char((uint8_t)*buf, pDev);
	}
}

void write_cmd(uint8_t value, struct device* pDev)
{
	gpio_write_value(GPIO_RS_IDX, LOW, pDev);
	gpio_write_value(GPIO_RW_IDX, LOW, pDev);

	mdelay(1);

	gpio_write_value(GPIO_EN_IDX, HIGH, pDev);

	mdelay(1);

	gpio_write_value(GPIO_D4_IDX, (value >> GPIO_D4_SHIFT) & 0x01, pDev);
	gpio_write_value(GPIO_D5_IDX, (value >> GPIO_D5_SHIFT) & 0x01, pDev);
	gpio_write_value(GPIO_D6_IDX, (value >> GPIO_D6_SHIFT) & 0x01, pDev);
	gpio_write_value(GPIO_D7_IDX, (value >> GPIO_D7_SHIFT) & 0x01, pDev);

	mdelay(1);

	gpio_write_value(GPIO_EN_IDX, LOW, pDev);

	mdelay(1);
}

void write_char(uint8_t value, struct device* pDev)
{
	int i;

	for(i = 1; i >= 0; i--)
	{
		gpio_write_value(GPIO_RS_IDX, HIGH, pDev);
		gpio_write_value(GPIO_RW_IDX, LOW, pDev);

		mdelay(1);

		gpio_write_value(GPIO_EN_IDX, HIGH, pDev);

		mdelay(1);

		gpio_write_value(GPIO_D4_IDX, (value >> (GPIO_D4_SHIFT + (4 * i))), pDev);
		gpio_write_value(GPIO_D5_IDX, (value >> (GPIO_D5_SHIFT + (4 * i))), pDev);
		gpio_write_value(GPIO_D6_IDX, (value >> (GPIO_D6_SHIFT + (4 * i))), pDev);
		gpio_write_value(GPIO_D7_IDX, (value >> (GPIO_D7_SHIFT + (4 * i))), pDev);

		mdelay(1);

		gpio_write_value(GPIO_EN_IDX, LOW, pDev);

		mdelay(1);
	}
}
