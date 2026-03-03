#include "task_headfile.h"
#include "sys.h"
#include "RC522.h"
#include "verify.h"
#include "doorLockControl.h"

extern RC522_HandleTypeDef hrc522;

void vNFCTask(void *pvParameters)
{
    for (;;) {
        if (doorLock.state == STATE_LOCKED) {
            if (MFRC522_GetCardState(&hrc522) == 1) {
                xTaskNotify(verifyTask_handle, (uint32_t)AUTH_NFC, eSetValueWithOverwrite);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
