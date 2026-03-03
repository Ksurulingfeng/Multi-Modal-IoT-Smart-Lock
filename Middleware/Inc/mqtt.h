#ifndef __MQTT_H__
#define __MQTT_H__

#include "sys.h"
#include "ESP8266.h"

typedef struct {
    ESP8266_HandleTypeDef *hesp; // ESP8266句柄
    char client_id[32];          // 客户端ID
    char username[32];           // 用户名
    char password[32];           // 密码
    char host[64];               // MQTT服务器地址
    char port[8];                // MQTT服务器端口

    uint16_t keep_alive;   // 心跳间隔
    uint8_t clean_session; // 清除会话标志
    bool connected;        // 连接标志
} MQTT_Client;

// MQTT函数
void MQTT_Init(ESP8266_HandleTypeDef *hesp, MQTT_Client *client,
               const char *client_id, const char *username, const char *password);
bool MQTT_Connect(MQTT_Client *client, const char *host, const char *port);
bool MQTT_Disconnect(MQTT_Client *client);
bool MQTT_Publish(MQTT_Client *client, const char *topic, const char *message);
bool MQTT_Publish_Raw(MQTT_Client *client, const char *topic, const char *message, uint16_t len);
bool MQTT_Subscribe(MQTT_Client *client, const char *topic);
bool MQTT_Unsubscribe(MQTT_Client *client, const char *topic);
bool MQTT_IsConnected(MQTT_Client *client);
bool MQTT_SetKeepAlive(MQTT_Client *client, uint16_t keepalive);
bool MQTT_SetCleanSession(MQTT_Client *client, bool clean_session);

#endif /* __MQTT_H__ */