#include "network.h"
#include "ESP8266.h"
#include "mqtt.h"
#include "doorLockControl.h"
#include "storage.h"
#include "debug.h"
#include <string.h>
#include "LED.h"
#include "Buzzer.h"
#include "config.h"

ESP8266_HandleTypeDef hesp;
MQTT_Client mqtt_client;

void network_init(void)
{
    ESP8266_Init(&hesp, &huart2);
    ESP8266_Send_Command(&hesp, "AT+CWMODE=1", "OK", 1000);
    MQTT_Init(&hesp, &mqtt_client, MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
}

bool connect_wifi(void)
{
    return ESP8266_Connect_WiFi(&hesp, WIFI_SSID, WIFI_PASSWORD) == ESP_OK;
}

bool disconnect_wifi(void)
{
    return ESP8266_Disconnect_WiFi(&hesp) == ESP_OK;
}

bool connect_mqtt(void)
{
    if (!MQTT_Connect(&mqtt_client, MQTT_BROKER, MQTT_PORT)) {
        return false;
    }
    if (MQTT_Subscribe(&mqtt_client, MQTT_TOPIC_SUBSCRIBE)) {
        return true;
    }
    return false;
}

bool disconnect_mqtt(void)
{
    return MQTT_Disconnect(&mqtt_client);
}

bool network_ping(void)
{
    if (ESP8266_Send_Command(&hesp, "AT+CWJAP?", WIFI_SSID, 3000) == ESP_OK)
        return true;
    return false;
}

void network_cmd_process(void)
{
    if (strstr((char *)hesp.rx_buffer, "unlock")) {
        if (doorLock.state == STATE_LOCKED) {
            xTaskNotify(doorLockControlTask_handle, (uint32_t)EVT_NETWORK_UNLOCK, eSetValueWithOverwrite);
            const char msg[] = "{\"id\":\"smartlock_772\",\"status\":\"opened\"}";
            MQTT_Publish_Raw(&mqtt_client, MQTT_TOPIC_PUBLISH, msg, strlen(msg));
            vTaskDelay(pdMS_TO_TICKS(UNLOCK_DURATION));
            const char msg1[] = "{\"id\":\"smartlock_772\",\"status\":\"closed\"}";
            MQTT_Publish_Raw(&mqtt_client, MQTT_TOPIC_PUBLISH, msg1, strlen(msg1));
        } else {
            const char msg[] = "{\"id\":\"smartlock_772\",\"status\":\"busy\"}";
            MQTT_Publish_Raw(&mqtt_client, MQTT_TOPIC_PUBLISH, msg, strlen(msg));
        }
    } else if (strstr((char *)hesp.rx_buffer, "change_password")) {
        if (doorLock.state != STATE_VERIFING) {
            char new_password[PASSWORD_MAX_LEN + 1] = {0};
            uint16_t index;
            char *p = strstr((char *)hesp.rx_buffer, "\"new_password\":\"");
            if (p) p += 16;
            for (index = 0; *p && *p != '\"'; index++) {
                new_password[index] = *p;
                p++;
            }
            new_password[index] = '\0';
            // Log_Info("New Password: %s", new_password);

            // 修改密码函数
            if (set_password(new_password)) {
                const char msg[] = "{\"id\":\"smartlock_772\",\"status\":\"pwd_change_success\"}";
                MQTT_Publish_Raw(&mqtt_client, MQTT_TOPIC_PUBLISH, msg, strlen(msg));
            } else {
                const char msg[] = "{\"id\":\"smartlock_772\",\"status\":\"pwd_change_failed\"}";
                MQTT_Publish_Raw(&mqtt_client, MQTT_TOPIC_PUBLISH, msg, strlen(msg));
            }
        } else {
            const char msg[] = "{\"id\":\"smartlock_772\",\"status\":\"busy\"}";
            MQTT_Publish_Raw(&mqtt_client, MQTT_TOPIC_PUBLISH, msg, strlen(msg));
        }
    }
    memset(hesp.rx_buffer, 0, sizeof(hesp.rx_buffer));
    hesp.rx_index = 0;
}
