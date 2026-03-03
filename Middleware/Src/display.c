#include "display.h"
#include "OLED_I2C.h"

#define DISPLAY_SIZE 16

void display_char(uint8_t x, uint8_t y, char chr)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_ShowChar(x, y, (uint8_t)chr, DISPLAY_SIZE);
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_string(uint8_t x, uint8_t y, char *str)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_ShowString(x, y, (uint8_t *)str, DISPLAY_SIZE, 1);
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_number(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_ShowNum(x, y, num, len, DISPLAY_SIZE);
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_float_number(uint8_t x, uint8_t y, float num, uint8_t len)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_ShowFNum(x, y, num, len, DISPLAY_SIZE, 1);
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_chinese(uint8_t x, uint8_t y, uint8_t no)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        OLED_ShowCHinese(x, y, no);
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}

void display_chineses(uint8_t x, uint8_t y, const uint16_t *no, uint16_t len)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        for (uint16_t i = 0; i < len; i++) {
            OLED_ShowCHinese(x + i * 16, y, no[i]);
        }
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

void display_switch(uint8_t mode)
{
    if (xSemaphoreTake(xSemphore_Display_Mutex, pdTICKS_TO_MS(500)) == pdTRUE) {
        mode ? OLED_Display_On() : OLED_Display_Off();
        xSemaphoreGive(xSemphore_Display_Mutex);
    }
}
