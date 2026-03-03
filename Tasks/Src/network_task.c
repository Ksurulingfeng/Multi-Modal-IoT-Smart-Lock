#include "task_headfile.h"
#include "sys.h"
#include "doorLockControl.h"
#include "ESP8266.h"
#include "mqtt.h"
#include "network.h"
#include "debug.h"
#include <string.h>
#include "config.h"

void vNetworkTask(void *pvParameters)
{
    TickType_t now_tick = 0;

    network_init();
    doorLock.isConnected = connect_wifi() & connect_mqtt();
    if (doorLock.isConnected) {
        Log_Info("网络和MQTT服务器连接成功，成功连接到主题 %s", MQTT_TOPIC_SUBSCRIBE);
    }

    for (;;) {
        // 5分钟检测一次网络连接
        if (xTaskGetTickCount() - now_tick >= pdMS_TO_TICKS(1000 * 60 * 5)) {
            now_tick             = xTaskGetTickCount();
            doorLock.isConnected = network_ping();
            if (!doorLock.isConnected) {
                doorLock.isConnected = connect_wifi() && connect_mqtt();
            }
        }

        if (doorLock.isConnected) {
            if (xTaskNotifyWait(0, 0xffffffff, NULL, pdMS_TO_TICKS(100)) == pdTRUE) {
                network_cmd_process();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
