#ifndef DS3231_AT24C32_H
#define DS3231_AT24C32_H

#include "stm32f10x.h"                  // Device header

#define I2C_SCL GPIO_Pin_6
#define I2C_SDA GPIO_Pin_7

#define I2C1_GPIO GPIOB

void At24c32_Write(uint8_t at24c32Addr, uint16_t memAddr, uint8_t dataValue);
uint8_t At24c32_Random_Read(uint8_t at24c32Addr, uint16_t memAddr);

void DS3231_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void DS3231_GetTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds);
	
#endif // DS3231_AT24C32_H
