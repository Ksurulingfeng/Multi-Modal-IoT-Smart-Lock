#include "manage.h"
#include "doorLockControl.h"
#include "storage.h"
#include "display.h"
#include "Keypad.h"
#include <stdlib.h>
#include "config.h"

typedef enum {
    ADMIN_MODE_ADD_FINGER, // 添加指纹模式
    ADMIN_MODE_DEL_FINGER, // 删除指纹模式
#if USE_EEPROM
    ADMIN_MODE_ADD_CARD, // 添加卡片模式
    ADMIN_MODE_DEL_CARD, // 删除卡片模式
    ADMIN_MODE_SET_PWD,  // 设置密码模式
    ADMIN_MODE_EXIT,     // 退出管理模式
    ADMIN_MODE_NUM,
#else
    ADMIN_MODE_EXIT, // 退出管理模式
    ADMIN_MODE_NUM,
    ADMIN_MODE_ADD_CARD, // 添加卡片模式
    ADMIN_MODE_DEL_CARD, // 删除卡片模式
    ADMIN_MODE_SET_PWD,  // 设置密码模式
#endif
} AdminMode_t;

static void display_title(AdminMode_t mode);
static void process_admin_selection(AdminMode_t mode);
static void add_fingerprint_ui(void);
static void del_fingerprint_ui(void);
static void set_password_ui(void);
static void add_card_ui(void);
static void del_card_ui(void);

void manage_ui(void)
{
    AdminMode_t ui       = ADMIN_MODE_ADD_FINGER;
    char keyNum          = 0;
    uint8_t need_refresh = 1; // 标志位，用于控制界面刷新

    display_clear();

    while (doorLock.isAdminMode) {
        // 刷新界面显示
        if (need_refresh) {
            display_clear();
            display_title(ui); // 显示当前模式标题
            display_text(0, 2, "*:next      #:ok");
            need_refresh = 0;
        }

        keyNum = Keypad_Scan(NULL);

        if (keyNum == 0) {
            // 无按键，继续循环
            continue;
        }

        // "*"键切换模式
        if (keyNum == '*') {
            ui           = (ui + 1) % ADMIN_MODE_NUM; // 总共6个模式
            need_refresh = 1;
        }
        // 确认选择
        else if (keyNum == '#') {
            if (ui == ADMIN_MODE_EXIT) {
                doorLock.isAdminMode = false;
                display_clear();
                return;
            } else
                process_admin_selection(ui);
            need_refresh = 1;
        }
    }
}

// 显示当前模式标题
static void display_title(AdminMode_t mode)
{
    switch (mode) {
        case ADMIN_MODE_ADD_FINGER:
            display_text(32, 0, "添加指纹"); // 添加指纹
            break;
        case ADMIN_MODE_DEL_FINGER:
            display_text(32, 0, "删除指纹"); // 删除指纹
            break;
        case ADMIN_MODE_ADD_CARD:
            display_text(32, 0, "添加卡片"); // 添加卡片
            break;
        case ADMIN_MODE_DEL_CARD:
            display_text(32, 0, "删除卡片"); // 删除卡片
            break;
        case ADMIN_MODE_SET_PWD:
            display_text(32, 0, "设置密码"); // 设置密码
            break;
        case ADMIN_MODE_EXIT:
            display_text(48, 0, "退出"); // 退出
            break;
        default:
            break;
    }
}

// 处理选择的功能
static void process_admin_selection(AdminMode_t mode)
{
    display_clear();

    switch (mode) {
        case ADMIN_MODE_ADD_FINGER:
            add_fingerprint_ui();
            break;

        case ADMIN_MODE_DEL_FINGER:
            del_fingerprint_ui();
            break;

        case ADMIN_MODE_ADD_CARD:
            add_card_ui();
            break;

        case ADMIN_MODE_DEL_CARD:
            del_card_ui();
            break;

        case ADMIN_MODE_SET_PWD:
            set_password_ui();
            break;

        default:
            break;
    }
}

// 添加指纹的UI处理
static void add_fingerprint_ui(void)
{
    char input[8] = {0};
    uint8_t index = 0;
    char keyNum   = 0;

    // 显示提示
    display_text(0, 0, "请输入ID:"); // 请输入ID:
    display_text(0, 2, "#:退出    *:确认");

    while (1) {
        keyNum = Keypad_Scan(NULL);
        if (keyNum == 0) {
            continue;
        }
        // 数字输入
        if (keyNum >= '0' && keyNum <= '9') {
            // 限制ID长度为2位(0~99)
            if (index < 2) {
                input[index] = keyNum;
                display_text(72 + 8 * index, 0, &keyNum);
                index++;
            }
        }
        // *键取消
        else if (keyNum == '*') {
            return;
        }
        // #键确认
        else if (keyNum == '#') {
            if (index == 0) {
                return;
            }
            input[index]     = '\0';
            uint16_t page_id = atoi(input);

            bool result = add_fingerprint(page_id);

            display_clear();
            if (result) {
                display_text(0, 0, "添加成功!"); // 添加成功
            } else {
                display_text(0, 0, "添加失败!"); // 添加失败
            }
            vTaskDelay(pdMS_TO_TICKS(2000));
            return;
        }
    }
}

// 删除指纹的UI处理
static void del_fingerprint_ui(void)
{
    char input[8] = {0};
    uint8_t index = 0;
    char keyNum   = 0;

    // 显示提示
    display_text(0, 0, "请输入ID:"); // 请输入ID:
    display_text(0, 2, "#:退出    *:确认");

    while (1) {
        keyNum = Keypad_Scan(NULL);
        if (keyNum == 0) {
            continue;
        }
        // 数字输入
        if (keyNum >= '0' && keyNum <= '9') {
            // 限制ID长度为2位(0~99)
            if (index <= 2) {
                input[index] = keyNum;
                display_text(72 + 8 * index, 0, &keyNum);
                index++;
            }
        }
        // *键取消
        else if (keyNum == '*') {
            return;
        }
        // #键确认
        else if (keyNum == '#') {
            if (index == 0) {
                return;
            }
            input[index]     = '\0';
            uint16_t page_id = atoi(input);

            bool result = delete_fingerprint(page_id);

            display_clear();
            if (result) {
                display_text(0, 0, "删除成功!"); // 删除成功
            } else {
                display_text(0, 0, "删除失败!"); // 删除失败
            }
            vTaskDelay(pdMS_TO_TICKS(2000));
            return;
        }
    }
}

// 添加卡片的UI处理
static void add_card_ui(void)
{
    char input[8] = {0};
    uint8_t index = 0;
    char keyNum   = 0;

    // 显示提示
    display_text(0, 0, "请输入ID:"); // 请输入ID:
    display_text(0, 2, "#:退出    *:确认");

    while (1) {
        keyNum = Keypad_Scan(NULL);
        if (keyNum == 0) {
            continue;
        }
        // 数字输入
        if (keyNum >= '0' && keyNum <= '9') {
            // 限制ID长度为2位(0~99)
            if (index < 2) {
                input[index] = keyNum;
                display_text(72 + 8 * index, 0, &keyNum);
                index++;
            }
        }
        // *键取消
        else if (keyNum == '*') {
            return;
        }
        // #键确认
        else if (keyNum == '#') {
            if (index == 0) {
                return;
            }
            input[index]     = '\0';
            uint16_t page_id = atoi(input);

            bool result = add_card(page_id);

            display_clear();
            if (result) {
                display_text(0, 0, "添加成功!"); // 添加成功
            } else {
                display_text(0, 0, "添加失败!"); // 添加失败
            }
            vTaskDelay(pdMS_TO_TICKS(2000));
            return;
        }
    }
}

// 删除卡片的UI处理
static void del_card_ui(void)
{
    char input[8] = {0};
    uint8_t index = 0;
    char keyNum   = 0;

    // 显示提示
    display_text(0, 0, "请输入ID:"); // 请输入ID:
    display_text(0, 2, "#:退出    *:确认");

    while (1) {
        keyNum = Keypad_Scan(NULL);
        if (keyNum == 0) {
            continue;
        }
        // 数字输入
        if (keyNum >= '0' && keyNum <= '9') {
            // 限制ID长度为2位(0~99)
            if (index <= 2) {
                input[index] = keyNum;
                display_text(72 + 8 * index, 0, &keyNum);
                index++;
            }
        }
        // *键取消
        else if (keyNum == '*') {
            return;
        }
        // #键确认
        else if (keyNum == '#') {
            if (index == 0) {
                return;
            }
            input[index]     = '\0';
            uint16_t page_id = atoi(input);

            bool result = delete_card(page_id);

            display_clear();
            if (result) {
                display_text(0, 0, "删除成功!"); // 删除成功
            } else {
                display_text(0, 0, "删除失败!"); // 删除失败
            }
            vTaskDelay(pdMS_TO_TICKS(2000));
            return;
        }
    }
}

// 设置密码的UI处理
static void set_password_ui(void)
{
    char password[PASSWORD_MAX_LEN + 1] = {0};
    uint8_t index                       = 0;
    char keyNum                         = 0;
    bool isOLEDClear                    = false;

    display_clear();
    display_text(0, 0, "输入密码:"); // 输入密码:
    display_text(0, 2, "#:退出    *:确认");

    while (1) {
        keyNum = Keypad_Scan(NULL);

        if (keyNum == 0) {
            continue;
        }

        if (keyNum >= '0' && keyNum <= '9') {
            if (!isOLEDClear) {
                isOLEDClear = true;
                display_clear_area(0, 2, 127, 4);
            }
            if (index < PASSWORD_MAX_LEN) {
                password[index] = keyNum;
                display_text(8 * index, 2, &keyNum);
                index++;
            }
        } else if (keyNum == '*') {
            return;
        } else if (keyNum == '#') {
            if (index == 0) {
                return;
            }
            password[index] = '\0';

            display_clear();
            display_text(0, 0, "确认修改密码?"); // 确认修改密码?
            display_text(0, 2, "*:no       #:yes");
            while (1) {
                keyNum = Keypad_Scan(NULL);
                if (keyNum == '*')
                    return;
                else if (keyNum == '#') {
                    display_clear();
                    if (set_password(password)) {
                        display_text(0, 0, "密码修改成功!"); // 密码修改成功
                    } else {
                        display_text(0, 0, "密码修改失败!"); // 密码修改失败
                    }
                    vTaskDelay(pdMS_TO_TICKS(1500));
                    return;
                }
            }
        }
    }
}
