#include "mqtt.h"
#include <string.h>
#include <stdio.h>

void MQTT_Init(ESP8266_HandleTypeDef *hesp, MQTT_Client *client,
               const char *client_id, const char *username, const char *password)
{
    if (client == NULL) return;

    memset(client, 0, sizeof(MQTT_Client));
    strncpy(client->client_id, client_id, sizeof(client->client_id) - 1);

    if (username != NULL) {
        strncpy(client->username, username, sizeof(client->username) - 1);
    }

    if (password != NULL) {
        strncpy(client->password, password, sizeof(client->password) - 1);
    }

    client->hesp          = hesp;
    client->keep_alive    = 0;
    client->clean_session = 1;
    client->connected     = false;
}

// 连接MQTT服务器
bool MQTT_Connect(MQTT_Client *client, const char *host, const char *port)
{
    if (client == NULL || host == NULL || port == NULL) {
        return false;
    }

    // 保存服务器信息
    strncpy(client->host, host, sizeof(client->host) - 1);
    strncpy(client->port, port, sizeof(client->port) - 1);

    char at_cmd[128];

    // 1. 配置MQTT用户参数
    sprintf(at_cmd, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"",
            client->client_id,
            client->username,
            client->password);

    if (ESP8266_Send_Command(client->hesp, at_cmd, "OK", 2000) != ESP_OK)
        return false;

    // 2. 连接MQTT服务器(自动重连)
    sprintf(at_cmd, "AT+MQTTCONN=0,\"%s\",%s,1",
            host, port);

    if (ESP8266_Send_Command(client->hesp, at_cmd, "OK", 10000) != ESP_OK)
        return false;

    client->connected = true;
    return true;
}

// 断开连接
bool MQTT_Disconnect(MQTT_Client *client)
{
    if (client == NULL || !client->connected) {
        return false;
    }

    if (ESP8266_Send_Command(client->hesp, "AT+MQTTCLEAN=0", "OK", 3000) != ESP_OK) {
        return false;
    }

    client->connected = false;
    return true;
}

// 发布消息
bool MQTT_Publish(MQTT_Client *client, const char *topic, const char *message)
{
    if (client == NULL || !client->connected ||
        topic == NULL || message == NULL) {
        return false;
    }

    char at_cmd[128];
    char escaped_msg[128];
    strncpy(escaped_msg, message, sizeof(escaped_msg) - 1);
    sprintf(at_cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0",
            topic, escaped_msg);

    return ESP8266_Send_Command(client->hesp, at_cmd, "OK", 5000) == ESP_OK;
}

bool MQTT_Publish_Raw(MQTT_Client *client, const char *topic, const char *message, uint16_t len)
{
    if (client == NULL || !client->connected ||
        topic == NULL || message == NULL) {
        return false;
    }
    char at_cmd[128];
    sprintf(at_cmd, "AT+MQTTPUBRAW=0,\"%s\",%d,0,0", topic, len);
    if (ESP8266_Send_Command(client->hesp, at_cmd, "OK", 1000) != ESP_OK)
        return false;

    char escaped_msg[128];
    strncpy(escaped_msg, message, sizeof(escaped_msg) - 1);

    return ESP8266_Send_Data(client->hesp, escaped_msg) == ESP_OK;
}

// 订阅主题
bool MQTT_Subscribe(MQTT_Client *client, const char *topic)
{
    if (client == NULL || !client->connected || topic == NULL) {
        return false;
    }

    char at_cmd[128];

    // AT+MQTTSUB=<linkID>,<topic>,<qos>
    sprintf(at_cmd, "AT+MQTTSUB=0,\"%s\",1", topic);

    return ESP8266_Send_Command(client->hesp, at_cmd, "OK", 3000) == ESP_OK;
}

// 取消订阅
bool MQTT_Unsubscribe(MQTT_Client *client, const char *topic)
{
    if (client == NULL || !client->connected || topic == NULL) {
        return false;
    }

    char at_cmd[128];

    // AT+MQTTUNSUB=<linkID>,<topic>
    sprintf(at_cmd, "AT+MQTTUNSUB=0,\"%s\"", topic);

    return ESP8266_Send_Command(client->hesp, at_cmd, "OK", 3000) == ESP_OK;
}

// 检查是否连接
bool MQTT_IsConnected(MQTT_Client *client)
{
    if (client == NULL) return false;
    return client->connected;
}

// 设置保持连接时间
bool MQTT_SetKeepAlive(MQTT_Client *client, uint16_t keepalive)
{
    if (client == NULL) return false;
    client->keep_alive = keepalive;
    return true;
}

// 设置清理会话
bool MQTT_SetCleanSession(MQTT_Client *client, bool clean_session)
{
    if (client == NULL) return false;
    client->clean_session = clean_session ? 1 : 0;
    return true;
}
