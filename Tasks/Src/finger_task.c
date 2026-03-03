#include "task_headfile.h"
#include "sys.h"
#include "AS608.h"
#include "verify.h"
#include "doorLockControl.h"

extern AS608_HandleTypeDef has608;

void vFingerTask(void *pvParameters)
{
    for (;;) {
        if (doorLock.state == STATE_LOCKED) {
            if (AS608_GetFingerState(&has608) == AS608_FINGER_PRESSED) {
                xTaskNotify(verifyTask_handle, (uint32_t)AUTH_FINGER, eSetValueWithOverwrite);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
