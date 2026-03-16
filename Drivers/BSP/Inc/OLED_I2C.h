#ifndef __OLED_I2C_H__
#define __OLED_I2C_H__

#include "sys.h"

#define OLED_MODE      0
#define SIZE           8
#define XLevelL        0x00
#define XLevelH        0x10
#define Max_Column     128
#define Max_Row        32
#define Brightness     0xFF
#define X_WIDTH        128
#define Y_WIDTH        32
#define OLED_SIZE_8x16 16
#define OLED_SIZE_6x8  12

//-----------------OLED IIC 软件模拟驱动接口----------------
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_SCL_GPIO_Port, OLED_SCL_Pin, GPIO_PIN_RESET) // SCL
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_SCL_GPIO_Port, OLED_SCL_Pin, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_SDA_GPIO_Port, OLED_SDA_Pin, GPIO_PIN_RESET) // SDA
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_SDA_GPIO_Port, OLED_SDA_Pin, GPIO_PIN_SET)

#define OLED_CMD        0 // 写指令
#define OLED_DATA       1 // 写数据

#define S_I2C           1 // 软件模拟 I2C 接口开关

void OLED_Init(void);
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_Clear_sw(uint8_t x, uint8_t x2, uint8_t y1, uint8_t y2);
void OLED_ShowFNum(uint8_t x, uint8_t y, float num, uint8_t len, uint8_t size, uint8_t mode);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size);
void OLED_ShowChar_Complete(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size, uint8_t mode);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *p, uint8_t Char_Size, uint8_t mode);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_Set_Pos2(uint8_t x, uint8_t y);
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t *s, uint8_t mode);
void OLED_ShowText(uint8_t x, uint8_t y, uint8_t *text, uint8_t Char_Size, uint8_t mode);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[]);

#endif
