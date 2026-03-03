#include "Keypad.h"

// 键盘引脚定义 - 根据实际硬件连接修改
#define ROWS 4
#define COLS 3

// 长按判定时间
#define LONG_PRESS_TIME 1000

// 行引脚配置 (输入)
GPIO_TypeDef *row_ports[ROWS] = {ROW0_GPIO_Port, ROW1_GPIO_Port, ROW2_GPIO_Port, ROW3_GPIO_Port};
const uint16_t row_pins[ROWS] = {ROW0_Pin, ROW1_Pin, ROW2_Pin, ROW3_Pin};

// 列引脚配置 (输出)
GPIO_TypeDef *col_ports[COLS] = {COL0_GPIO_Port, COL1_GPIO_Port, COL2_GPIO_Port};
const uint16_t col_pins[COLS] = {COL0_Pin, COL1_Pin, COL2_Pin};

const char Keypad_map[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

char Keypad_Scan(uint8_t *is_long_press)
{
    static uint32_t press_start_time   = 0;
    static uint8_t long_press_detected = 0;
    char pressed_key                   = 0;

    for (int row = 0; row < ROWS; row++) {
        HAL_GPIO_WritePin(row_ports[row], row_pins[row], GPIO_PIN_RESET);
        HAL_Delay(1);

        for (int col = 0; col < COLS; col++) {
            if (HAL_GPIO_ReadPin(col_ports[col], col_pins[col]) == GPIO_PIN_RESET) {
                // 按键按下，记录时间
                if (press_start_time == 0) {
                    press_start_time    = HAL_GetTick();
                    long_press_detected = 0;
                }

                // 等待释放或长按
                while (HAL_GPIO_ReadPin(col_ports[col], col_pins[col]) == GPIO_PIN_RESET) {
                    // 检查是否达到长按阈值
                    if (!long_press_detected &&
                        (HAL_GetTick() - press_start_time) >= LONG_PRESS_TIME) {
                        long_press_detected = 1;
                        if (is_long_press) *is_long_press = 1;
                        pressed_key = Keypad_map[row][col];
                    }
                }

                // 释放后处理
                if (!long_press_detected) {
                    // 短按
                    if (is_long_press) *is_long_press = 0;
                    pressed_key = Keypad_map[row][col];
                }

                // 重置状态
                press_start_time = 0;
                HAL_GPIO_WritePin(row_ports[row], row_pins[row], GPIO_PIN_SET);
                return pressed_key;
            }
        }
        HAL_GPIO_WritePin(row_ports[row], row_pins[row], GPIO_PIN_SET);
    }
    return 0;
}
