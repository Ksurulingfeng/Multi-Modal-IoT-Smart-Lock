#ifndef __DOORLOCKCONTROL_H__
#define __DOORLOCKCONTROL_H__

#include "sys.h"
#include "task_headfile.h"

#define PASSWORD_ADDR    0x20 // 密码存放地址
#define PASSWORD_MAX_LEN 16   // 密码最大长度
#define CARD_ADDR        0x50 // 卡号存放地址
#define CARD_SIZE        4    // 卡号长度
#define CARD_MAX_NUM     10   // 最大卡片数量

// 状态定义
typedef enum {
    STATE_INIT,       // 初始化
    STATE_LOCKED,     // 已锁定
    STATE_UNLOCKED,   // 未锁定
    STATE_VERIFING,   // 验证中
    STATE_ALARM,      // 安全异常
    STATE_ADMIN_MODE, // 管理模式
} DoorState;
// 事件定义
typedef enum {
    EVT_NO_EVENT         = (1 << 0), // 无事件
    EVT_VERIFY_ATTEMPT   = (1 << 1), // 尝试验证
    EVT_VERIFY_SUCCESS   = (1 << 2), // 验证成功
    EVT_VERIFY_FAIL      = (1 << 3), // 验证失败
    EVT_VERIFY_TIMEOUT   = (1 << 4), // 验证超时
    EVT_MULTI_FAILED     = (1 << 5), // 多次验证失败
    EVT_ENTER_ADMIN_MODE = (1 << 6), // 进入管理模式
    EVT_NETWORK_UNLOCK   = (1 << 7), // 远程解锁
} DoorEvent;
// 智能门锁结构体
typedef struct {
    DoorState state;                         // 当前状态
    char password[PASSWORD_MAX_LEN + 1];     // 密码
    uint8_t cardID[CARD_MAX_NUM][CARD_SIZE]; // 卡号库
    SemaphoreHandle_t tryCount_Sem;          // 尝试次数信号量
    bool isAdminMode;                        // 是否为管理模式
    bool isConnected;                        // 是否连接网络
} DoorLock_HandleTypeDef;

extern DoorLock_HandleTypeDef doorLock;

void doorLock_init(void);
void Unlock_Door(void);
void Lock_Door(void);
void Success_Sound(void);
void Error_Sound(void);

#endif /* __DOORLOCKCONTROL_H__ */