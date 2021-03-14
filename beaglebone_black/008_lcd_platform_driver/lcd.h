#ifndef __LCD_H_
#define __LCD_H_

#include "common.h"

void init_lcd(struct deivce*);
void write_cmd(uint8_t, struct device*);
void write_char(uint8_t, struct device*);
void print_to_lcd(const char*);

#endif
