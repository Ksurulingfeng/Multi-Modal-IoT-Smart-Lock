#include "task_headfile.h"
#include "sys.h"
#include "verify.h"
#include "display.h"
#include "manage.h"
#include "doorLockControl.h"
#include "config.h"

void vTimerCallback(TimerHandle_t xTimer);
void vVerifyTask(void *pvParameters)
{
    VerifyResult_t verify_result;
    uint32_t pulNotificationValue;
    uint16_t foundFingerID;
    TimerHandle_t xOneShotTimer = xTimerCreate("xOneShotTimer", pdMS_TO_TICKS(TRY_COUNT_AUTO_RESET_TIME), pdFALSE, 0, vTimerCallback);

    for (;;) {
        if (xTaskNotifyWait(0x00, 0xFFFFFFFF, &pulNotificationValue, portMAX_DELAY) == pdTRUE) {
            // 尝试获取一次机会
            if (xSemaphoreTake(doorLock.tryCount_Sem, 0) == pdTRUE) {
                xTaskNotify(doorLockControlTask_handle, (uint32_t)EVT_VERIFY_ATTEMPT, eSetValueWithOverwrite);
                if (xTimerIsTimerActive(xOneShotTimer) == pdFALSE)
                    xTimerStart(xOneShotTimer, 0);
                AuthType auth_type = (AuthType)pulNotificationValue;
                switch (auth_type) {
                    case AUTH_PASSWORD: {
                        // "请输入密码:"
                        display_chineses(0, 0, (const uint16_t[]){18, 22, 23, 10, 11}, 5);
                        display_string(80, 0, ":");
                        if (doorLock.isAdminMode == false) {
                            display_string(0, 2, "#:bksp      *:ok");
                        }

                        char password[PASSWORD_MAX_LEN + 1] = {0};
                        verify_result                       = password_verify();
                        break;
                    }
                    case AUTH_FINGER: {
                        // "正在验证指纹..."
                        display_chineses(0, 0, (const uint16_t[]){0, 1, 2, 3, 8, 9}, 6);
                        display_string(96, 0, "...");

                        verify_result = fingerprint_verify(&foundFingerID);
                        break;
                    }
                    case AUTH_NFC: {
                        // "正在验证NFC..."
                        display_chineses(0, 0, (const uint16_t[]){0, 1, 2, 3}, 4);
                        display_string(64, 0, "NFC...");

                        verify_result = nfc_verify(&foundFingerID);
                        break;
                    }
                }
                switch (verify_result) {
                    case VERIFY_SUCCESS:
                        xTaskNotify(doorLockControlTask_handle, (uint32_t)EVT_VERIFY_SUCCESS, eSetValueWithOverwrite);
                        if (auth_type == AUTH_FINGER || auth_type == AUTH_NFC) {
                            // 显示识别到的ID
                            display_string(0, 2, "ID:");
                            display_number(24, 2, foundFingerID, 2);
                        }
                        for (uint16_t i = 0; i < MAX_TRY_COUNT; i++) {
                            xSemaphoreGive(doorLock.tryCount_Sem);
                        }
                        break;
                    case VERIFY_FAIL:
                        if (xSemaphoreTake(doorLock.tryCount_Sem, 0) == pdTRUE) {
                            xSemaphoreGive(doorLock.tryCount_Sem);
                            xTaskNotify(doorLockControlTask_handle, (uint32_t)EVT_VERIFY_FAIL, eSetValueWithOverwrite);
                        } else {
                            xTaskNotify(doorLockControlTask_handle, (uint32_t)EVT_MULTI_FAILED, eSetValueWithOverwrite);
                        }
                        break;
                    case VERIFY_TIMEOUT:
                        if (xSemaphoreTake(doorLock.tryCount_Sem, 0) == pdTRUE) {
                            xSemaphoreGive(doorLock.tryCount_Sem);
                            xTaskNotify(doorLockControlTask_handle, (uint32_t)EVT_VERIFY_TIMEOUT, eSetValueWithOverwrite);
                        } else {
                            xTaskNotify(doorLockControlTask_handle, (uint32_t)EVT_MULTI_FAILED, eSetValueWithOverwrite);
                        }
                        break;
                }
            }
        }
    }
}

void vTimerCallback(TimerHandle_t xTimer)
{
    for (uint16_t i = 0; i < MAX_TRY_COUNT; i++) {
        xSemaphoreGive(doorLock.tryCount_Sem);
    }
}
