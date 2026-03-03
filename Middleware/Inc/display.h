#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "sys.h"
#include "task_headfile.h"

void display_char(uint8_t x, uint8_t y, char chr);
void display_string(uint8_t x, uint8_t y, char *str);
void display_number(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void display_float_number(uint8_t x, uint8_t y, float num, uint8_t len);
void display_chinese(uint8_t x, uint8_t y, uint8_t no);
void display_chineses(uint8_t x, uint8_t y, const uint16_t *no, uint16_t len);
void display_clear(void);
void display_clear_area(uint8_t x, uint8_t y, uint8_t x2, uint8_t y2);
void display_switch(uint8_t mode);

#endif /* __DISPLAY_H__ */