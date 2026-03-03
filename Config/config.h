#ifndef __CONFIG_H__
#define __CONFIG_H__

// 系统配置
#define USE_EEPROM 1 // 是否使用EEPROM

// 网络配置
#define WIFI_SSID            "*******"        // WiFi名称
#define WIFI_PASSWORD        ""*******""           // WiFi密码
#define MQTT_BROKER          ""*******""        // MQTT服务器地址
#define MQTT_PORT            ""*******""                  // MQTT服务器端口
#define MQTT_CLIENT_ID       ""*******""         // MQTT客户端ID
#define MQTT_USERNAME        ""                      // MQTT用户名
#define MQTT_PASSWORD        ""                      // MQTT密码
#define MQTT_TOPIC_SUBSCRIBE ""*******"" // MQTT订阅主题
#define MQTT_TOPIC_PUBLISH   ""*******""  // MQTT发布主题

// 门锁配置
#define OPEN_ANGLE      73   // 开门角度
#define MAX_TRY_COUNT   6    // 最大尝试次数
#define UNLOCK_DURATION 2500 // 解锁持续时间(ms)
#define FAIL_DURATION   1500 // 失败提示持续时间(ms)
#define ALARM_DURATION  60   // 报警持续时间(s)

// 验证配置
#define DEFAUT_PASSWORD ""*******""                               // 默认密码
#define DEFAUT_CARDID_1 (uint8_t[4]){0x00, 0x00, 0x00, 0x00} // 默认卡号1
#define DEFAUT_CARDID_2 (uint8_t[4]){0x00, 0x00, 0x00, 0x00} // 默认卡号2
#define DEFAUT_CARDID_3 (uint8_t[4]){0x00, 0x00, 0x00, 0x00} // 默认卡号3

#endif /* __CONFIG_H__ */
