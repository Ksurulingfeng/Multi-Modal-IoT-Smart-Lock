#include "RC522.h"
#include "MySPI.h"

// RC522初始化
void MFRC522_Init(RC522_HandleTypeDef *hrc522, SPI_HandleTypeDef *hspi,
                  GPIO_TypeDef *CS_Port, uint16_t CS_Pin)
{
    hrc522->hspi    = hspi;
    hrc522->CS_Port = CS_Port;
    hrc522->CS_Pin  = CS_Pin;
    hrc522->timeout = 1000;

    MFRC522_Reset(hrc522);
    MFRC522_WriteRegister(hrc522, MFRC522_REG_T_MODE, 0x8D);
    MFRC522_WriteRegister(hrc522, MFRC522_REG_T_PRESCALER, 0x3E);
    MFRC522_WriteRegister(hrc522, MFRC522_REG_T_RELOAD_L, 30);
    MFRC522_WriteRegister(hrc522, MFRC522_REG_T_RELOAD_H, 0);
    MFRC522_WriteRegister(hrc522, MFRC522_REG_RF_CFG, 0x7F);       // 48dB gain
    MFRC522_WriteRegister(hrc522, MFRC522_REG_RX_THRESHOLD, 0x84); ////////////////////////////<<<<<<<<<<<<<<<<<<<<
    MFRC522_WriteRegister(hrc522, MFRC522_REG_TX_AUTO, 0x40);
    MFRC522_WriteRegister(hrc522, MFRC522_REG_MODE, 0x3D);
    MFRC522_AntennaOn(hrc522); // 打开天线
}

// 写寄存器
void MFRC522_WriteRegister(RC522_HandleTypeDef *hrc522, uint8_t addr, uint8_t data)
{
    addr = (addr << 1) & 0x7E; // Address format: 0XXXXXX0
    MySPI_WriteReg(hrc522->hspi, hrc522->CS_Port, hrc522->CS_Pin, addr, data);
}

// 读寄存器
uint8_t MFRC522_ReadRegister(RC522_HandleTypeDef *hrc522, uint8_t addr)
{
    addr = ((addr << 1) & 0x7E) | 0x80; // Address format: 1XXXXXX0
    return MySPI_ReadReg(hrc522->hspi, hrc522->CS_Port, hrc522->CS_Pin, addr);
}

// 按位设置
void MFRC522_SetBitMask(RC522_HandleTypeDef *hrc522, uint8_t reg, uint8_t mask)
{
    MFRC522_WriteRegister(hrc522, reg, MFRC522_ReadRegister(hrc522, reg) | mask);
}

// 按位清除
void MFRC522_ClearBitMask(RC522_HandleTypeDef *hrc522, uint8_t reg, uint8_t mask)
{
    MFRC522_WriteRegister(hrc522, reg, MFRC522_ReadRegister(hrc522, reg) & (~mask));
}

// 检测是否检测到卡片
uint8_t MFRC522_GetCardState(RC522_HandleTypeDef *hrc522)
{
    uint8_t str[4] = {0};
    return MFRC522_Request(hrc522, PICC_REQALL, str) == MI_OK;
}

// 检测RFID标签
uint8_t MFRC522_CheckCard(RC522_HandleTypeDef *hrc522, uint8_t *id)
{
    uint8_t status;
    status = MFRC522_Request(hrc522, PICC_REQIDL, id);          // 查找卡片，返回卡片类型
    if (status == MI_OK) status = MFRC522_Anticoll(hrc522, id); // 卡检测。防碰撞，返回卡序号4字节
    MFRC522_Halt(hrc522);                                       // 命令卡进入休眠状态
    return status;
}

// 比较卡号
uint8_t MFRC522_CompareCardID(RC522_HandleTypeDef *hrc522, uint8_t *CardID, uint8_t *CompareID)
{
    uint8_t i;
    for (i = 0; i < 5; i++) {
        if (CardID[i] != CompareID[i]) return MI_ERR;
    }
    return MI_OK;
}

// 寻卡
uint8_t MFRC522_ToCard(RC522_HandleTypeDef *hrc522, uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backLen)
{
    uint8_t status  = MI_ERR;
    uint8_t irqEn   = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;

    switch (command) {
        case PCD_AUTHENT: {
            irqEn   = 0x12;
            waitIRq = 0x10;
            break;
        }
        case PCD_TRANSCEIVE: {
            irqEn   = 0x77;
            waitIRq = 0x30;
            break;
        }
        default:
            break;
    }

    MFRC522_WriteRegister(hrc522, MFRC522_REG_COMM_IE_N, irqEn | 0x80);
    MFRC522_ClearBitMask(hrc522, MFRC522_REG_COMM_IRQ, 0x80);
    MFRC522_SetBitMask(hrc522, MFRC522_REG_FIFO_LEVEL, 0x80);
    MFRC522_WriteRegister(hrc522, MFRC522_REG_COMMAND, PCD_IDLE);

    // 将数据写入FIFO
    for (i = 0; i < sendLen; i++) MFRC522_WriteRegister(hrc522, MFRC522_REG_FIFO_DATA, sendData[i]);

    // 执行命令
    MFRC522_WriteRegister(hrc522, MFRC522_REG_COMMAND, command);
    if (command == PCD_TRANSCEIVE) MFRC522_SetBitMask(hrc522, MFRC522_REG_BIT_FRAMING, 0x80); // StartSend=1，数据开始传输

    // 等待接收数据完成
    i = 2000; // i根据时钟频率调整，操作员M1卡的最大等待时间为25ms
    do {
        // CommIrqReg[7..0]
        // 设置中断 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = MFRC522_ReadRegister(hrc522, MFRC522_REG_COMM_IRQ);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

    MFRC522_ClearBitMask(hrc522, MFRC522_REG_BIT_FRAMING, 0x80); // StartSend=0

    if (i != 0) {
        if (!(MFRC522_ReadRegister(hrc522, MFRC522_REG_ERROR) & 0x1B)) {
            status = MI_OK;
            if (n & irqEn & 0x01) status = MI_NOTAGERR;
            if (command == PCD_TRANSCEIVE) {
                n        = MFRC522_ReadRegister(hrc522, MFRC522_REG_FIFO_LEVEL);
                lastBits = MFRC522_ReadRegister(hrc522, MFRC522_REG_CONTROL) & 0x07;
                if (lastBits)
                    *backLen = (n - 1) * 8 + lastBits;
                else
                    *backLen = n * 8;
                if (n == 0) n = 1;
                if (n > MFRC522_MAX_LEN) n = MFRC522_MAX_LEN;
                for (i = 0; i < n; i++) backData[i] = MFRC522_ReadRegister(hrc522, MFRC522_REG_FIFO_DATA); // FIFO读取接收到的数据
            }
        } else
            status = MI_ERR;
    }
    return status;
}

// 寻卡请求
uint8_t MFRC522_Request(RC522_HandleTypeDef *hrc522, uint8_t reqMode, uint8_t *TagType)
{
    uint8_t status;
    uint16_t backBits;

    MFRC522_WriteRegister(hrc522, MFRC522_REG_BIT_FRAMING, 0x07);
    TagType[0] = reqMode;
    status     = MFRC522_ToCard(hrc522, PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
    if ((status != MI_OK) || (backBits != 0x10)) status = MI_ERR;
    return status;
}

// 防碰撞
uint8_t MFRC522_Anticoll(RC522_HandleTypeDef *hrc522, uint8_t *serNum)
{
    uint8_t status;
    uint8_t i;
    uint8_t serNumCheck = 0;
    uint16_t unLen;

    MFRC522_WriteRegister(hrc522, MFRC522_REG_BIT_FRAMING, 0x00); // TxLastBists = BitFramingReg[2..0]
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status    = MFRC522_ToCard(hrc522, PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
    if (status == MI_OK) {
        // 检查卡序号
        for (i = 0; i < 4; i++) serNumCheck ^= serNum[i];
        if (serNumCheck != serNum[i]) status = MI_ERR;
    }
    return status;
}

// 校验CRC
void MFRC522_CalculateCRC(RC522_HandleTypeDef *hrc522, uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
    uint8_t i, n;

    MFRC522_ClearBitMask(hrc522, MFRC522_REG_DIV_IRQ, 0x04);  // CRCIrq = 0
    MFRC522_SetBitMask(hrc522, MFRC522_REG_FIFO_LEVEL, 0x80); // 清除FIFO指针
    // Write_MFRC522(hrc522, CommandReg, PCD_IDLE);

    // 将数据写入FIFO
    for (i = 0; i < len; i++) MFRC522_WriteRegister(hrc522, MFRC522_REG_FIFO_DATA, *(pIndata + i));
    MFRC522_WriteRegister(hrc522, MFRC522_REG_COMMAND, PCD_CALCCRC);

    // 等待CRC计算完成
    i = 0xFF;
    do {
        n = MFRC522_ReadRegister(hrc522, MFRC522_REG_DIV_IRQ);
        i--;
    } while ((i != 0) && !(n & 0x04)); // CRCIrq = 1

    // 读取CRC计算结果
    pOutData[0] = MFRC522_ReadRegister(hrc522, MFRC522_REG_CRC_RESULT_L);
    pOutData[1] = MFRC522_ReadRegister(hrc522, MFRC522_REG_CRC_RESULT_M);
}

// 选择标签
uint8_t MFRC522_SelectTag(RC522_HandleTypeDef *hrc522, uint8_t *serNum)
{
    uint8_t i;
    uint8_t status;
    uint8_t size;
    uint16_t recvBits;
    uint8_t buffer[9];

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i = 0; i < 5; i++) buffer[i + 2] = *(serNum + i);
    MFRC522_CalculateCRC(hrc522, buffer, 7, &buffer[7]);
    status = MFRC522_ToCard(hrc522, PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    if ((status == MI_OK) && (recvBits == 0x18))
        size = buffer[0];
    else
        size = 0;
    return size;
}

// 验证块
uint8_t MFRC522_Auth(RC522_HandleTypeDef *hrc522, uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum)
{
    uint8_t status;
    uint16_t recvBits;
    uint8_t i;
    uint8_t buff[12];

    // 验证命令块 = 模式+地址+扇区+密码+卡序列号
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i = 0; i < 6; i++) buff[i + 2] = *(Sectorkey + i);
    for (i = 0; i < 4; i++) buff[i + 8] = *(serNum + i);
    status = MFRC522_ToCard(hrc522, PCD_AUTHENT, buff, 12, buff, &recvBits);
    if ((status != MI_OK) || (!(MFRC522_ReadRegister(hrc522, MFRC522_REG_STATUS2) & 0x08))) status = MI_ERR;
    return status;
}

// 读取块数据
uint8_t MFRC522_Read(RC522_HandleTypeDef *hrc522, uint8_t blockAddr, uint8_t *recvData)
{
    uint8_t status;
    uint16_t unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    MFRC522_CalculateCRC(hrc522, recvData, 2, &recvData[2]);
    status = MFRC522_ToCard(hrc522, PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
    if ((status != MI_OK) || (unLen != 0x90)) status = MI_ERR;
    return status;
}

// 写入块数据
uint8_t MFRC522_Write(RC522_HandleTypeDef *hrc522, uint8_t blockAddr, uint8_t *writeData)
{
    uint8_t status;
    uint16_t recvBits;
    uint8_t i;
    uint8_t buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    MFRC522_CalculateCRC(hrc522, buff, 2, &buff[2]);
    status = MFRC522_ToCard(hrc522, PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) status = MI_ERR;
    if (status == MI_OK) {
        // Data to the FIFO write 16Byte
        for (i = 0; i < 16; i++) buff[i] = *(writeData + i);
        MFRC522_CalculateCRC(hrc522, buff, 16, &buff[16]);
        status = MFRC522_ToCard(hrc522, PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
        if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) status = MI_ERR;
    }
    return status;
}

// RC522复位
void MFRC522_Reset(RC522_HandleTypeDef *hrc522)
{
    MFRC522_WriteRegister(hrc522, MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

// 开启天线
void MFRC522_AntennaOn(RC522_HandleTypeDef *hrc522)
{
    uint8_t temp;

    temp = MFRC522_ReadRegister(hrc522, MFRC522_REG_TX_CONTROL);
    if (!(temp & 0x03)) MFRC522_SetBitMask(hrc522, MFRC522_REG_TX_CONTROL, 0x03);
}

// 关闭天线
void MFRC522_AntennaOff(RC522_HandleTypeDef *hrc522)
{
    MFRC522_ClearBitMask(hrc522, MFRC522_REG_TX_CONTROL, 0x03);
}

// 进入休眠状态
void MFRC522_Halt(RC522_HandleTypeDef *hrc522)
{
    uint16_t unLen;
    uint8_t buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    MFRC522_CalculateCRC(hrc522, buff, 2, &buff[2]);
    MFRC522_ToCard(hrc522, PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}
