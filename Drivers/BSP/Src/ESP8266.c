#include "ESP8266.h"
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "task_headfile.h"

#define DEBUG_ON 0

// 发送数据
ESP_Status ESP8266_Send_Data(ESP8266_HandleTypeDef *hesp, const char *data)
{
    Log_Info("Tx: %s", data);
    HAL_UART_Transmit(hesp->huart, (uint8_t *)data, strlen(data), 1000);
    return ESP_OK;
}

// 发送AT命令并等待响应
ESP_Status ESP8266_Send_Command(ESP8266_HandleTypeDef *hesp, const char *cmd, const char *expected, uint32_t timeout)
{
    char tx_cmd[256];
    uint32_t tickstart = HAL_GetTick();

    sprintf(tx_cmd, "%s\r\n", cmd);
#if DEBUG_ON
    Log_Info("Tx: %s", tx_cmd);
#endif

    // 清空接收缓冲区
    memset(hesp->rx_buffer, 0, sizeof(hesp->rx_buffer));
    hesp->rx_index = 0;

    // 发送命令
    HAL_UART_Transmit(hesp->huart, (uint8_t *)tx_cmd, strlen(tx_cmd), 1000);

    // 等待响应
    while (HAL_GetTick() - tickstart < timeout) {
        if (strstr((char *)hesp->rx_buffer, expected) != NULL) {
            return ESP_OK;
        }
        if (strstr((char *)hesp->rx_buffer, "ERROR") != NULL) {
            return ESP_ERROR;
        }
    }

    return ESP_TIMEOUT;
}

// 接收中断回调
void ESP8266_RxCallback(ESP8266_HandleTypeDef *hesp)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#if DEBUG_ON
    HAL_UART_Transmit(&huart1, &hesp->rx_byte, 1, 0); // 调试输出
#endif

    if (networkTask_handle != NULL) {
        if (hesp->rx_byte == '\r' || hesp->rx_byte == '\n') {
            xTaskNotifyFromISR(networkTask_handle, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        }
    }

    if (hesp->rx_index < ESP_BUFFER_SIZE - 1) {
        hesp->rx_buffer[hesp->rx_index++] = hesp->rx_byte;
    }

    HAL_UART_Receive_IT(hesp->huart, &hesp->rx_byte, 1);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// 初始化ESP8266
ESP_Status ESP8266_Init(ESP8266_HandleTypeDef *hesp, UART_HandleTypeDef *huart)
{
    hesp->huart    = huart;
    hesp->rx_index = 0;
    hesp->rx_byte  = 0;
    hesp->timeout  = 5000;

    HAL_UART_Receive_IT(huart, &hesp->rx_byte, 1);

    if (ESP8266_Send_Command(hesp, "AT+RST", "OK", 2000) != ESP_OK)
        return ESP_ERROR;
    HAL_Delay(500);

    return ESP_OK;
}

// 复位ESP8266
ESP_Status ESP8266_Reset(ESP8266_HandleTypeDef *hesp)
{
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    // HAL_Delay(100);
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    // HAL_Delay(1000);

    return ESP_OK;
}

// 连接WiFi
ESP_Status ESP8266_Connect_WiFi(ESP8266_HandleTypeDef *hesp, const char *ssid, const char *password)
{
    char cmd[128];

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    return ESP8266_Send_Command(hesp, cmd, "OK", 10000);
}

// 断开WiFi连接
ESP_Status ESP8266_Disconnect_WiFi(ESP8266_HandleTypeDef *hesp)
{
    return ESP8266_Send_Command(hesp, "AT+CWQAP", "OK", 2000);
}
