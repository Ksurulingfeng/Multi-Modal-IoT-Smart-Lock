#ifndef MYSPI
#define MYSPI

#include "sys.h"

void MySPI_CS_Clr(GPIO_TypeDef *CS_Port, uint16_t CS_Pin);
void MySPI_CS_Set(GPIO_TypeDef *CS_Port, uint16_t CS_Pin);
uint8_t MySPI_SendByte(SPI_HandleTypeDef *hspi, uint8_t data);
void MySPI_WriteReg(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin, uint8_t address, uint8_t value);
uint8_t MySPI_ReadReg(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin, uint8_t address);
void MySPI_Write(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin, uint8_t *data, uint16_t len);
void MySPI_Read(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CS_Port, uint16_t CS_Pin, uint8_t *data, uint16_t len);

#endif /* MYSPI */
