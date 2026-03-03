#include "task_headfile.h"
#include "sys.h"
#include "Keypad.h"
#include "verify.h"
#include "doorLockControl.h"

void vKeypadTask(void *pvParameters)
{
    for (;;) {
        if (doorLock.state == STATE_LOCKED) {
            uint8_t is_long = 0;
            char key        = Keypad_Scan(&is_long);
            if (is_long == 1 && key == '*') {
                doorLock.isAdminMode = true;
                xTaskNotify(verifyTask_handle, (uint32_t)AUTH_PASSWORD, eSetValueWithOverwrite);
            } else if (key) {
                doorLock.isAdminMode = false;
                xTaskNotify(verifyTask_handle, (uint32_t)AUTH_PASSWORD, eSetValueWithOverwrite);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
