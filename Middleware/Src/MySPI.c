#include "MySPI.h"

void MySPI_CS_Clr(GPIO_TypeDef *CS_Port, uint16_t CS_Pin)
{
    HAL_GPIO_WritePin(CS_Port, CS_Pin, GPIO_PIN_RESET);
}

void MySPI_CS_Set(GPIO_TypeDef *CS_Port, uint16_t CS_Pin)
{
    HAL_GPIO_WritePin(CS_Port, CS_Pin, GPIO_PIN_SET);
}

uint8_t MySPI_SendByte(SPI_HandleTypeDef *hspi, uint8_t data)
{
    uint8_t readValue = 0;
    HAL_SPI_TransmitReceive(hspi, &data, &readValue, 1, 10);
    return readValue;
}

void MySPI_WriteReg(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin, uint8_t address, uint8_t value)
{
    MySPI_CS_Clr(CS_Port, CS_Pin);
    MySPI_SendByte(hspi, address);
    MySPI_SendByte(hspi, value);
    MySPI_CS_Set(CS_Port, CS_Pin);
}

uint8_t MySPI_ReadReg(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin, uint8_t address)
{
    uint8_t val;
    MySPI_CS_Clr(CS_Port, CS_Pin);
    MySPI_SendByte(hspi, address);
    val = MySPI_SendByte(hspi, 0x00);
    MySPI_CS_Set(CS_Port, CS_Pin);
    return val;
}

void MySPI_Write(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin, uint8_t *data, uint16_t len)
{
    MySPI_CS_Clr(CS_Port, CS_Pin);
    for (uint16_t i = 0; i < len; i++) {
        MySPI_SendByte(hspi, data[i]);
    }
    MySPI_CS_Set(CS_Port, CS_Pin);
}

void MySPI_Read(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin, uint8_t *data, uint16_t len)
{
    MySPI_CS_Clr(CS_Port, CS_Pin);
    for (uint16_t i = 0; i < len; i++) {
        data[i] = MySPI_SendByte(hspi, 0x00);
    }
    MySPI_CS_Set(CS_Port, CS_Pin);
}
