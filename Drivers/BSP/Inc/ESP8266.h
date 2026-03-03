#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "sys.h"

#define ESP_BUFFER_SIZE 512

typedef enum {
    ESP_OK = 0,
    ESP_ERROR,
    ESP_TIMEOUT,
    ESP_NO_RESPONSE
} ESP_Status;

typedef struct {
    UART_HandleTypeDef *huart;
    uint8_t rx_buffer[ESP_BUFFER_SIZE];
    uint8_t rx_byte;
    uint8_t rx_ready;
    uint16_t rx_index;
    uint32_t timeout;
} ESP8266_HandleTypeDef;

ESP_Status ESP8266_Init(ESP8266_HandleTypeDef *hesp, UART_HandleTypeDef *huart);
ESP_Status ESP8266_Reset(ESP8266_HandleTypeDef *hesp);
ESP_Status ESP8266_Connect_WiFi(ESP8266_HandleTypeDef *hesp, const char *ssid, const char *password);
ESP_Status ESP8266_Disconnect_WiFi(ESP8266_HandleTypeDef *hesp);
ESP_Status ESP8266_Send_Data(ESP8266_HandleTypeDef *hesp, const char *data);
ESP_Status ESP8266_Send_Command(ESP8266_HandleTypeDef *hesp, const char *cmd, const char *expected, uint32_t timeout);
void ESP8266_RxCallback(ESP8266_HandleTypeDef *hesp);

#endif /* __ESP8266_H__ */