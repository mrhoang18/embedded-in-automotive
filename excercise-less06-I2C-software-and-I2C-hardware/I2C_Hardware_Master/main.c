#include "stm32f10x.h"      // Device header
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_i2c.h"  // Keil::Device:StdPeriph Drivers:I2C
#include "stm32f10x_rcc.h"  // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"  // Keil::Device:StdPeriph Drivers:TIM

#include <stdlib.h>
#include "Delay.h"
#include "DS3231_AT24C32.h"

void GPIO_Config(void)
{
    // Enable the clock for GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    
    // 1. Configure SCL and SDA as GPIO Output Push-Pull
    GPIO_InitStruct.GPIO_Pin = I2C_SCL | I2C_SDA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 2. Generate 8 clock pulses on SCL to release SDA
    for (volatile int i = 0; i < 8; i++) {
        // Set SCL high
        GPIO_SetBits(GPIOB, I2C_SCL);
        Delay_Us(1000);
        
        // Set SCL low
        GPIO_ResetBits(GPIOB, I2C_SCL);
        Delay_Us(1000);
    }
    
    // 3. Set both SCL and SDA to high level
    GPIO_SetBits(GPIOB, I2C_SCL | I2C_SDA);
	Delay_Us(1000);
	
    // 4. Reconfigure pins for I2C mode (Alternate Function Open-Drain)
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Pin = I2C_SCL | I2C_SDA;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void I2C_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
    I2C_InitTypeDef I2C_InitStructure;

	I2C_DeInit(I2C1);
	I2C_Cmd(I2C1, DISABLE);
	
    I2C_InitStructure.I2C_ClockSpeed = 100000; // Fast mode
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00; // Address Master
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
}

uint8_t At24c32_Address = 0x57;          // 7-bit I2C address of EEPROM
uint16_t At24c32_MemoryAddress = 0x0000; // Memory address in EEPROM
uint8_t At24c32_DataToWrite = 0xC0;      // Data to write to EEPROM
uint8_t At24c32_DataRead;                // Variable to store read data

int main()
{
    TIM2_Config();
    GPIO_Config();
	I2C_Config();
	
	// Set up time if needed ( hours, minutes, seconds, 0-24h mode)/1-12h mode)
	// DS3231_SetTime(17, 23, 00, 0, 0); //
	
	// Set up date if needed (Sunday, November 24, 2024)
	//  DS3231_SetDate(DAY_SUNDAY, 24, MONTH_NOVEMBER, 24);

	
	uint8_t get_hours = 0, get_minutes = 0, get_seconds = 0;
	uint8_t get_day = 0, get_date = 0, get_month = 0, get_year = 0;
	//	uint8_t Array_DataToWrite[3] = {0x01, 0x02, 0x03};
	//	uint8_t Array_DataToRead[3] = {0x00, 0x00, 0x00};
	
    while (1)
    {
		DS3231_GetTime(&get_hours, &get_minutes, &get_seconds);
		DS3231_GetDate(&get_day, &get_date, &get_month, &get_year);
		
        // Write data to At24c32 at specified memory address
        // At24c32_Write(At24c32_Address, At24c32_MemoryAddress, At24c32_DataToWrite);

//	for (uint8_t i = 0; i < 3; i++)
//	{
//		At24c32_Write(At24c32_Address, At24c32_MemoryAddress + i, Array_DataToWrite[i]); 
//	}
//	
//	for (uint8_t i = 0; i < 3; i++)
//	{
//		Array_DataToRead[i] = At24c32_Random_Read(At24c32_Address, At24c32_MemoryAddress + i); 
//		// Array_DataToRead
//		Delay_Ms(1000);
//	}
	
    }
}
