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

                        display_clear_area(0, 0, 127, 2);
                        display_text(0, 0, "验证成功"); // "验证成功"
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

                            display_clear_area(0, 0, 127, 2);
                            display_text(0, 0, "验证成功!"); // "验证成功"
                            Success_Sound();

                            if (!doorLock.isAdminMode)
                                doorLock.state = STATE_UNLOCKED;
                            else {
                                doorLock.state = STATE_ADMIN_MODE;
                            }
                            break;

                        case EVT_VERIFY_FAIL:
                            display_clear_area(0, 0, 127, 2);
                            display_text(0, 0, "验证失败!"); // "验证失败"

                            Error_Sound();
                            vTaskDelay(pdMS_TO_TICKS(FAIL_DURATION));
                            display_clear();
                            doorLock.state = STATE_LOCKED;
                            break;

                        case EVT_VERIFY_TIMEOUT:
                            display_clear();
                            display_text(0, 0, "验证超时!"); // "验证超时"

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
                display_text(0, 0, "失败次数过多"); // "失败次数过多"
                display_text(0, 2, "请60s后再试");  //  "请60s后再试"
                Error_Sound();
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
