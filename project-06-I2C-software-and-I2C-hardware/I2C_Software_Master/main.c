#include "stm32f10x.h"      // Device header
#include "stm32f10x_rcc.h"  // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h"  // Keil::Device:StdPeriph Drivers:TIM

#define I2C_SCL GPIO_Pin_6
#define I2C_SDA GPIO_Pin_7
#define I2C_GPIO GPIOB

#define WRITE_SDA_0 GPIO_ResetBits(I2C_GPIO, I2C_SDA)
#define WRITE_SDA_1 GPIO_SetBits(I2C_GPIO, I2C_SDA)
#define WRITE_SCL_0 GPIO_ResetBits(I2C_GPIO, I2C_SCL)
#define WRITE_SCL_1 GPIO_SetBits(I2C_GPIO, I2C_SCL)
#define READ_SDA_VAL GPIO_ReadInputDataBit(I2C_GPIO, I2C_SDA)

typedef enum
{
    NOT_OK = 0,
    OK = 1
} Status;

typedef enum
{
    NACK = 0,
    ACK = 1
} ACK_Bit;

// Delay function using Timer2
void Delay_Us(uint32_t Delay)
{
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < Delay)
        ;
}

// Configure clocks for GPIOB and TIM2
void RCC_Config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

// Configure TIM2 to use as a microsecond delay timer
void TIM2_Config()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; // Prescale to 1 MHz (1 �s period)
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;    // Maximum period for counting
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    TIM_Cmd(TIM2, ENABLE);
}

// Configure GPIO for I2C SDA and SCL pins
void GPIO_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // Open-drain mode for I2C
    GPIO_InitStructure.GPIO_Pin = I2C_SDA | I2C_SCL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(I2C_GPIO, &GPIO_InitStructure);
}

// Initialize I2C line (set SDA and SCL high)
void I2C_Config()
{
    WRITE_SDA_1;
    Delay_Us(1);
    WRITE_SCL_1;
    Delay_Us(1);
}

// Generate I2C start condition
void I2C_Start()
{
    WRITE_SCL_1;
    Delay_Us(3);
    WRITE_SDA_1;
    Delay_Us(3);
    WRITE_SDA_0; // Pull SDA low before SCL
    Delay_Us(3);
    WRITE_SCL_0;
    Delay_Us(3);
}

// Generate I2C stop condition
void I2C_Stop()
{
    WRITE_SDA_0;
    Delay_Us(3);
    WRITE_SCL_1; // Pull SCL high before SDA
    Delay_Us(3);
    WRITE_SDA_1;
    Delay_Us(3);
}

// Write a byte to I2C and check for ACK
Status I2C_Write(uint8_t u8Data)
{
    uint8_t i;
    Status result;

    for (i = 0; i < 8; i++)
    { // Write byte bit by bit
        if (u8Data & 0x80)
        {
            WRITE_SDA_1;
        }
        else
        {
            WRITE_SDA_0;
        }
        Delay_Us(3);
        WRITE_SCL_1; // Clock pulse
        Delay_Us(5);
        WRITE_SCL_0;
        Delay_Us(2);
        u8Data <<= 1;
    }
    WRITE_SDA_1; // Release SDA for ACK bit
    Delay_Us(3);
    WRITE_SCL_1; // Clock pulse for ACK bit
    Delay_Us(3);

    // Check for ACK from slave
    if (READ_SDA_VAL)
    {
        result = NOT_OK;
    }
    else
    {
        result = OK;
    }

    Delay_Us(2);
    WRITE_SCL_0;
    Delay_Us(5);

    return result;
}

// Read a byte from I2C and send ACK/NACK
uint8_t I2C_Read(ACK_Bit _ACK)
{
    uint8_t i;
    uint8_t u8Ret = 0x00;
    WRITE_SDA_1; // Release SDA to receive data
    Delay_Us(3);
    for (i = 0; i < 8; ++i)
    {
        u8Ret <<= 1;
        WRITE_SCL_1; // Clock pulse
        Delay_Us(3);
        if (READ_SDA_VAL)
        { // Read bit
            u8Ret |= 0x01;
        }
        Delay_Us(2);
        WRITE_SCL_0;
        Delay_Us(5);
    }

    // Send ACK or NACK after reading byte
    if (_ACK)
    {
        WRITE_SDA_0;
    }
    else
    {
        WRITE_SDA_1;
    }
    Delay_Us(3);

    WRITE_SCL_1; // Clock pulse for ACK/NACK
    Delay_Us(5);
    WRITE_SCL_0;
    Delay_Us(5);

    return u8Ret;
}

// Define EEPROM I2C address (7-bit address) and memory location
uint8_t slave_address = 0x57;
uint8_t memory_address_high = 0x00;
uint8_t memory_address_low = 0x10; // Specific memory address in EEPROM
uint8_t data_to_send = 0x60;
uint8_t received_data = 0x00;

int main()
{
    RCC_Config();  // Configure clocks for GPIO and Timer
    TIM2_Config(); // Configure Timer2 for delay function
    GPIO_Config(); // Configure GPIO pins for I2C
    I2C_Config();  // Initialize I2C lines (set idle state)

    Status status;

    while (1)
    {
        // Start I2C communication and write data to EEPROM
        I2C_Start();

        status = I2C_Write(slave_address << 1); // Send slave address + write bit

        if (status == OK)
        {
            // Send high byte of memory address in EEPROM
            status = I2C_Write(memory_address_high);
            if (status == OK)
            {
                // Send low byte of memory address in EEPROM
                status = I2C_Write(memory_address_low);
                if (status == OK)
                {
                    // Send data to write to EEPROM
                    status = I2C_Write(data_to_send);
                    if (status == OK)
                    {
                        I2C_Stop(); // End write operation

                        // Wait for EEPROM write cycle to complete (typically 5ms)
                        Delay_Us(5000);

                        // Read back data from the same EEPROM address to verify
                        I2C_Start();

                        status = I2C_Write(slave_address << 1); // Send slave address + write bit to set address
                        if (status == OK)
                        {
                            status = I2C_Write(memory_address_high); // Send high byte of memory address
                            if (status == OK)
                            {
                                status = I2C_Write(memory_address_low); // Send low byte of memory address
                                if (status == OK)
                                {
                                    // Restart communication and send read request
                                    I2C_Start();

                                    status = I2C_Write((slave_address << 1) | 1); // Send slave address + read bit
                                    if (status == OK)
                                    {
                                        received_data = I2C_Read(NACK); // Read data and send NACK
                                    }
                                    I2C_Stop(); // End read operation
                                }
                            }
                        }
                    }
                }
            }
        }

        Delay_Us(1000); // Delay before repeating
    }
}