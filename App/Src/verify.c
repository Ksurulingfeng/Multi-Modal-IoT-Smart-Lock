#include "verify.h"
#include "RC522.h"
#include "AS608.h"
#include "Keypad.h"
#include "Buzzer.h"
#include "display.h"
#include "doorLockControl.h"
#include <string.h>

extern AS608_HandleTypeDef has608;
extern RC522_HandleTypeDef hrc522;

VerifyResult_t fingerprint_verify(uint16_t *foundPageID)
{
    uint32_t startTime = xTaskGetTickCount();
    while (xTaskGetTickCount() - startTime < FINGER_TIMEOUT) {
        if (PS_GetImage(&has608) == ACK_OK) {
            if (PS_GenChar(&has608, BUFFER1) == ACK_OK) {
                if (PS_HighSpeedSearch(&has608, BUFFER1, 0, has608.fingerNum, foundPageID) == ACK_OK)
                    return VERIFY_SUCCESS;
                else
                    return VERIFY_FAIL;
            }
        }
    }
    return VERIFY_TIMEOUT;
}

VerifyResult_t nfc_verify(uint16_t *foundPageID)
{
    uint32_t startTime = xTaskGetTickCount();
    uint8_t temp[4]    = {0};
    while (xTaskGetTickCount() - startTime < NFC_TIMEOUT) {
        if (MFRC522_Request(&hrc522, PICC_REQALL, temp) == MI_OK) {
            if (MFRC522_Anticoll(&hrc522, temp) == MI_OK) {
                for (uint8_t i = 0; i < CARD_MAX_NUM; i++) {
                    if (memcmp(doorLock.cardID[i], temp, CARD_SIZE) == 0) {
                        *foundPageID = i;
                        return VERIFY_SUCCESS;
                    }
                }
                return VERIFY_FAIL;
            }
        }
    }
    return VERIFY_TIMEOUT;
}

VerifyResult_t password_verify(void)
{
    uint32_t startTime      = xTaskGetTickCount();
    char input_password[16] = {0};
    uint8_t index           = 0;
    char keyNum             = 0;
    bool isOLEDClear        = false;

    while (xTaskGetTickCount() - startTime < PASSWORD_TIMEOUT) {
        keyNum = Keypad_Scan(NULL);

        if (keyNum == 0) {
            continue;
        }

        if (keyNum && keyNum != '*' && keyNum != '#') {
            Buzzer_Beep(20); // 按键提示音
            if (!isOLEDClear) {
                isOLEDClear = true;
                display_clear_area(0, 2, 127, 4);
            }
            if (index < sizeof(input_password) - 1) {
                input_password[index++] = keyNum;
                display_char(8 * (index - 1), 2, '*');
            }
        } else if (keyNum == '*') { // 退格
            if (index > 0) {
                input_password[--index] = 0;
                display_char(8 * index, 2, ' ');
            }
        } else if (keyNum == '#') { // 确认
            input_password[index] = '\0';
            display_clear();
            return strcmp((char *)input_password, doorLock.password) == 0 ? VERIFY_SUCCESS : VERIFY_FAIL;
        }
    }
    return VERIFY_TIMEOUT;
}
