#include "stm32f10x.h"      // Device header
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_i2c.h"  // Keil::Device:StdPeriph Drivers:I2C
#include "stm32f10x_rcc.h"  // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"  // Keil::Device:StdPeriph Drivers:TIM

#include "Delay.h"
#include "DS3231_AT24C32.h"

void GPIO_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/*
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = I2C_SCL | I2C_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C1_GPIO, &GPIO_InitStructure);
	*/
	
	 GPIO_InitTypeDef GPIO_InitStruct;
    
    // 1. Chuy?n SCL sang ch? d? GPIO Output Open-Drain
    GPIO_InitStruct.GPIO_Pin = I2C_SCL | I2C_SDA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 2. T?o 8 xung clock trên SCL d? gi?i phóng SDA
    for (int i = 0; i < 8; i++) {
        // Ðua SCL lên cao
        GPIO_SetBits(GPIOB, I2C_SCL);
        for (volatile int j = 0; j < 1000; j++);  // Ð? tr? ng?n
        
        // Ðua SCL xu?ng th?p
        GPIO_ResetBits(GPIOB, I2C_SCL);
        for (volatile int j = 0; j < 1000; j++);  // Ð? tr? ng?n
    }
    
    // 3. Ðua SCL và SDA tr? l?i m?c cao
    GPIO_SetBits(GPIOB, I2C_SCL | I2C_SDA);

    // 4. Khôi ph?c các chân v? ch? d? I2C (Alternate Function Open-Drain)
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Pin = I2C_SCL | I2C_SDA;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void I2C_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
    I2C_InitTypeDef I2C_InitStructure;

    I2C_InitStructure.I2C_ClockSpeed = 400000; // Fast mode
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00; // Address Master
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
}

uint8_t At24c32_Address = 0x57;          // 7-bit I2C address of EEPROM
uint16_t At24c32_MemoryAddress = 0x0010; // Memory address in EEPROM
uint8_t At24c32_DataToWrite = 0xC0;      // Data to write to EEPROM
uint8_t At24c32_DataRead;                // Variable to store read data

int main()
{
    TIM2_Config();
    GPIO_Config();
	I2C_Config();
	
	/*
	uint8_t hours = 1;
    uint8_t minutes = 51;
    uint8_t seconds = 00;

    // Set up time for DS3231 if needed
    DS3231_SetTime(hours, minutes, seconds);
	*/
	
	uint8_t hours = 0, minutes = 0, seconds = 0;

    while (1)
    {
		DS3231_GetTime(&hours, &minutes, &seconds);
		
        // Write data to At24c32 at specified memory address
        // At24c32_Write(At24c32_Address, At24c32_MemoryAddress, At24c32_DataToWrite);

        // Read data back from the same At24c32 memory address
        // At24c32_DataRead = At24c32_Random_Read(At24c32_Address, At24c32_MemoryAddress);
		
    }
}
