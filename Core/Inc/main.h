/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BOARD_LED_Pin GPIO_PIN_13
#define BOARD_LED_GPIO_Port GPIOC
#define AS608_IRQ_Pin GPIO_PIN_14
#define AS608_IRQ_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_15
#define BUZZER_GPIO_Port GPIOC
#define WIFI_TX_Pin GPIO_PIN_2
#define WIFI_TX_GPIO_Port GPIOA
#define WIFI_RX_Pin GPIO_PIN_3
#define WIFI_RX_GPIO_Port GPIOA
#define COL1_Pin GPIO_PIN_4
#define COL1_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_0
#define SPI1_CS_GPIO_Port GPIOB
#define AS608_TX_Pin GPIO_PIN_10
#define AS608_TX_GPIO_Port GPIOB
#define AS608_RX_Pin GPIO_PIN_11
#define AS608_RX_GPIO_Port GPIOB
#define ROW1_Pin GPIO_PIN_13
#define ROW1_GPIO_Port GPIOB
#define ROW2_Pin GPIO_PIN_14
#define ROW2_GPIO_Port GPIOB
#define COL2_Pin GPIO_PIN_15
#define COL2_GPIO_Port GPIOB
#define ROW3_Pin GPIO_PIN_8
#define ROW3_GPIO_Port GPIOA
#define COL0_Pin GPIO_PIN_11
#define COL0_GPIO_Port GPIOA
#define ROW0_Pin GPIO_PIN_12
#define ROW0_GPIO_Port GPIOA
#define SERVO_Pin GPIO_PIN_15
#define SERVO_GPIO_Port GPIOA
#define WIFI_SW_Pin GPIO_PIN_3
#define WIFI_SW_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_6
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOB
#define EEPROM_SCL_Pin GPIO_PIN_8
#define EEPROM_SCL_GPIO_Port GPIOB
#define EEPROM_SDA_Pin GPIO_PIN_9
#define EEPROM_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
