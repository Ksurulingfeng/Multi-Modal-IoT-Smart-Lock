#include "task_headfile.h"
#include "sys.h"
#include "doorLockControl.h"
#include "display.h"
#include "manage.h"
#include "debug.h"
#include "config.h"

void vDoorLockControlTask(void *pvParameters)
{
    uint32_t pulNotificationValue;
    doorLock.state = STATE_INIT;
    for (;;) {
        switch (doorLock.state) {
            case STATE_INIT:
                doorLock_init();
                doorLock.state = STATE_LOCKED;
                break;

            case STATE_LOCKED:
                if (xTaskNotifyWait(0x00, 0xFFFFFFFF, &pulNotificationValue, portMAX_DELAY) == pdTRUE) {
                    DoorEvent event = (DoorEvent)pulNotificationValue;
                    if (event == EVT_VERIFY_ATTEMPT)
                        doorLock.state = STATE_VERIFING;
                    else if (event == EVT_ENTER_ADMIN_MODE) {
                        doorLock.state = STATE_VERIFING;
                    } else if (event == EVT_NETWORK_UNLOCK) {
                        Unlock_Door();
                        Success_Sound();
                        // "验证成功"
                        display_clear_area(0, 0, 127, 2);
                        display_chineses(0, 0, (const uint16_t[]){2, 3, 4, 5}, 4);
                        doorLock.state = STATE_UNLOCKED;
                    }
                }
                break;

            case STATE_UNLOCKED:
                vTaskDelay(pdMS_TO_TICKS(UNLOCK_DURATION));
                Lock_Door();
                display_clear();
                doorLock.state = STATE_LOCKED;
                break;

            case STATE_VERIFING:
                if (xTaskNotifyWait(0x00, 0xFFFFFFFF, &pulNotificationValue, portMAX_DELAY) == pdTRUE) {
                    DoorEvent event = (DoorEvent)pulNotificationValue;
                    switch (event) {
                        case EVT_VERIFY_SUCCESS:
                            if (!doorLock.isAdminMode)
                                Unlock_Door();
                            // "验证成功"
                            display_clear_area(0, 0, 127, 2);
                            display_chineses(0, 0, (const uint16_t[]){2, 3, 4, 5}, 4);
                            Success_Sound();

                            if (!doorLock.isAdminMode)
                                doorLock.state = STATE_UNLOCKED;
                            else {
                                doorLock.state = STATE_ADMIN_MODE;
                            }
                            break;

                        case EVT_VERIFY_FAIL:
                            // "验证失败"
                            display_clear_area(0, 0, 127, 2);
                            display_chineses(0, 0, (const uint16_t[]){2, 3, 6, 7}, 4);

                            Error_Sound();
                            vTaskDelay(pdMS_TO_TICKS(FAIL_DURATION));
                            display_clear();
                            doorLock.state = STATE_LOCKED;
                            break;

                        case EVT_VERIFY_TIMEOUT:
                            // "验证超时"
                            display_clear();
                            display_chineses(0, 0, (const uint16_t[]){2, 3, 12, 13}, 4);

                            Error_Sound();
                            vTaskDelay(pdMS_TO_TICKS(FAIL_DURATION));
                            display_clear();
                            doorLock.state = STATE_LOCKED;
                            break;

                        case EVT_MULTI_FAILED:
                            doorLock.state = STATE_ALARM;
                            break;

                        default:
                            break;
                    }
                }
                break;

            case STATE_ALARM:
                display_clear();
                // "失败次数过多"
                display_chineses(0, 0, (const uint16_t[]){6, 7, 14, 15, 16, 17}, 6);
                Error_Sound();
                //  "请60s后再试"
                display_chinese(0, 2, 18);
                display_string(32, 2, "s");
                display_chineses(48, 2, (const uint16_t[]){19, 20, 21}, 3);
                // 锁定60s
                TickType_t pxPreviousWakeTime = xTaskGetTickCount();
                for (uint16_t i = 0; i < ALARM_DURATION; i++) {
                    display_number(16, 2, ALARM_DURATION - i, 2);
                    vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(1000));
                }
                // 重置尝试次数
                for (uint16_t i = 0; i < MAX_TRY_COUNT; i++) {
                    xSemaphoreGive(doorLock.tryCount_Sem);
                }
                display_clear();
                doorLock.state = STATE_LOCKED;
                break;

            case STATE_ADMIN_MODE:
                manage_ui();
                doorLock.state = STATE_LOCKED;
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
