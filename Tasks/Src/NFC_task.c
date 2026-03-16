#include "task_headfile.h"
#include "sys.h"
#include "RC522.h"
#include "verify.h"
#include "doorLockControl.h"

extern RC522_HandleTypeDef hrc522;

void vNFCTask(void *pvParameters)
{
    uint8_t errorCount = 0;
    for (;;) {
        if (doorLock.state == STATE_LOCKED) {
            uint8_t status = MFRC522_GetCardState(&hrc522);
            if (status == MI_OK) {
                errorCount = 0;
                xTaskNotify(verifyTask_handle, (uint32_t)AUTH_NFC, eSetValueWithOverwrite);
                vTaskDelay(pdMS_TO_TICKS(500));
            } else if (status == MI_ERR) {
                errorCount++;
                if (errorCount > 10) {
                    MFRC522_Init(&hrc522, hrc522.hspi, hrc522.CS_Port, hrc522.CS_Pin);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}
