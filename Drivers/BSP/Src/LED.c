#include "LED.h"

void LED_On(void)
{
    HAL_GPIO_WritePin(BOARD_LED_GPIO_Port, BOARD_LED_Pin, GPIO_PIN_RESET);
}

void LED_Off(void)
{
    HAL_GPIO_WritePin(BOARD_LED_GPIO_Port, BOARD_LED_Pin, GPIO_PIN_SET);
}

void LED_Toggle(void)
{
    HAL_GPIO_TogglePin(BOARD_LED_GPIO_Port, BOARD_LED_Pin);
}
