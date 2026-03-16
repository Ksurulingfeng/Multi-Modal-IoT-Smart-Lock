#ifndef __RC522_H__
#define __RC522_H__

#include "sys.h"

// ==================== 宏定义 ====================
// 状态枚举，用于大多数函数返回值
#define MI_OK       0 // 操作成功
#define MI_NOTAGERR 1 // 未检测到卡片错误
#define MI_ERR      2 // 通用错误
// MFRC522 命令集
#define PCD_IDLE       0x00 // 空闲命令：取消当前命令执行
#define PCD_AUTHENT    0x0E // 认证密钥命令
#define PCD_RECEIVE    0x08 // 接收数据命令
#define PCD_TRANSMIT   0x04 // 发送数据命令
#define PCD_TRANSCEIVE 0x0C // 发送并接收数据命令
#define PCD_RESETPHASE 0x0F // 复位相位命令
#define PCD_CALCCRC    0x03 // CRC计算命令
// Mifare One 卡片指令集
#define PICC_REQIDL    0x26 // 寻卡：查找天线区域内未休眠的卡片
#define PICC_REQALL    0x52 // 寻卡：查找天线区域内所有卡片
#define PICC_ANTICOLL  0x93 // 防冲突检测
#define PICC_SElECTTAG 0x93 // 选择卡片
#define PICC_AUTHENT1A 0x60 // 使用密钥A认证
#define PICC_AUTHENT1B 0x61 // 使用密钥B认证
#define PICC_READ      0x30 // 读取块数据
#define PICC_WRITE     0xA0 // 写入块数据
#define PICC_DECREMENT 0xC0 // 减值操作(扣款)
#define PICC_INCREMENT 0xC1 // 增值操作(充值)
#define PICC_RESTORE   0xC2 // 将块数据传输到缓冲区
#define PICC_TRANSFER  0xB0 // 将缓冲区数据保存到卡片
#define PICC_HALT      0x50 // 休眠指令
// MFRC522 寄存器定义
// 第0页：命令与状态寄存器
#define MFRC522_REG_RESERVED00  0x00 // 保留寄存器00
#define MFRC522_REG_COMMAND     0x01 // 命令寄存器：启动和停止命令执行
#define MFRC522_REG_COMM_IE_N   0x02 // 通信中断使能寄存器
#define MFRC522_REG_DIV1_EN     0x03 // 分频器使能寄存器
#define MFRC522_REG_COMM_IRQ    0x04 // 通信中断请求寄存器
#define MFRC522_REG_DIV_IRQ     0x05 // 分频器中断请求寄存器
#define MFRC522_REG_ERROR       0x06 // 错误寄存器：显示错误状态
#define MFRC522_REG_STATUS1     0x07 // 状态寄存器1：通信状态
#define MFRC522_REG_STATUS2     0x08 // 状态寄存器2：接收器和发送器状态
#define MFRC522_REG_FIFO_DATA   0x09 // FIFO数据寄存器：输入输出数据流
#define MFRC522_REG_FIFO_LEVEL  0x0A // FIFO级别寄存器：存储的字节数
#define MFRC522_REG_WATER_LEVEL 0x0B // 水位级别寄存器：FIFO警告级别
#define MFRC522_REG_CONTROL     0x0C // 控制寄存器：各种控制位
#define MFRC522_REG_BIT_FRAMING 0x0D // 位帧调节寄存器：位导向帧的调节
#define MFRC522_REG_COLL        0x0E // 冲突位置寄存器：第一个冲突位的位置
#define MFRC522_REG_RESERVED01  0x0F // 保留寄存器01
// 第1页：命令寄存器
#define MFRC522_REG_RESERVED10   0x10 // 保留寄存器10
#define MFRC522_REG_MODE         0x11 // 模式寄存器：定义发送和接收模式
#define MFRC522_REG_TX_MODE      0x12 // 发送模式寄存器
#define MFRC522_REG_RX_MODE      0x13 // 接收模式寄存器
#define MFRC522_REG_TX_CONTROL   0x14 // 发送控制寄存器：控制天线驱动器
#define MFRC522_REG_TX_AUTO      0x15 // 自动发送控制寄存器
#define MFRC522_REG_TX_SELL      0x16 // 发送器自检寄存器
#define MFRC522_REG_RX_SELL      0x17 // 接收器自检寄存器
#define MFRC522_REG_RX_THRESHOLD 0x18 // 接收器阈值寄存器：判决阈值
#define MFRC522_REG_DEMOD        0x19 // 解调器寄存器：解调器设置
#define MFRC522_REG_RESERVED11   0x1A // 保留寄存器11
#define MFRC522_REG_RESERVED12   0x1B // 保留寄存器12
#define MFRC522_REG_MIFARE       0x1C // Mifare寄存器：特殊Mifare接口设置
#define MFRC522_REG_RESERVED13   0x1D // 保留寄存器13
#define MFRC522_REG_RESERVED14   0x1E // 保留寄存器14
#define MFRC522_REG_SERIALSPEED  0x1F // 串行速度寄存器：设置串行接口速率
// 第2页：配置寄存器
#define MFRC522_REG_RESERVED20        0x20 // 保留寄存器20
#define MFRC522_REG_CRC_RESULT_M      0x21 // CRC结果寄存器(高位字节)
#define MFRC522_REG_CRC_RESULT_L      0x22 // CRC结果寄存器(低位字节)
#define MFRC522_REG_RESERVED21        0x23 // 保留寄存器21
#define MFRC522_REG_MOD_WIDTH         0x24 // 调制宽度寄存器：调制脉冲宽度
#define MFRC522_REG_RESERVED22        0x25 // 保留寄存器22
#define MFRC522_REG_RF_CFG            0x26 // RF配置寄存器：接收器增益配置
#define MFRC522_REG_GS_N              0x27 // GsN寄存器：调制器导电性
#define MFRC522_REG_CWGS_PREG         0x28 // 载波波形发生器设置寄存器
#define MFRC522_REG__MODGS_PREG       0x29 // 调制器设置寄存器
#define MFRC522_REG_T_MODE            0x2A // 定时器模式寄存器
#define MFRC522_REG_T_PRESCALER       0x2B // 定时器预分频器寄存器
#define MFRC522_REG_T_RELOAD_H        0x2C // 定时器重载值寄存器(高位字节)
#define MFRC522_REG_T_RELOAD_L        0x2D // 定时器重载值寄存器(低位字节)
#define MFRC522_REG_T_COUNTER_VALUE_H 0x2E // 定时器计数器值寄存器(高位字节)
#define MFRC522_REG_T_COUNTER_VALUE_L 0x2F // 定时器计数器值寄存器(低位字节)
// 第3页：测试寄存器
#define MFRC522_REG_RESERVED30     0x30 // 保留寄存器30
#define MFRC522_REG_TEST_SEL1      0x31 // 测试选择寄存器1
#define MFRC522_REG_TEST_SEL2      0x32 // 测试选择寄存器2
#define MFRC522_REG_TEST_PIN_EN    0x33 // 测试引脚使能寄存器
#define MFRC522_REG_TEST_PIN_VALUE 0x34 // 测试引脚值寄存器
#define MFRC522_REG_TEST_BUS       0x35 // 测试总线寄存器
#define MFRC522_REG_AUTO_TEST      0x36 // 自测试寄存器
#define MFRC522_REG_VERSION        0x37 // 版本寄存器：芯片版本信息
#define MFRC522_REG_ANALOG_TEST    0x38 // 模拟测试寄存器
#define MFRC522_REG_TEST_ADC1      0x39 // ADC测试寄存器1
#define MFRC522_REG_TEST_ADC2      0x3A // ADC测试寄存器2
#define MFRC522_REG_TEST_ADC0      0x3B // ADC测试寄存器0
#define MFRC522_REG_RESERVED31     0x3C // 保留寄存器31
#define MFRC522_REG_RESERVED32     0x3D // 保留寄存器32
#define MFRC522_REG_RESERVED33     0x3E // 保留寄存器33
#define MFRC522_REG_RESERVED34     0x3F // 保留寄存器34
// 通用定义
#define MFRC522_DUMMY   0x00 // 哑字节：用于通信填充
#define MFRC522_MAX_LEN 16   // 缓冲区最大长度(字节)

// ==================== 系统参数结构体 ====================
typedef struct {
    SPI_HandleTypeDef *hspi; // SPI 句柄
    GPIO_TypeDef *CS_Port;   // 片选端口
    uint16_t CS_Pin;         // 片选引脚
    uint16_t timeout;        // 超时时间
} RC522_HandleTypeDef;

// ==================== 函数声明 ====================
uint8_t MFRC522_GetCardState(RC522_HandleTypeDef *hrc522);
void MFRC522_Init(RC522_HandleTypeDef *hrc522, SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin);
uint8_t MFRC522_CheckCard(RC522_HandleTypeDef *hrc522, uint8_t *id);
uint8_t MFRC522_CompareCardID(RC522_HandleTypeDef *hrc522, uint8_t *CardID, uint8_t *CompareID);
uint8_t MFRC522_Request(RC522_HandleTypeDef *hrc522, uint8_t reqMode, uint8_t *TagType);
uint8_t MFRC522_ToCard(RC522_HandleTypeDef *hrc522, uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backLen);
uint8_t MFRC522_Anticoll(RC522_HandleTypeDef *hrc522, uint8_t *serNum);
uint8_t MFRC522_SelectTag(RC522_HandleTypeDef *hrc522, uint8_t *serNum);
uint8_t MFRC522_Auth(RC522_HandleTypeDef *hrc522, uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum);
void MFRC522_Reset(RC522_HandleTypeDef *hrc522);
void MFRC522_AntennaOn(RC522_HandleTypeDef *hrc522);
void MFRC522_AntennaOff(RC522_HandleTypeDef *hrc522);
void MFRC522_Halt(RC522_HandleTypeDef *hrc522);

#endif
