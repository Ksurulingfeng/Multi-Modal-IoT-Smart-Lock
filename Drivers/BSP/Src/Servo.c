#include "Servo.h"

void Servo_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void Servo_SetAngle(float Angle)
{
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, (uint16_t)(Angle / 180 * 2000 + 500));
}
