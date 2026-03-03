#include "AS608.h"
#include <string.h>

/**
 * @brief AS608初始化
 *
 * @param has608 AS608句柄
 * @param huart 串口句柄
 */
void AS608_Init(AS608_HandleTypeDef *has608, UART_HandleTypeDef *huart, GPIO_TypeDef *IRQ_Port, uint16_t IRQ_Pin)
{
    has608->huart           = huart;
    has608->IRQ_Port        = IRQ_Port;
    has608->IRQ_Pin         = IRQ_Pin;
    has608->chipAddr        = AS608_DEFAULT_ADDRESS;
    has608->timeout         = 1000;
    has608->state           = AS608_STATE_IDLE;
    has608->fingerState     = AS608_FINGER_NONE;
    has608->fingerPressTime = 0;
    has608->fingerNum       = 16;

    // HAL_UARTEx_ReceiveToIdle_DMA(&huart, (uint8_t *)has608->rx_buf, 64);
    // __HAL_UART_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
}

/**
 * @brief AS608发送数据
 *
 * @param has608 AS608句柄
 * @param data 待发送数据
 * @param len 待发送数据长度
 */
static void AS608_Send(AS608_HandleTypeDef *has608, uint8_t *data, uint8_t len)
{
    HAL_UART_Transmit(&huart3, data, len, HAL_MAX_DELAY);
}

/**
 * @brief 获取手指状态
 *
 * @param has608 AS608句柄
 * @param cmd 指令
 * @param params 指令参数
 * @param param_len 指令参数长度
 * @param response 响应包源数据
 * @return 手指状态
 */
AS608_FingerStatus AS608_GetFingerState(AS608_HandleTypeDef *has608)
{
    if (HAL_GPIO_ReadPin(has608->IRQ_Port, has608->IRQ_Pin) == GPIO_PIN_SET)
        has608->fingerState = AS608_FINGER_PRESSED;
    else
        has608->fingerState = AS608_FINGER_NONE;
    return has608->fingerState;
}

/**
 * @brief 计算校验和
 *
 * @param data 待计算数据
 * @param len 待计算数据长度
 * @return 校验和
 */
static uint16_t AS608_CalculateChecksum(uint8_t *data, uint16_t len)
{
    uint32_t sum = 0;
    for (uint16_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return (uint16_t)(sum & 0xFFFF);
}

/**
 * @brief 发送命令并返回确认码
 *
 * @param has608 AS608句柄
 * @param cmd 指令
 * @param params 指令参数
 * @param param_len 指令参数长度
 * @param response 响应包源数据
 * @return 确认码
 */
static uint8_t AS608_SendCmd(AS608_HandleTypeDef *has608, uint8_t cmd,
                             uint8_t *params, uint8_t param_len,
                             uint8_t *response)
{
    uint8_t packet[32];
    uint8_t resp[32];

    // 包头
    packet[0] = 0xEF;
    packet[1] = 0x01;
    // 芯片地址
    packet[2] = (has608->chipAddr >> 24) & 0xFF;
    packet[3] = (has608->chipAddr >> 16) & 0xFF;
    packet[4] = (has608->chipAddr >> 8) & 0xFF;
    packet[5] = has608->chipAddr;
    // 命令包标识
    packet[6] = PKG_CMD;
    // 包长度 = 指令(1) + 参数(n) + 校验和(2)
    uint16_t data_len = 1 + param_len + 2;
    packet[7]         = (data_len >> 8) & 0xFF;
    packet[8]         = data_len & 0xFF;
    // 指令
    packet[9] = cmd;
    // 参数
    for (uint8_t i = 0; i < param_len; i++) {
        packet[10 + i] = params[i];
    }
    // 计算校验和
    uint16_t checksum      = AS608_CalculateChecksum(&packet[6], 4 + param_len);
    packet[10 + param_len] = (checksum >> 8) & 0xFF;
    packet[11 + param_len] = checksum & 0xFF;
    // 发送
    HAL_UART_Transmit(has608->huart, packet, 12 + param_len, has608->timeout);
    // 接收响应
    uint16_t rxLen;
    if (HAL_UARTEx_ReceiveToIdle(has608->huart, resp, 32, &rxLen, has608->timeout) == HAL_TIMEOUT)
        return ACK_TIMEOUT;
    if (response != NULL && rxLen > 0)
        memcpy(response, resp, rxLen);
    // 调试输出
    // HAL_UART_Transmit(&huart1, response, rxLen, HAL_MAX_DELAY);
    // 解析响应
    if (resp[0] == 0xEF && resp[1] == 0x01) {
        if (has608->chipAddr == (resp[2] << 24 | resp[3] << 16 | resp[4] << 8 | resp[5])) {
            if (resp[6] == PKG_ACK) {
                return resp[9]; // 返回确认码
            }
        }
    }
    return ACK_ERR; // 错误
}

// ======================================== 指令集 ======================================== //
// 函数名称： 录入图像  PS_GetImage
// 功能说明： 探测手指，探测到后录入指纹图像存于ImageBuffer。返回确认
//           码表示：录入成功、无手指等。
// 输入参数： none
// 返回参数： 确认字
// 指令代码： 01H
uint8_t PS_GetImage(AS608_HandleTypeDef *has608)
{
    return AS608_SendCmd(has608, CMD_GET_IMAGE, NULL, 0, NULL);
}

// 函数名称： 生成特征  PS_GenChar
// 功能说明： 将ImageBuffer中的原始图像生成指纹特
//           征文件存于CharBuffer1 或CharBuffer2
// 输入参数： BufferID(特征缓冲区号)
// 返回参数： 确认字
// 指令代码： 02H
uint8_t PS_GenChar(AS608_HandleTypeDef *has608, uint8_t BufferID)
{
    uint8_t params[1] = {BufferID};
    return AS608_SendCmd(has608, CMD_GEN_CHAR, params, 1, NULL);
}

// 函数名称： 合并特征（生成模板） PS_RegModel
// 功能说明： 将CharBuffer1与CharBuffer2中的特征文件合并生成
//           模板，结果存于CharBuffer1与CharBuffer2。
// 输入参数： none
// 返回参数： 确认字
// 指令代码：05H
uint8_t PS_RegModel(AS608_HandleTypeDef *has608)
{
    return AS608_SendCmd(has608, CMD_REG_MODEL, NULL, 0, NULL);
}

// 函数名称： 储存模板  PS_StoreChar
// 功能说明： 将CharBuffer1 或CharBuffer2中的模板文件存到PageID号
//           flash数据库位置。
// 输入参数： BufferID(缓冲区号)，PageID（指纹库位置号）
// 返回参数： 确认字
// 指令代码： 06H
uint8_t PS_StoreChar(AS608_HandleTypeDef *has608, uint8_t BufferID, uint16_t PageID)
{
    uint8_t params[3] = {BufferID,
                         (PageID >> 8) & 0xFF, PageID & 0xFF};
    return AS608_SendCmd(has608, CMD_STORE_CHAR, params, 3, NULL);
}

// 函数名称： 删除模板 PS_DeletChar
// 功能说明： 删除flash数据库中指定ID号开始的N个指纹模板
// 输入参数： PageID(指纹库模板号)，N删除的模板个数。
// 返回参数： 确认字
// 指令代码： 0cH
uint8_t PS_DeletChar(AS608_HandleTypeDef *has608, uint16_t PageID, uint16_t N)
{
    uint8_t params[4] = {(PageID >> 8) & 0xFF, PageID & 0xFF,
                         (N >> 8) & 0xFF, N & 0xFF};
    return AS608_SendCmd(has608, CMD_DELETE_CHAR, params, 4, NULL);
}

// 函数名称： 清空指纹库  PS_Empty
// 功能说明： 删除flash数据库中所有指纹模板
// 输入参数： none
// 返回参数： 确认字
// 指令代码： 0dH
uint8_t PS_Empty(AS608_HandleTypeDef *has608)
{
    return AS608_SendCmd(has608, CMD_EMPTY, NULL, 0, NULL);
}

// 函数名称： 高速搜索  PS_HighSpeedSearch
// 功能说明：
//      .以CharBuffer1或CharBuffer2中的特征文件高速搜索整个或部分指纹
//      库。若搜索到，则返回页码。
//      .该指令对于的确存在于指纹库中，且登录时质量很好的指纹，会很快给
//      出搜索结果。
// 输入参数： BufferID， StartPage(起始页)，PageNum（页数）
// 返回参数： 确认字，页码（相配指纹模板）
// 指令代码： 1bH
uint8_t PS_HighSpeedSearch(AS608_HandleTypeDef *has608, uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, uint16_t *foundPageID)
{
    uint8_t params[5]    = {BufferID,
                            (StartPage >> 8) & 0xFF, StartPage & 0xFF,
                            (PageNum >> 8) & 0xFF, PageNum & 0xFF};
    uint8_t response[32] = {0};
    uint8_t ack          = AS608_SendCmd(has608, CMD_HIGH_SPEED_SEARCH, params, 5, response);

    if (ack == ACK_OK) {
        *foundPageID = (response[10] << 8) | response[11];
    }
    return ack;
}

// 函数名称： 写系统寄存器 PS_WriteReg
// 功能说明： 写模块寄存器(详细见手册)
// 输入参数： 寄存器序号 4(波特率)/5(安全等级)/6(包大小)， 内容(9600倍数N/1~5/32bytes倍数N(1~4))
// 返回参数： 确认字
// 指令代码： 0eH
uint8_t PS_WriteReg(AS608_HandleTypeDef *has608, uint8_t RegID, uint8_t Value)
{
    uint8_t params[2] = {RegID, Value};
    return AS608_SendCmd(has608, CMD_WRITE_REG, params, 2, NULL);
}

// 函数名称： 读系统基本参数 PS_ReadSysPara
// 功能说明：(详细见手册)
//      .读取模块的基本参数（波特率，包大小等）。
//      .参数表前16个字节存放了模块的基本通讯和配置信息，称为模块的基本参数
// 输入参数： none
// 返回参数： 确认字 + 基本参数（16bytes）
// 指令代码： 0fH
uint8_t PS_ReadSysPara(AS608_HandleTypeDef *has608, uint16_t *para)
{
    uint8_t response[32] = {0};
    uint8_t ack          = AS608_SendCmd(has608, CMD_READ_SYS_PARA, NULL, 0, response);
    if (ack == ACK_OK) {
        memcpy(para, response + 10, 16);
    }
    return ack;
}
