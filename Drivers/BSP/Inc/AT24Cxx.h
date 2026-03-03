#ifndef __AT24CXX_H__
#define __AT24CXX_H__

#include "sys.h"

// AT24C01    128字节, 页大小8字节
// AT24C02    256字节, 页大小8字节
// AT24C04    512字节, 页大小16字节(已选)
// AT24C08    1KB, 页大小16字节
// AT24C16    2KB, 页大小16字节
// AT24C32    4KB, 页大小32字节
// AT24C64    8KB, 页大小32字节
// AT24C128   16KB, 页大小64字节
// AT24C256   32KB, 页大小64字节
// AT24C512   64KB, 页大小128字节

#define AT24Cxx_I2C_W_SCL(x) HAL_GPIO_WritePin(EEPROM_SCL_GPIO_Port, EEPROM_SCL_Pin, (GPIO_PinState)x)
#define AT24Cxx_I2C_W_SDA(x) HAL_GPIO_WritePin(EEPROM_SDA_GPIO_Port, EEPROM_SDA_Pin, (GPIO_PinState)x)
#define AT24Cxx_I2C_R_SDA(x) HAL_GPIO_ReadPin(EEPROM_SDA_GPIO_Port, EEPROM_SDA_Pin)

#define S_I2C                1 // 软件模拟 I2C 接口开关

#define AT24Cxx_ADDRESS_R    0xA1 // 读地址
#define AT24Cxx_ADDRESS_W    0xA0 // 写地址

void AT24Cxx_WriteByte(uint8_t addr, uint8_t data);
uint8_t AT24Cxx_ReadByte(uint8_t addr);
void AT24Cxx_Write(uint8_t addr, uint8_t *data, uint16_t len);
void AT24Cxx_Read(uint8_t addr, uint8_t *data, uint16_t len);

#endif