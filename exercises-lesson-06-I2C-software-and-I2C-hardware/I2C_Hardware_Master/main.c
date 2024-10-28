#include "stm32f10x.h"      // Device header
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_i2c.h"  // Keil::Device:StdPeriph Drivers:I2C
#include "stm32f10x_rcc.h"  // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"  // Keil::Device:StdPeriph Drivers:TIM

#define I2C_SCL GPIO_Pin_6
#define I2C_SDA GPIO_Pin_7

#define I2C1_GPIO GPIOB

// Delay function using Timer2
void Delay_Us(uint32_t Time)
{
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < Time);
}

// Configure TIM2 to use as a microsecond delay timer
void TIM2_Config()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; // Prescale to 1 MHz (1 µs period)
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;    // Maximum period for counting
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    TIM_Cmd(TIM2, ENABLE);
}

void RCC_Config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
}

void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = I2C_SCL | I2C_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C1_GPIO, &GPIO_InitStructure);
}

void I2C_Config()
{
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

void I2C_Send_Address(uint8_t Address, uint8_t Direction)
{

    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Shift address left by 1 and set LSB according to Direction
    I2C_Send7bitAddress(I2C1, (Address << 1) | Direction, Direction);

    // Wait for either transmitter or receiver mode to be selected
    while (!I2C_CheckEvent(I2C1, (Direction == I2C_Direction_Transmitter) ? 
								I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED : 
								I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
}

void I2C_Send_Data(uint8_t Data)
{
    I2C_SendData(I2C1, Data);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

// Read a byte of data over I2C with specified ACK/NACK
uint8_t I2C_Read_Data(uint8_t ack)
{
    if (ack == 0)
    {
        I2C_AcknowledgeConfig(I2C1, DISABLE); // Send NACK after receiving data
    }
    else
    {
        I2C_AcknowledgeConfig(I2C1, ENABLE); // Send ACK after receiving data
    }

    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	
    return I2C_ReceiveData(I2C1);
}

// Write data to a specific memory address in At24c32
void At24c32_Write(uint8_t at24c32Addr, uint16_t memAddr, uint8_t dataValue)
{	
	// START
	I2C_GenerateSTART(I2C1, ENABLE);
	
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	// Shift address left by 1 to write
	I2C_Send7bitAddress(I2C1, at24c32Addr << 1 , I2C_Direction_Transmitter);
	
	// ACK
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	// Send high byte of memory address
	I2C_SendData(I2C1, (memAddr >> 8) & 0xFF);
	
	// ACK
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	//Send low byte of memory address
	I2C_SendData(I2C1, memAddr & 0xFF);
	
	// ACK
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	// Write data into EEPROM
	I2C_SendData(I2C1, dataValue);
	
	// ACK
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	// STOP
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	// Add a delay to ensure it is completed
	Delay_Us(5000);
}

// Random read mode
uint8_t At24c32_Random_Read(uint8_t at24c32Addr, uint16_t memAddr)
{
    uint8_t receivedData;

	// Start
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	// Send slave address with write intent
    I2C_Send7bitAddress(I2C1, at24c32Addr << 1, I2C_Direction_Transmitter);
	
	// ACK
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Send high byte of memory address
    I2C_SendData(I2C1, (memAddr >> 8) & 0xFF);
	
	// ACK
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Send low byte of memory address
    I2C_SendData(I2C1, memAddr & 0xFF);
	
	// ACL
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Start
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	// Send slave address with read intent
    I2C_Send7bitAddress(I2C1, at24c32Addr << 1 | 0x01, I2C_Direction_Receiver);
	
	// ACK
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    // Read data from the specified address with NACK 
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	
	// Read data
    receivedData = I2C_ReceiveData(I2C1);

    // STOP
    I2C_GenerateSTOP(I2C1, ENABLE);

    // Re-enable ACK for future communications
    I2C_AcknowledgeConfig(I2C1, ENABLE);
	
    return receivedData;
}

uint8_t At24c32_Address = 0x57;          // 7-bit I2C address of EEPROM
uint16_t At24c32_MemoryAddress = 0x0010; // Memory address in EEPROM
uint8_t At24c32_DataToWrite = 0xA5;      // Data to write to EEPROM
uint8_t At24c32_DataRead;                // Variable to store read data
int main()
{
    RCC_Config();
    TIM2_Config();
    GPIO_Config();
    I2C_Config();

    while (1)
    {
        // Write data to At24c32 at specified memory address
        At24c32_Write(At24c32_Address, At24c32_MemoryAddress, At24c32_DataToWrite);

        // Read data back from the same At24c32 memory address
        At24c32_DataRead = At24c32_Random_Read(At24c32_Address, At24c32_MemoryAddress);

        Delay_Us(1000);
    }
}
