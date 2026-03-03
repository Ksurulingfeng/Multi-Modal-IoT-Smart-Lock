#ifndef __AS608_H__
#define __AS608_H__

#include "sys.h"

// ==================== 默认数据 ====================
#define AS608_DEFAULT_ADDRESS  0xFFFFFFFF // 默认芯片地址
#define AS608_DEFAULT_PKG_HEAD 0xEF01     // 默认包头

// ==================== 包标识定义 ====================
#define PKG_CMD  0x01 // 命令包
#define PKG_DATA 0x02 // 数据包
#define PKG_END  0x08 // 结束包
#define PKG_ACK  0x07 // 确认包

// ==================== 指令代码定义 ====================
#define CMD_GET_IMAGE          0x01 // 录入图像
#define CMD_GEN_CHAR           0x02 // 生成特征
#define CMD_MATCH              0x03 // 精确比对两枚指纹特征
#define CMD_SEARCH             0x04 // 搜索指纹
#define CMD_REG_MODEL          0x05 // 合并特征（生成模板）
#define CMD_STORE_CHAR         0x06 // 储存模板
#define CMD_LOAD_CHAR          0x07 // 读出模板
#define CMD_UP_CHAR            0x08 // 上传特征或模板
#define CMD_DOWN_CHAR          0x09 // 下载特征或模板
#define CMD_UP_IMAGE           0x0A // 上传图像
#define CMD_DOWN_IMAGE         0x0B // 下载图像
#define CMD_DELETE_CHAR        0x0C // 删除模板
#define CMD_EMPTY              0x0D // 清空指纹库
#define CMD_WRITE_REG          0x0E // 写系统寄存器
#define CMD_READ_SYS_PARA      0x0F // 读系统基本参数
#define CMD_ENROLL             0x10 // 自动注册模板
#define CMD_IDENTIFY           0x11 // 自动验证指纹
#define CMD_SET_PWD            0x12 // 设置口令
#define CMD_VERIFY_PWD         0x13 // 验证口令
#define CMD_GET_RANDOM         0x14 // 采样随机数
#define CMD_SET_ADDR           0x15 // 设置芯片地址
#define CMD_READ_INFO          0x16 // 读flash信息页
#define CMD_PORT_CTRL          0x17 // 端口控制
#define CMD_WRITE_NOTE         0x18 // 写记事本
#define CMD_READ_NOTE          0x19 // 读记事本
#define CMD_BURN_CODE          0x1A // 烧写片内FLASH
#define CMD_HIGH_SPEED_SEARCH  0x1B // 高速搜索
#define CMD_GEN_BIN_IMAGE      0x1C // 生成细化指纹图像
#define CMD_VALID_TEMPLETE_NUM 0x1D // 读有效模板个数
#define CMD_USER_GPIO_COMMAND  0x1E // 用户GPIO控制命令（保留）
#define CMD_READ_INDEX_TABLE   0x1F // 读索引表（保留）

// ==================== 确认码定义 ====================
#define ACK_OK                  0x00 // 指令执行完毕或OK
#define ACK_ERR                 0xFF // 指令执行失败
#define ACK_TIMEOUT             0xFE // 指令超时
#define ACK_RECV_ERR            0x01 // 数据包接收错误
#define ACK_NO_FINGER           0x02 // 传感器上没有手指
#define ACK_IMG_FAIL            0x03 // 录入指纹图像失败
#define ACK_IMG_TOO_DRY         0x04 // 指纹图像太干、太淡而生不成特征
#define ACK_IMG_TOO_WET         0x05 // 指纹图像太湿、太糊而生不成特征
#define ACK_IMG_TOO_MESS        0x06 // 指纹图像太乱而生不成特征
#define ACK_IMG_FEW_FEATURE     0x07 // 指纹图像正常，但特征点太少而生不成特征
#define ACK_NOT_MATCH           0x08 // 指纹不匹配
#define ACK_NOT_FOUND           0x09 // 没搜索到指纹
#define ACK_MERGE_FAIL          0x0A // 特征合并失败
#define ACK_ADDR_OVER           0x0B // 访问指纹库时地址序号超出指纹库范围
#define ACK_READ_ERR            0x0C // 从指纹库读模板出错或无效
#define ACK_UP_CHAR_FAIL        0x0D // 上传特征失败
#define ACK_NO_DATA             0x0E // 模块不能接受后续数据包
#define ACK_UP_IMG_FAIL         0x0F // 上传图像失败
#define ACK_DELETE_FAIL         0x10 // 删除模板失败
#define ACK_CLEAR_FAIL          0x11 // 清空指纹库失败
#define ACK_LOW_POWER_FAIL      0x12 // 不能进入低功耗状态
#define ACK_PWD_WRONG           0x13 // 口令不正确
#define ACK_RESET_FAIL          0x14 // 系统复位失败
#define ACK_NO_VALID_IMG        0x15 // 缓冲区内没有有效原始图而生不成图像
#define ACK_UPGRADE_FAIL        0x16 // 在线升级失败
#define ACK_RESIDUAL_FINGER     0x17 // 残留指纹或两次采集之间手指没有移动过
#define ACK_FLASH_ERR           0x18 // 读写FLASH出错
#define ACK_HAS_MORE_DATA       0xF0 // 有后续数据包的指令，正确接收后用0xF0应答
#define ACK_CMD_RECEIVED        0xF1 // 有后续数据包的指令，命令包用0xF1应答
#define ACK_FLASH_CHECKSUM_ERR  0xF2 // 烧写内部FLASH时，校验和错误
#define ACK_FLASH_PACKET_ERR    0xF3 // 烧写内部FLASH时，包错误
#define ACK_FLASH_PACKET_ERR2   0xF4 // 烧写内部FLASH时，包错误
#define ACK_FLASH_CODE_TOO_LONG 0xF5 // 烧写内部FLASH时，代码长度太长
#define ACK_FLASH_BURN_FAIL     0xF6 // 烧写内部FLASH时，烧写FLASH失败
#define ACK_UNDEFINED_ERR       0x19 // 未定义错误
#define ACK_INVALID_REG         0x1A // 无效寄存器号
#define ACK_REG_CONTENT_ERR     0x1B // 寄存器设定内容错误号
#define ACK_NOTEBOOK_PAGE_ERR   0x1C // 记事本页码指定错误
#define ACK_PORT_OP_FAIL        0x1D // 端口操作失败
#define ACK_ENROLL_FAIL         0x1E // 自动注册（enroll）失败
#define ACK_DB_FULL             0x1F // 指纹库满
#define ACK_RESERVED_START      0x20 // 保留确认码起始
#define ACK_RESERVED_END        0xEF // 保留确认码结束

// ==================== 缓冲区ID ====================
#define BUFFER1 0x01
#define BUFFER2 0x02

// ==================== 状态机定义 ====================
typedef enum {
    AS608_STATE_IDLE,             // 空闲
    AS608_STATE_RECEIVING_PACKET, // 接收数据包
    AS608_STATE_PROCESSING,       // 处理数据包
    AS608_STATE_TIMEOUT,          // 超时
    AS608_STATE_ERROR             // 错误
} AS608_State;
typedef enum {
    AS608_FINGER_NONE,    // 无手指
    AS608_FINGER_PRESSED, // 手指按下
} AS608_FingerStatus;

// ==================== 系统参数结构体 ====================
typedef struct {
    UART_HandleTypeDef *huart;      // 串口句柄
    GPIO_TypeDef *IRQ_Port;         // 中断端口
    uint16_t IRQ_Pin;               // 中断引脚
    AS608_State state;              // 状态机状态
    AS608_FingerStatus fingerState; // 手指状态
    uint16_t timeout;               // 超时时间
    uint16_t fingerNum;             // 自定义指纹容量
    uint16_t fingerPressTime;       // 手指按下时间
    uint32_t chipAddr;              // 芯片地址
} AS608_HandleTypeDef;

// ==================== 函数声明 ====================
// 基本函数
void AS608_Init(AS608_HandleTypeDef *has608, UART_HandleTypeDef *huart, GPIO_TypeDef *IRQ_Port, uint16_t IRQ_Pin);
AS608_FingerStatus AS608_GetFingerState(AS608_HandleTypeDef *has608);
// 高级功能函数
uint8_t PS_GetImage(AS608_HandleTypeDef *has608);
uint8_t PS_GenChar(AS608_HandleTypeDef *has608, uint8_t BufferID);
uint8_t PS_RegModel(AS608_HandleTypeDef *has608);
uint8_t PS_StoreChar(AS608_HandleTypeDef *has608, uint8_t BufferID, uint16_t PageID);
uint8_t PS_DeletChar(AS608_HandleTypeDef *has608, uint16_t PageID, uint16_t N);
uint8_t PS_Empty(AS608_HandleTypeDef *has608);
uint8_t PS_HighSpeedSearch(AS608_HandleTypeDef *has608, uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, uint16_t *foundPageID);
uint8_t PS_WriteReg(AS608_HandleTypeDef *has608, uint8_t RegID, uint8_t Value);
uint8_t PS_ReadSysPara(AS608_HandleTypeDef *has608, uint16_t *para);

#endif /* AS608 */
