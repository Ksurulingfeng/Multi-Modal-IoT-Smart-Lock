#include "AT24Cxx.h"

/**********************************************
//Software I2C Start
**********************************************/
static void AT24Cxx_I2C_Start(void)
{
    AT24Cxx_I2C_W_SDA(1);
    AT24Cxx_I2C_W_SCL(1);
    AT24Cxx_I2C_W_SDA(0);
    AT24Cxx_I2C_W_SCL(0);
}

/**********************************************
//Software I2C Stop
**********************************************/
static void AT24Cxx_I2C_Stop(void)
{
    AT24Cxx_I2C_W_SDA(0);
    AT24Cxx_I2C_W_SCL(1);
    AT24Cxx_I2C_W_SDA(1);
}

/**********************************************
//Software I2C SendByte
**********************************************/
static void AT24Cxx_I2C_SendByte(uint8_t byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        AT24Cxx_I2C_W_SDA(byte & (0x80 >> i));
        AT24Cxx_I2C_W_SCL(1);
        AT24Cxx_I2C_W_SCL(0);
    }
}

/**********************************************
//Software I2C ReadByte
**********************************************/
static uint8_t AT24Cxx_I2C_ReadByte(void)
{
    uint8_t i, byte = 0;
    AT24Cxx_I2C_W_SDA(1);
    for (i = 0; i < 8; i++) {
        AT24Cxx_I2C_W_SCL(1);
        if (AT24Cxx_I2C_R_SDA())
            byte |= (0x80 >> i);
        AT24Cxx_I2C_W_SCL(0);
    }
    return byte;
}

/**********************************************
//Software I2C SendAck
**********************************************/
static void AT24Cxx_I2C_SendAck(uint8_t ack)
{
    AT24Cxx_I2C_W_SDA(ack);
    AT24Cxx_I2C_W_SCL(1);
    AT24Cxx_I2C_W_SCL(0);
}

/**********************************************
//Software I2C ReceiveAck
**********************************************/
static uint8_t AT24Cxx_I2C_ReceiveAck(void)
{
    uint8_t ack;
    AT24Cxx_I2C_W_SDA(1);
    AT24Cxx_I2C_W_SCL(1);
    ack = AT24Cxx_I2C_R_SDA();
    AT24Cxx_I2C_W_SCL(0);
    return ack;
}

void AT24Cxx_WriteByte(uint8_t addr, uint8_t data)
{
#if S_I2C
    AT24Cxx_I2C_Start();
    AT24Cxx_I2C_SendByte(AT24Cxx_ADDRESS_W);
    AT24Cxx_I2C_ReceiveAck();
    AT24Cxx_I2C_SendByte(addr);
    AT24Cxx_I2C_ReceiveAck();
    AT24Cxx_I2C_SendByte(data);
    AT24Cxx_I2C_ReceiveAck();
    AT24Cxx_I2C_Stop();
    HAL_Delay(5);
#else
    HAL_I2C_Mem_Write();
#endif
}

uint8_t AT24Cxx_ReadByte(uint8_t addr)
{
    uint8_t data;
#if S_I2C
    AT24Cxx_I2C_Start();
    AT24Cxx_I2C_SendByte(AT24Cxx_ADDRESS_W);
    AT24Cxx_I2C_ReceiveAck();
    AT24Cxx_I2C_SendByte(addr);
    AT24Cxx_I2C_ReceiveAck();
    AT24Cxx_I2C_Start();
    AT24Cxx_I2C_SendByte(AT24Cxx_ADDRESS_R);
    AT24Cxx_I2C_ReceiveAck();
    data = AT24Cxx_I2C_ReadByte();
    AT24Cxx_I2C_SendAck(1);
    AT24Cxx_I2C_Stop();
#else
    HAL_I2C_Mem_Read();
#endif
    return data;
}

// 写多个字节，注意写长度不能超过芯片页大小
void AT24Cxx_Write(uint8_t addr, uint8_t *data, uint16_t len)
{
#if S_I2C
    uint16_t i;
    AT24Cxx_I2C_Start();
    AT24Cxx_I2C_SendByte(AT24Cxx_ADDRESS_W);
    AT24Cxx_I2C_ReceiveAck();
    AT24Cxx_I2C_SendByte(addr);
    AT24Cxx_I2C_ReceiveAck();
    for (i = 0; i < len; i++) {
        AT24Cxx_I2C_SendByte(data[i]);
        AT24Cxx_I2C_ReceiveAck();
    }
    AT24Cxx_I2C_Stop();
    HAL_Delay(5);
#else
    HAL_I2C_Mem_Write();
#endif
}

void AT24Cxx_Read(uint8_t addr, uint8_t *data, uint16_t len)
{
#if S_I2C
    uint16_t i;
    for (i = 0; i < len; i++) {
        AT24Cxx_I2C_Start();
        AT24Cxx_I2C_SendByte(AT24Cxx_ADDRESS_W);
        AT24Cxx_I2C_ReceiveAck();
        AT24Cxx_I2C_SendByte(addr + i);
        AT24Cxx_I2C_ReceiveAck();
        AT24Cxx_I2C_Start();
        AT24Cxx_I2C_SendByte(AT24Cxx_ADDRESS_R);
        AT24Cxx_I2C_ReceiveAck();
        data[i] = AT24Cxx_I2C_ReadByte();
        AT24Cxx_I2C_SendAck(1);
        AT24Cxx_I2C_Stop();
    }
#else
    HAL_I2C_Mem_Read();
#endif
}
