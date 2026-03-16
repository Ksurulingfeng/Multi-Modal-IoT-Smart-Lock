#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "sys.h"
#include "task_headfile.h"

void display_text(uint8_t x, uint8_t y, const char *str);
void display_number(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void display_float_number(uint8_t x, uint8_t y, float num, uint8_t len);
void display_image(uint8_t x, uint8_t y, uint8_t length, uint8_t width, const uint8_t *image);
void display_clear(void);
void display_clear_area(uint8_t x, uint8_t y, uint8_t x2, uint8_t y2);
void display_highlight(uint8_t new_mode);

#endif /* __DISPLAY_H__ */