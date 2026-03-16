#include "display.h"
#include "OLED_I2C.h"

#define DISPLAY_SIZE 16

static uint8_t current_mode = 1;

void display_text(uint8_t x, uint8_t y, const char *str)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_ShowText(x, y, (uint8_t *)str, DISPLAY_SIZE, current_mode);
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_number(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_ShowNum(x, y, num, len, DISPLAY_SIZE, current_mode);
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_float_number(uint8_t x, uint8_t y, float num, uint8_t len)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_ShowFNum(x, y, num, len, DISPLAY_SIZE, current_mode);
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_image(uint8_t x, uint8_t y, uint8_t length, uint8_t width, const uint8_t *image)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_DrawBMP(x, y, length, width, image);
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_clear(void)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_Clear();
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_clear_area(uint8_t x, uint8_t y, uint8_t x2, uint8_t y2)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_Clear_sw(x, x2, y, y2);
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_highlight(uint8_t new_mode)
{
    current_mode = new_mode;
}