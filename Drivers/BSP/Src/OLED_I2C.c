#include "OLED_font.h"
#include "OLED_I2C.h"
#include <stdlib.h>

// 如果S_I2C=1,则使用软件模拟I2C接口, 否则使用硬件I2C接口
// OLED 的显存
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

/**********************************************
//Software IIC Start
**********************************************/
static void IIC_Start(void)
{
    OLED_SCLK_Set();
    OLED_SDIN_Set();
    OLED_SDIN_Clr();
    OLED_SCLK_Clr();
}

/**********************************************
//Software IIC Stop
**********************************************/
static void IIC_Stop(void)
{
    OLED_SCLK_Set();
    OLED_SDIN_Clr();
    OLED_SDIN_Set();
}

/**********************************************
//Software IIC Ack
**********************************************/
static void IIC_Wait_Ack(void)
{
    OLED_SCLK_Set();
    OLED_SCLK_Clr();
}

/**********************************************
// IIC Write byte
**********************************************/
static void Write_IIC_Byte(uint8_t IIC_Byte)
{
    uint8_t i;
    uint8_t m, da;
    da = IIC_Byte;
    OLED_SCLK_Clr();
    for (i = 0; i < 8; i++) {
        m = da;
        m = m & 0x80;
        if (m == 0x80) {
            OLED_SDIN_Set();
        } else
            OLED_SDIN_Clr();
        da = da << 1;
        OLED_SCLK_Set();
        OLED_SCLK_Clr();
    }
}

/**********************************************
// IIC Write Command
**********************************************/
static void Write_IIC_Command(uint8_t IIC_Command)
{
#if S_I2C
    IIC_Start();
    Write_IIC_Byte(0x78); // Slave address,SA0=0
    IIC_Wait_Ack();
    Write_IIC_Byte(0x00); // write command
    IIC_Wait_Ack();
    Write_IIC_Byte(IIC_Command);
    IIC_Wait_Ack();
    IIC_Stop();
#else
    HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &IIC_Command, 1, 100);
#endif
}

/**********************************************
// IIC Write Data
**********************************************/
static void Write_IIC_Data(uint8_t IIC_Data)
{
#if S_I2C
    IIC_Start();
    Write_IIC_Byte(0x78); // Slave address,SA0=0
    IIC_Wait_Ack();
    Write_IIC_Byte(0x40); // write data
    IIC_Wait_Ack();
    Write_IIC_Byte(IIC_Data);
    IIC_Wait_Ack();
    IIC_Stop();
#else
    HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, &IIC_Data, 1, 100);
#endif
}

/**********************************************
//写入函数
**********************************************/
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
    if (cmd) {
        Write_IIC_Data(dat);
    } else {
        Write_IIC_Command(dat);
    }
}

/**********************************************
//取反写入函数
**********************************************/
void OLED_WR_Byte_Inverse(uint8_t dat, uint8_t cmd)
{
    dat = ~dat;
    if (cmd) {
        Write_IIC_Data(dat);
    } else {
        Write_IIC_Command(dat);
    }
}

/**********************************************
//Set Position
//坐标设置
**********************************************/
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}
void OLED_Set_Pos2(uint8_t x, uint8_t y)
{
    OLED_WR_Byte(((y & 0xb0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}

/**********************************************
//Turn on OLED display
//开启OLED显示
**********************************************/
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
    OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON
    OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON
}

/**********************************************
//Turn off OLED display
//关闭OLED显示
**********************************************/
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
    OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
    OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}

/**********************************************
//清屏函数,清完屏,整个屏幕是黑色的!
**********************************************/
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++) {
        OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
        OLED_WR_Byte(0x00, OLED_CMD);     // 设置显示位置—列低地址
        OLED_WR_Byte(0x10, OLED_CMD);     // 设置显示位置—列高地址
        for (n = 0; n < 128; n++) { OLED_WR_Byte(0, OLED_DATA); }
    } // 更新显示
}

/**********************************************
//部分清屏
**********************************************/
void OLED_Clear_sw(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2)
{
    uint8_t i, n;
    for (i = y1; i < y2; i++) {
        for (n = x1; n < x2; n++) {
            OLED_Set_Pos(n, i);
            OLED_WR_Byte(0, OLED_DATA);
        }
    } // 更新显示
}

/**********************************************
//亮屏函数，整个屏幕点亮!
**********************************************/
void OLED_On(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++) {
        OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
        OLED_WR_Byte(0x00, OLED_CMD);     // 设置显示位置—列低地址
        OLED_WR_Byte(0x10, OLED_CMD);     // 设置显示位置—列高地址
        for (n = 0; n < 128; n++) OLED_WR_Byte(1, OLED_DATA);
    } // 更新显示
}

/**********************************************
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//chr：要显示的字符
//size:选择字体 16/12
**********************************************/
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
    uint8_t c = 0, i = 0;
    c = chr - ' '; // 得到偏移后的值，为什么做偏移可查看 ASCII 表
    if (x > Max_Column - 1) {
        x = 0;
        y = y + 2;
    }
    if (Char_Size == 16) {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
        OLED_Set_Pos(x, y + 1);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
    } else {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 6; i++)
            OLED_WR_Byte(F6x8[c][i], OLED_DATA);
    }
}

/**********************************************
//在指定位置显示一个字符,包括部分字符，添加了反白显示
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示
//chr：要显示的字符
//size:选择字体 16/12
**********************************************/
void OLED_ShowChar_Complete(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size, uint8_t mode)
{
    uint8_t c = 0, i = 0;
    c = chr - ' '; // 得到偏移后的值，为什么做偏移可查看 ASCII 表
    if (x > Max_Column - 1) {
        x = 0;
        y = y + 2;
    }
    if (mode == 0) {
        if (Char_Size == 16) {
            OLED_Set_Pos(x, y);
            for (i = 0; i < 8; i++)
                OLED_WR_Byte_Inverse(F8X16[c * 16 + i], OLED_DATA);
            OLED_Set_Pos(x, y + 1);
            for (i = 0; i < 8; i++)
                OLED_WR_Byte_Inverse(F8X16[c * 16 + i + 8], OLED_DATA);
        } else {
            OLED_Set_Pos(x, y);
            for (i = 0; i < 6; i++)
                OLED_WR_Byte_Inverse(F6x8[c][i], OLED_DATA);
        }
    }
    if (mode) {
        if (Char_Size == 16) {
            OLED_Set_Pos(x, y);
            for (i = 0; i < 8; i++)
                OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
            OLED_Set_Pos(x, y + 1);
            for (i = 0; i < 8; i++)
                OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
        } else {
            OLED_Set_Pos(x, y);
            for (i = 0; i < 6; i++)
                OLED_WR_Byte(F6x8[c][i], OLED_DATA);
        }
    }
}

/**********************************************
//m^n函数
**********************************************/
static uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--) result *= m;
    return result;
}

/**********************************************
//显示2个数字
//x:0~127
//y:0~63
//num:数值(0~4294967295);
//len :数字的位数
//size:字体大小
**********************************************/
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode) 
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++) {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                OLED_ShowChar_Complete(x + (size / 2) * t, y, ' ', size, mode);
                continue;
            } else
                enshow = 1;
        }
        OLED_ShowChar_Complete(x + (size / 2) * t, y, temp + '0', size, mode);
    }
}

/**********************************************
// 显示负数、浮点数字
// x,y :起点坐标
// num :要显示的数字
// len :数字的位数，包括小数点 len=4时显示x.xx
// size:字体大小
// mode:0,反色显示;1,正常显示
//@n，小数点后位数，默认为2
**********************************************/
void OLED_ShowFNum(uint8_t x, uint8_t y, float num, uint8_t len, uint8_t size, uint8_t mode)
{
    uint8_t t, temp, i = 0, m = 0, n = 2;
    uint8_t enshow = 0, pointshow = 0;
    uint16_t k;
    len--;
    if (size == 8) m = 2;
    if (num < 0) {
        num = -num;
        i   = 1; // 负数标志
    }
    k = num * oled_pow(10, n); // 此处为显示一位小数*10转化为整数
    for (t = 0; t < len; t++) {
        temp = (k / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 2)) {
            if (temp == 0) {
                if (((k / oled_pow(10, len - t - 2) % 10) != 0) && (i == 1)) // 判断是否为负数且在最高位前一位
                {
                    OLED_ShowChar_Complete(x + (size / 2 + m) * t, y, '-', size, mode);

                    i = 0; // 清除判断后一位的标志
                } else
                    OLED_ShowChar_Complete(x + (size / 2 + m) * t, y, '0', size, mode); // 如果没到数字就显示0

                continue;
            } else
                enshow = 1; // 此处是判断是否要显示数字
        }
        if (t == len - n) // 判断是否为最后一位的前一位（显示一位小数）
        {
            OLED_ShowChar_Complete(x + (size / 2 + m) * t, y, '.', size, mode);
            OLED_ShowChar_Complete(x + (size / 2 + m) * (t + 1), y, temp + '0', size, mode);
            pointshow = 1;
            continue;
        }
        if (pointshow == 1) {
            OLED_ShowChar_Complete(x + (size / 2 + m) * (t + 1), y, temp + '0', size, mode);

        } else
            OLED_ShowChar_Complete(x + (size / 2 + m) * t, y, temp + '0', size, mode);
        // 一位一位显示下去
    }
}

/**********************************************
//显示一个字符号串
//Char——Size影响的是两字符间间距
**********************************************/
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size, uint8_t mode)
{
    uint8_t j = 0;
    while (chr[j] != '\0') {
        OLED_ShowChar_Complete(x, y, chr[j], Char_Size, mode);
        x += Char_Size / 2;
        if (x > 120) {
            x = 0;
            y += 2;
        }
        j++;
    }
}

/**********************************************
//显示汉字
**********************************************/
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t *s, uint8_t mode)
{
    uint16_t HZnum = sizeof(tfont16) / sizeof(typFNT_GB16); // 统计汉字数目

    for (uint16_t k = 0; k < HZnum; k++) {
        if ((tfont16[k].Index[0] == *(s)) &&
            (tfont16[k].Index[1] == *(s + 1)) &&
            (tfont16[k].Index[2] == *(s + 2))) {
            OLED_Set_Pos(x, y);
            for (uint8_t i = 0; i < 16; i++) {
                if (mode == 0)
                    OLED_WR_Byte_Inverse(tfont16[k].Msk[i], OLED_DATA);
                else
                    OLED_WR_Byte(tfont16[k].Msk[i], OLED_DATA);
            }
            OLED_Set_Pos(x, y + 1);
            for (uint8_t i = 16; i < 32; i++) {
                if (mode == 0)
                    OLED_WR_Byte_Inverse(tfont16[k].Msk[i], OLED_DATA);
                else
                    OLED_WR_Byte(tfont16[k].Msk[i], OLED_DATA);
            }
            break;
        }
    }
}

/**********************************************
//显示文本
**********************************************/
void OLED_ShowText(uint8_t x, uint8_t y, uint8_t *text, uint8_t Char_Size, uint8_t mode)
{
    while (*text) {
        // 判断是否为汉字（GB2312编码中汉字高位为1）
        if (*text >= 0x81 && *(text + 1) >= 0x40) {
            // 显示汉字（占3字节）
            OLED_ShowCHinese(x, y, text, mode);
            text += 3; // 跳过3字节汉字编码
            x += 16;   // 汉字占16列
        } else {
            // 显示ASCII字符（原有的显示字符函数）
            OLED_ShowChar_Complete(x, y, *text, Char_Size, mode);
            text += 1; // 跳过1字节ASCII码
            x += 8;    // ASCII字符占8列
        }
    }
}

/**********************************************
功能描述：显示BMP图片
//x0，y0视屏幕而定
**********************************************/
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[])
{
    uint32_t j = 0;
    uint8_t x, y;

    if (y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;
    for (y = y0; y < y1; y++) {
        OLED_Set_Pos(x0, y);
        for (x = x0; x < x1; x++) {
            OLED_WR_Byte(BMP[j++], OLED_DATA);
        }
    }
}

/**********************************************
//初始化SSD1306
**********************************************/
void OLED_Init(void)
{
    OLED_WR_Byte(0xAE, OLED_CMD); // 关闭显示
    OLED_WR_Byte(0x40, OLED_CMD); //---set low column address
    OLED_WR_Byte(0xB0, OLED_CMD); //---set high column address
    OLED_WR_Byte(0xC8, OLED_CMD); //-not offset

    OLED_WR_Byte(0x81, OLED_CMD); // 设置对比度
    OLED_WR_Byte(0xff, OLED_CMD);

    OLED_WR_Byte(0xa1, OLED_CMD); // 段重定向设置
    OLED_WR_Byte(0xa6, OLED_CMD); //

    OLED_WR_Byte(0xa8, OLED_CMD); // 设置驱动路数
    OLED_WR_Byte(0x1f, OLED_CMD);

    OLED_WR_Byte(0xd3, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);

    OLED_WR_Byte(0xd5, OLED_CMD);
    OLED_WR_Byte(0xf0, OLED_CMD);

    OLED_WR_Byte(0xd9, OLED_CMD);
    OLED_WR_Byte(0x22, OLED_CMD);

    OLED_WR_Byte(0xda, OLED_CMD);
    OLED_WR_Byte(0x02, OLED_CMD);

    OLED_WR_Byte(0xdb, OLED_CMD);
    OLED_WR_Byte(0x49, OLED_CMD);

    OLED_WR_Byte(0x8d, OLED_CMD);
    OLED_WR_Byte(0x14, OLED_CMD);

    OLED_WR_Byte(0xaf, OLED_CMD);
    OLED_Clear();
}
