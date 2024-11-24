/**
 * @file rtc_at24c32.h
 * @brief Header file for interfacing with DS3231 RTC and AT24C32 EEPROM using I2C.
 * 
 * This file provides function prototypes to configure and communicate with the 
 * DS3231 Real-Time Clock (RTC) and AT24C32 EEPROM. The DS3231 handles time 
 * and date management, while the AT24C32 provides non-volatile storage.
 * 
 * @note Ensure proper I2C initialization before using these functions.
 * @author Tong Xuan Hoang
 * @date 11/24/2024
 */

#ifndef DS3231_AT24C32_H
#define DS3231_AT24C32_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "stm32f10x.h"                  // Device header

/**
 * @brief I2C pin and port configuration for DS3231 communication.
 * 
 * These macros define the GPIO pins and port used for the I2C1 bus, 
 * which communicates with the DS3231 RTC.
 * 
 * Macros:
 * - `I2C_SCL`: Pin for I2C clock signal (SCL).
 * - `I2C_SDA`: Pin for I2C data signal (SDA).
 * - `I2C1_GPIO`: GPIO port used for I2C1 communication.
 */
#define I2C_SCL 	GPIO_Pin_6
#define I2C_SDA 	GPIO_Pin_7
#define I2C1_GPIO 	GPIOB

/**
 * @brief 7-bit I2C address for the DS3231 RTC module.
 * 
 * The DS3231 module communicates via the I2C bus and is addressed using 
 * a 7-bit address. This address is fixed and defined as `0x68`.
 * 
 * Macro:
 * - `DS3231_ADDRESS`: 7-bit I2C address for the DS3231 RTC.
 */
#define DS3231_ADDRESS 0x68 

/**
 * @brief Register addresses for DS3231 timekeeping functions.
 * 
 * These macros define the addresses of the DS3231 registers that store 
 * the timekeeping information, including seconds, minutes, hours, day of the week, 
 * date, month, and year. All values are stored in Binary-Coded Decimal (BCD) format.
 * 
 * Macros:
 * - `DS3231_REG_SECONDS`: Seconds register (0-59 in BCD format).
 * - `DS3231_REG_MINUTES`: Minutes register (0-59 in BCD format).
 * - `DS3231_REG_HOURS`: Hours register (0-23 in 24-hour format or 1-12 in 12-hour format).
 * - `DS3231_REG_DAY`: Day of the week register (1 = Sunday, 7 = Saturday).
 * - `DS3231_REG_DATE`: Date of the month register (1-31 in BCD format).
 * - `DS3231_REG_MONTH`: Month register (1-12 in BCD format; bit 7 indicates century).
 * - `DS3231_REG_YEAR`: Year register (0-99 in BCD format, representing 2000-2099).
 */
#define DS3231_REG_SECONDS       0x00  
#define DS3231_REG_MINUTES       0x01  
#define DS3231_REG_HOURS         0x02  
#define DS3231_REG_DAY           0x03  
#define DS3231_REG_DATE          0x04 
#define DS3231_REG_MONTH         0x05 
#define DS3231_REG_YEAR          0x06

/**
 * @brief Register addresses for DS3231 alarm configuration.
 * 
 * These macros define the addresses of the DS3231 alarm registers. The DS3231 
 * supports two alarms: Alarm 1 and Alarm 2. Each alarm has its own configuration 
 * for seconds, minutes, hours, and day/date.
 * 
 * Macros for Alarm 1:
 * - `DS3231_REG_ALARM1_SECONDS`: Alarm 1 seconds register.
 * - `DS3231_REG_ALARM1_MINUTES`: Alarm 1 minutes register.
 * - `DS3231_REG_ALARM1_HOURS`: Alarm 1 hours register.
 * - `DS3231_REG_ALARM1_DAY_DATE`: Alarm 1 day/date register.
 * 
 * Macros for Alarm 2:
 * - `DS3231_REG_ALARM2_MINUTES`: Alarm 2 minutes register.
 * - `DS3231_REG_ALARM2_HOURS`: Alarm 2 hours register.
 * - `DS3231_REG_ALARM2_DAY_DATE`: Alarm 2 day/date register.
 */
#define DS3231_REG_ALARM1_SECONDS	0x07 
#define DS3231_REG_ALARM1_MINUTES 	0x08 
#define DS3231_REG_ALARM1_HOURS   	0x09
#define DS3231_REG_ALARM1_DAY_DATE 	0x0A
#define DS3231_REG_ALARM2_MINUTES 	0x0B
#define DS3231_REG_ALARM2_HOURS   	0x0C
#define DS3231_REG_ALARM2_DAY_DATE 	0x0D

/**
 * @brief Register addresses for DS3231 control and status configuration.
 * 
 * These macros define the addresses of the DS3231 control and status registers, 
 * which are used to configure the device's behavior and monitor its state.
 * 
 * Macros:
 * - `DS3231_REG_CONTROL`: Control register for enabling/disabling features.
 * - `DS3231_REG_STATUS`: Status register for monitoring device state.
 * - `DS3231_REG_AGING_OFFSET`: Register for adjusting oscillator aging offset.
 */
#define DS3231_REG_CONTROL        0x0E // Control register
#define DS3231_REG_STATUS         0x0F // Status register
#define DS3231_REG_AGING_OFFSET   0x10 // Aging offset register

/**
 * @brief Register addresses for DS3231 temperature readings.
 * 
 * The DS3231 includes a built-in temperature sensor for compensating the clock's 
 * oscillator. These macros define the addresses of the registers storing the 
 * temperature value.
 * 
 * Macros:
 * - `DS3231_REG_TEMP_MSB`: Most significant byte (MSB) of the temperature value.
 * - `DS3231_REG_TEMP_LSB`: Least significant byte (LSB) of the temperature value.
 */
#define DS3231_REG_TEMP_MSB       0x11 // Temperature MSB register
#define DS3231_REG_TEMP_LSB       0x12 // Temperature LSB register

/**
 * @brief Constants for days of the week.
 * 
 * These macros define constants for the days of the week, where:
 * - `1 = Sunday`
 * - `7 = Saturday`
 * 
 * Macros:
 * - `DAY_SUNDAY`: Sunday.
 * - `DAY_MONDAY`: Monday.
 * - `DAY_TUESDAY`: Tuesday.
 * - `DAY_WEDNESDAY`: Wednesday.
 * - `DAY_THURSDAY`: Thursday.
 * - `DAY_FRIDAY`: Friday.
 * - `DAY_SATURDAY`: Saturday.
 */
#define DAY_SUNDAY     1
#define DAY_MONDAY     2
#define DAY_TUESDAY    3
#define DAY_WEDNESDAY  4
#define DAY_THURSDAY   5
#define DAY_FRIDAY     6
#define DAY_SATURDAY   7

/**
 * @brief Constants for months of the year.
 * 
 * These macros define constants for the months of the year, where:
 * - `1 = January`
 * - `12 = December`
 * 
 * Macros:
 * - `MONTH_JANUARY`: January.
 * - `MONTH_FEBRUARY`: February.
 * - `MONTH_MARCH`: March.
 * - `MONTH_APRIL`: April.
 * - `MONTH_MAY`: May.
 * - `MONTH_JUNE`: June.
 * - `MONTH_JULY`: July.
 * - `MONTH_AUGUST`: August.
 * - `MONTH_SEPTEMBER`: September.
 * - `MONTH_OCTOBER`: October.
 * - `MONTH_NOVEMBER`: November.
 * - `MONTH_DECEMBER`: December.
 */
#define MONTH_JANUARY    1
#define MONTH_FEBRUARY   2
#define MONTH_MARCH      3
#define MONTH_APRIL      4
#define MONTH_MAY        5
#define MONTH_JUNE       6  
#define MONTH_JULY       7
#define MONTH_AUGUST     8  
#define MONTH_SEPTEMBER  9  
#define MONTH_OCTOBER   10  
#define MONTH_NOVEMBER  11  
#define MONTH_DECEMBER  12  

/* AT24C32 EEPROM Functions */

/**
 * @brief Write a byte to a specific memory address in AT24C32.
 * @param at24c32Addr: I2C address of the AT24C32 EEPROM.
 * @param memAddr: 16-bit memory address where the data will be written.
 * @param dataValue: Byte value to write to the specified memory location.
 */
void At24c32_Write(uint8_t at24c32Addr, uint16_t memAddr, uint8_t dataValue);

/**
 * @brief Read a byte from a specific memory address in AT24C32.
 * @param at24c32Addr: I2C address of the AT24C32 EEPROM.
 * @param memAddr: 16-bit memory address to read from.
 * @retval The byte value read from the specified memory location.
 */
uint8_t At24c32_Random_Read(uint8_t at24c32Addr, uint16_t memAddr);

/* DS3231 RTC Functions */

/**
 * @brief Set the time on the DS3231 RTC.
 * @param hours: Hour value (1-12 for 12-hour mode, 0-23 for 24-hour mode).
 * @param minutes: Minute value (0-59).
 * @param seconds: Second value (0-59).
 * @param is_12_hour_mode: 0 for 24-hour mode, 1 for 12-hour mode.
 * @param is_pm: 0 for AM, 1 for PM (only applicable in 12-hour mode).
 */
void DS3231_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t is_12_hour_mode, uint8_t is_pm);

/**
 * @brief Get the current time from the DS3231 RTC.
 * @param hours: Pointer to store the hour value.
 * @param minutes: Pointer to store the minute value.
 * @param seconds: Pointer to store the second value.
 */
void DS3231_GetTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds);

/**
 * @brief Set the date on the DS3231 RTC.
 * @param day: Day of the week (1 = Sunday, 7 = Saturday).
 * @param date: Date of the month (1-31).
 * @param month: Month of the year (1-12).
 * @param year: Year (0-99, representing 2000-2099).
 */
void DS3231_SetDate(uint8_t day, uint8_t date, uint8_t month, uint8_t year);

/**
 * @brief Get the current date from the DS3231 RTC.
 * @param day: Pointer to store the day of the week.
 * @param date: Pointer to store the date of the month.
 * @param month: Pointer to store the month of the year.
 * @param year: Pointer to store the year (0-99, representing 2000-2099).
 */
void DS3231_GetDate(uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year);

/**
 * @brief Converts a day number (1-7) to the corresponding day name.
 * @param day: Day number (1 = Sunday, 7 = Saturday).
 * @retval Pointer to a string containing the day name.
 */
const char*  DS3231_GetDayName(uint8_t day);

#ifdef __cplusplus
}
#endif
#endif // DS3231_AT24C32_H
