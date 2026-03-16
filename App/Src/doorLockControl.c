#include "doorLockControl.h"
#include "LED.h"
#include "Servo.h"
#include "Buzzer.h"
#include "AT24Cxx.h"
#include "display.h"
#include "storage.h"
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "debug.h"

DoorLock_HandleTypeDef doorLock;

void doorLock_init(void)
{
    doorLock.tryCount_Sem = xSemaphoreCreateCounting(MAX_TRY_COUNT, MAX_TRY_COUNT);
    doorLock.isAdminMode  = false;
    doorLock.isConnected  = false;

#if USE_EEPROM
    // AT24Cxx_Write(CARD_ADDR + 1 * 4, (uint8_t[]){0x53, 0xDE, 0xAF, 0x2C}, 4);

    // 读取密码
    for (int i = 0; i < PASSWORD_MAX_LEN; i++) {
        doorLock.password[i] = AT24Cxx_ReadByte(PASSWORD_ADDR + i);
        if (doorLock.password[i] == '\0') break;
    }
    // 读取卡号
    for (uint8_t i = 0; i < CARD_MAX_NUM; i++) {
        AT24Cxx_Read(CARD_ADDR + i * CARD_SIZE, doorLock.cardID[i], CARD_SIZE);
    }
#else
    strcpy(doorLock.password, DEFAUT_PASSWORD);
    memcpy(doorLock.cardID[0], DEFAUT_CARDID_1, CARD_SIZE);
    memcpy(doorLock.cardID[1], DEFAUT_CARDID_2, CARD_SIZE);
    memcpy(doorLock.cardID[2], DEFAUT_CARDID_3, CARD_SIZE);
#endif

    // 打印密码和卡号
    printf("password: %s\n", doorLock.password);
    for (int i = 0; i < CARD_MAX_NUM; i++) {
        uint32_t cardUID = (doorLock.cardID[i][0] << 24) | (doorLock.cardID[i][1] << 16) | (doorLock.cardID[i][2] << 8) | (doorLock.cardID[i][3]);
        printf("cardID[%d]: %08X\n", i, cardUID);
    }

    Lock_Door();
    display_text(32, 0, "智能门锁");
    vTaskDelay(pdMS_TO_TICKS(200));
    LED_Toggle();
    vTaskDelay(pdMS_TO_TICKS(200));
    LED_Toggle();
    vTaskDelay(pdMS_TO_TICKS(1000));
    display_clear();
}

void Unlock_Door(void)
{
    Servo_SetAngle(OPEN_ANGLE);
}

void Lock_Door(void)
{
    Servo_SetAngle(0);
}

void Success_Sound(void)
{
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    for (uint16_t i = 0; i < 100; i++) {
        Buzzer_Toggle();
        vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(1));
    }
}

void Error_Sound(void)
{
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    for (uint16_t i = 0; i < 50; i++) {
        Buzzer_Toggle();
        vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(2));
    }
    vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(50));
    for (uint16_t i = 0; i < 50; i++) {
        Buzzer_Toggle();
        vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(2));
    }
}
