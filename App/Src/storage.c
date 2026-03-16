#include "storage.h"
#include "RC522.h"
#include "AS608.h"
#include "AT24Cxx.h"
#include "Keypad.h"
#include "display.h"
#include "doorLockControl.h"
#include <string.h>
#include "debug.h"

extern AS608_HandleTypeDef has608;
extern RC522_HandleTypeDef hrc522;

bool add_fingerprint(uint16_t page_id)
{
    display_clear();
    display_text(0, 0, "请按下指纹"); // "请按下指纹"

    const TickType_t timeout = pdMS_TO_TICKS(10000);
    TickType_t startTime     = xTaskGetTickCount();
    while (1) {
        if (xTaskGetTickCount() - startTime >= timeout)
            return false; // 超时
        if (AS608_GetFingerState(&has608) == AS608_FINGER_PRESSED) {
            display_clear();
            display_text(0, 0, "正在采集指纹..."); // "正在采集指纹..."
            vTaskDelay(pdMS_TO_TICKS(150));
            if (PS_GetImage(&has608) == ACK_OK) {
                if (PS_GenChar(&has608, BUFFER1) == ACK_OK) {
                    break;
                } else {
                    return false;
                }
            }
        }
    }

    display_clear();
    vTaskDelay(pdMS_TO_TICKS(2000));
    display_text(0, 0, "请再次按下指纹"); // "请再次按下指纹"

    // 再次采集指纹
    startTime = xTaskGetTickCount();
    while (1) {
        if (xTaskGetTickCount() - startTime >= timeout)
            return false; // 超时
        if (AS608_GetFingerState(&has608) == AS608_FINGER_PRESSED) {
            display_clear();
            display_text(0, 0, "正在采集指纹..."); // "正在采集指纹..."
            vTaskDelay(pdMS_TO_TICKS(150));
            if (PS_GetImage(&has608) == ACK_OK) {
                if (PS_GenChar(&has608, BUFFER2) == ACK_OK) {
                    display_clear();
                    break;
                } else {
                    return false;
                }
            }
        }
    }

    // 注册模板并存储
    if (PS_RegModel(&has608) == ACK_OK) {
        if (PS_StoreChar(&has608, BUFFER2, page_id) == ACK_OK) {
            return true;
        }
    }
    return false;
}

bool delete_fingerprint(uint16_t page_id)
{
    return PS_DeletChar(&has608, page_id, 1) == ACK_OK;
}

bool add_card(uint16_t page_id)
{
    if (page_id >= CARD_MAX_NUM) return false; // 超过最大卡片数量

    display_clear();
    display_text(0, 0, "请将卡片靠近"); // "请将卡片靠近"

    const TickType_t timeout = pdMS_TO_TICKS(10000);
    TickType_t startTime     = xTaskGetTickCount();
    uint8_t temp[4]          = {0};
    while (xTaskGetTickCount() - startTime < timeout) {
        if (MFRC522_CheckCard(&hrc522, temp) == MI_OK) {
            // 保存到内存
            memcpy(doorLock.cardID[page_id], temp, CARD_SIZE);
            // 写入EEPROM
            AT24Cxx_Write(CARD_ADDR + page_id * CARD_SIZE, temp, CARD_SIZE);
            return true; // 添加成功
        }
    }
    return false; // 超时
}

bool delete_card(uint16_t page_id)
{
    if (page_id >= CARD_MAX_NUM) return false; // 超过最大卡片数量
    // 内存清空
    memset(doorLock.cardID[page_id], 0, CARD_SIZE);
    // EEPROM清空
    AT24Cxx_Write(CARD_ADDR + page_id * CARD_SIZE, (uint8_t[CARD_SIZE]){0}, CARD_SIZE);
    return true;
}

bool set_password(const char *password)
{
    char current_password[PASSWORD_MAX_LEN + 1] = {0};
    // 保存到内存
    memcpy(doorLock.password, password, PASSWORD_MAX_LEN);
    // 写入EEPROM
    AT24Cxx_Write(PASSWORD_ADDR, (uint8_t *)password, strlen(password) + 1);
    AT24Cxx_Read(PASSWORD_ADDR, (uint8_t *)current_password, strlen(password) + 1);

    return strcmp(current_password, password) == 0;
}
