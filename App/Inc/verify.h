#ifndef __VERIFY_H__
#define __VERIFY_H__

#include "sys.h"
#include "task_headfile.h"

#define FINGER_TIMEOUT            4000   // 指纹识别超时时间(ms)
#define NFC_TIMEOUT               4000   // NFC识别超时时间(ms)
#define PASSWORD_TIMEOUT          20000  // 输入超时时间(ms)
#define TRY_COUNT_AUTO_RESET_TIME 300000 // 尝试计数自动重置时间(ms)

typedef enum {
    VERIFY_SUCCESS, // 验证成功
    VERIFY_FAIL,    // 验证失败
    VERIFY_TIMEOUT, // 验证超时
} VerifyResult_t;

typedef enum {
    AUTH_PASSWORD, // 指纹验证
    AUTH_FINGER,   // 密码验证
    AUTH_NFC,      // NFC验证
} AuthType;

VerifyResult_t fingerprint_verify(uint16_t *foundPageID);
VerifyResult_t nfc_verify(uint16_t *foundPageID);
VerifyResult_t password_verify(void);

#endif /* __VERIFY_H__ */