#include "stm32f10x.h"                  // Device header
#include "DS3231_AT24C32.h"
#include "Delay.h"

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
	Delay_Us(10000);
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
	
	Delay_Us(10000);
	
    return receivedData;
}

#define DS3231_ADDRESS 0x68 // 7-bit I2C address for DS3231

// Function to convert decimal to BCD (Binary-Coded Decimal) format
uint8_t DecimalToBCD(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

// Function to set the time (hours, minutes, and seconds) on the DS3231 RTC
void DS3231_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    // Convert time values to BCD format
    uint8_t bcd_seconds = DecimalToBCD(seconds);
    uint8_t bcd_minutes = DecimalToBCD(minutes);
    uint8_t bcd_hours = DecimalToBCD(hours);

    // Send START condition on the I2C bus
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send DS3231 address with write intent
    I2C_Send7bitAddress(I2C1, DS3231_ADDRESS << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Send register address 0x00 (seconds register) to start from seconds register
    I2C_SendData(I2C1, 0x00); // Register 0x00 for seconds
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write seconds value to the seconds register
    I2C_SendData(I2C1, bcd_seconds);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write minutes value to the minutes register
    I2C_SendData(I2C1, bcd_minutes);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write hours value to the hours register
    I2C_SendData(I2C1, bcd_hours);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Send STOP condition to end the I2C communication
    I2C_GenerateSTOP(I2C1, ENABLE);
}

#define TIMEOUT 10000  // Set a suitable timeout value

int I2C_WaitForEvent(I2C_TypeDef* I2Cx, uint32_t event) {
    uint32_t timeout = TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, event)) {
        if ((timeout--) == 0) return 1;  // Return 1 if timeout occurs
    }
    return 0;  // Return 0 if the event occurs successfully
}

// Function to convert from BCD (Binary-Coded Decimal) to decimal
uint8_t BCDToDecimal(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

// Function to read hours, minutes, and seconds from DS3231
void DS3231_GetTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds) {
	
    // Send START condition
    I2C_GenerateSTART(I2C1, ENABLE);
     while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
    // Send DS3231 address with write intent
    I2C_Send7bitAddress(I2C1, DS3231_ADDRESS << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Send register address to start reading from (0x00 for seconds register)
    I2C_SendData(I2C1, 0x00);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Send repeated START condition to switch to read mode
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send DS3231 address with read intent
    I2C_Send7bitAddress(I2C1, DS3231_ADDRESS << 1 | 0x01, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    // Read seconds from register 0x00
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    *seconds = I2C_ReceiveData(I2C1);

    // Read minutes from register 0x01
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    *minutes = I2C_ReceiveData(I2C1);

    // Read hours from register 0x02
    I2C_AcknowledgeConfig(I2C1, DISABLE); // Send NACK after reading the last byte
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    *hours = I2C_ReceiveData(I2C1);

    // Send STOP condition to end the read operation
    I2C_GenerateSTOP(I2C1, ENABLE);

    // Re-enable ACK for future communications
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    // Convert BCD to decimal format
    *seconds = BCDToDecimal(*seconds);
    *minutes = BCDToDecimal(*minutes);
    *hours = BCDToDecimal(*hours);
}
