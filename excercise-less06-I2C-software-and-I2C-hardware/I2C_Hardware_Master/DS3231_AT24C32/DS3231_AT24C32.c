#include "stm32f10x.h"                  // Device header
#include "DS3231_AT24C32.h"
#include "Delay.h"

/**
 * @brief Writes a byte of data to a specific memory address in the AT24C32 EEPROM.
 * @param at24c32Addr: I2C address of the AT24C32 EEPROM.
 * @param memAddr: 16-bit memory address to write the data to.
 * @param dataValue: The byte value to write to the specified memory location.
 * 
 * This function writes a single byte of data to the specified memory address
 * and includes a delay to ensure the write operation is completed.
 */
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


/**
 * @brief Reads a byte of data from a specific memory address in the AT24C32 EEPROM using random read mode.
 * @param at24c32Addr: I2C address of the AT24C32 EEPROM.
 * @param memAddr: 16-bit memory address to read the data from.
 * @retval The byte value read from the specified memory location.
 * 
 * This function performs a random read operation by first writing the target 
 * memory address and then switching to read mode to retrieve the data. A delay 
 * is added to ensure stable communication.
 */
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

/**
 * @brief Converts a decimal number to Binary-Coded Decimal (BCD) format.
 * @param decimal: The decimal value to be converted (0-99).
 * @retval The BCD representation of the input decimal value.
 */
static uint8_t DecimalToBCD(uint8_t decimal) 
{
    return ((decimal / 10) << 4) | (decimal % 10);
}

/**
 * @brief Converts a Binary-Coded Decimal (BCD) number to decimal format.
 * @param bcd: The BCD value to be converted.
 * @retval The decimal representation of the input BCD value.
 */
static uint8_t BCDToDecimal(uint8_t bcd) 
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/**
 * @brief Set the time on DS3231 with 12-hour or 24-hour mode.
 * @param hours: Hour value (1-12 for 12-hour mode, 0-23 for 24-hour mode).
 * @param minutes: Minute value (0-59).
 * @param seconds: Second value (0-59).
 * @param is_12_hour_mode: 
 *        - 0: 24-hour mode
 *        - 1: 12-hour mode
 * @param is_pm: 
 *        - 0: AM (Only applicable in 12-hour mode)
 *        - 1: PM (Only applicable in 12-hour mode)
 */
void DS3231_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t is_12_hour_mode, uint8_t is_pm)
{
    uint8_t bcd_seconds, bcd_minutes, bcd_hours;

    // Convert time values to BCD
    bcd_seconds = DecimalToBCD(seconds);
    bcd_minutes = DecimalToBCD(minutes);

    // Set hours in 12-hour or 24-hour format
    if (is_12_hour_mode)
    {
        // Ensure hours are in valid range for 12-hour mode
        if (hours < 1 || hours > 12)
            return; // Invalid hour value for 12-hour mode

        bcd_hours = DecimalToBCD(hours) | (1 << 6); // Set bit 6 for 12-hour mode
        if (is_pm)
        {
            bcd_hours |= (1 << 5); // Set bit 5 for PM
        }
    }
    else
    {
        // Ensure hours are in valid range for 24-hour mode
        if (hours > 23)
            return; // Invalid hour value for 24-hour mode

        bcd_hours = DecimalToBCD(hours) & ~(1 << 6); // Clear bit 6 for 24-hour mode
    }

    // Start I2C communication and write time values to DS3231
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, DS3231_ADDRESS << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Write to Seconds register (0x00)
    I2C_SendData(I2C1, DS3231_REG_SECONDS);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write seconds
    I2C_SendData(I2C1, bcd_seconds);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write minutes
    I2C_SendData(I2C1, bcd_minutes);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write hours
    I2C_SendData(I2C1, bcd_hours);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE); // Stop I2C communication
}

/**
 * @brief Validates the input day, date, month, and year.
 *        Ensures the date is within valid ranges and accounts for leap years.
 * @param day: Day of the week (1 = Sunday, 7 = Saturday).
 * @param date: Date of the month (1-31).
 * @param month: Month of the year (1-12).
 * @param year: Year (0-99, representing 2000-2099).
 * @retval 1 if the date is valid, 0 otherwise.
 */
uint8_t IsDateValid(uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
    // Check day of the week (1 = Sunday, 7 = Saturday)
    if (day < 1 || day > 7)
        return 0;

    // Check month (1-12)
    if (month < 1 || month > 12)
        return 0;

    // Check date (1-31, based on the month)
    if (date < 1 || date > 31)
        return 0;

    // Handle months with 30 days
    if ((month == 4 || month == 6 || month == 9 || month == 11) && date > 30)
        return 0;

    // Handle February
    if (month == 2)
    {
        // Check for leap year
        uint8_t isLeapYear = (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
        if (date > (isLeapYear ? 29 : 28))
            return 0;
    }

    return 1; // Valid date
}

/**
 * @brief Sets the day, date, month, and year on the DS3231 RTC module.
 *        Converts inputs to BCD format and writes to DS3231 registers.
 * @param day: Day of the week (1 = Sunday, 7 = Saturday).
 * @param date: Date of the month (1-31).
 * @param month: Month of the year (1-12).
 * @param year: Year (0-99, representing 2000-2099).
 * @retval None.
 */
void DS3231_SetDate(uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
    // Validate the input parameters
    if (!IsDateValid(day, date, month, year))
    {
        return; // Exit the function if the date is invalid
    }

    uint8_t bcd_day, bcd_date, bcd_month, bcd_year;

    // Convert input values to BCD
    bcd_day = DecimalToBCD(day);
    bcd_date = DecimalToBCD(date);
    bcd_month = DecimalToBCD(month);
    bcd_year = DecimalToBCD(year);

    // Start I2C communication and write date values to DS3231
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send DS3231 address with write intent
    I2C_Send7bitAddress(I2C1, DS3231_ADDRESS << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Set register address to start writing from (Day register, 0x03)
    I2C_SendData(I2C1, DS3231_REG_DAY);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write Day of the week
    I2C_SendData(I2C1, bcd_day);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write Date
    I2C_SendData(I2C1, bcd_date);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write Month
    I2C_SendData(I2C1, bcd_month);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Write Year
    I2C_SendData(I2C1, bcd_year);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // End communication
    I2C_GenerateSTOP(I2C1, ENABLE);
}

/**
 * @brief Reads the current time (hours, minutes, seconds) from the DS3231 RTC module.
 * @param hours: Pointer to store the hour value (in decimal format).
 * @param minutes: Pointer to store the minute value (in decimal format).
 * @param seconds: Pointer to store the second value (in decimal format).
 * 
 * This function reads the time registers (seconds, minutes, and hours) from the DS3231,
 * converts the values from Binary-Coded Decimal (BCD) to decimal, and stores them 
 * in the provided pointers. It handles the I2C communication and reconfigures 
 * the DS3231 for subsequent operations.
 */
void DS3231_GetTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds)
{
    uint8_t raw_seconds, raw_minutes, raw_hours;

    // Send START condition
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send DS3231 address with write intent
    I2C_Send7bitAddress(I2C1, DS3231_ADDRESS << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Set register address to 0x00 (Seconds register)
    I2C_SendData(I2C1, DS3231_REG_SECONDS);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Send repeated START condition for read mode
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send DS3231 address with read intent
    I2C_Send7bitAddress(I2C1, DS3231_ADDRESS << 1 | 0x01, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    // Read Seconds
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    raw_seconds = I2C_ReceiveData(I2C1);

    // Read Minutes
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    raw_minutes = I2C_ReceiveData(I2C1);

    // Read Hours
    I2C_AcknowledgeConfig(I2C1, DISABLE); // Send NACK for the last byte
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    raw_hours = I2C_ReceiveData(I2C1);

    // Send STOP condition
    I2C_GenerateSTOP(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE); // Re-enable ACK for next communication

    // Convert BCD to decimal
    *seconds = BCDToDecimal(raw_seconds);
    *minutes = BCDToDecimal(raw_minutes);
    *hours = BCDToDecimal(raw_hours & 0x3F); // Mask bits 6 and 5 for hours
}

/**
 * @brief Reads the day, date, month, and year from the DS3231 RTC module.
 * @param day: Pointer to store the day of the week (1 = Sunday, 7 = Saturday).
 * @param date: Pointer to store the date of the month (1-31).
 * @param month: Pointer to store the month of the year (1-12).
 * @param year: Pointer to store the year (0-99, representing 2000-2099).
 * @retval None.
 */
void DS3231_GetDate(uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year)
{
    // Send START condition
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send DS3231 address with write intent
    I2C_Send7bitAddress(I2C1, DS3231_ADDRESS << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Set the register address to start reading from (Day register, 0x03)
    I2C_SendData(I2C1, DS3231_REG_DAY);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Send repeated START condition to switch to read mode
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    // Send DS3231 address with read intent
    I2C_Send7bitAddress(I2C1, DS3231_ADDRESS << 1 | 0x01, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    // Read Day
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    *day = BCDToDecimal(I2C_ReceiveData(I2C1));

    // Read Date
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    *date = BCDToDecimal(I2C_ReceiveData(I2C1));

    // Read Month
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    *month = BCDToDecimal(I2C_ReceiveData(I2C1) & 0x1F); // Mask century bit (bit 7)

    // Read Year
    I2C_AcknowledgeConfig(I2C1, DISABLE); // Send NACK for the last byte
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    *year = BCDToDecimal(I2C_ReceiveData(I2C1));

    // Send STOP condition to end communication
    I2C_GenerateSTOP(I2C1, ENABLE);

    // Re-enable ACK for future communications
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}
