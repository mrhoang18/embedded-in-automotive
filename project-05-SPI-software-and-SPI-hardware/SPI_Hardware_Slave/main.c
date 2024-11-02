#include "stm32f10x.h"

// Initialize SPI for Slave mode
void SPI1_Slave_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable Clock for SPI1 and GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    // Configure GPIO pins for SPI1: SCK, MISO, MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // Alternate Function Push-Pull
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure GPIO pin for CS (Chip Select) as Floating Input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Input floating mode for CS
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure SPI in Slave mode
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard; // NSS is controlled by hardware
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // Enable SPI1
    SPI_Cmd(SPI1, ENABLE);
}

// Function to receive data via SPI1 in Slave mode
uint8_t SPI1_Slave_Receive(void)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        ;                             // Wait until data is received
    return SPI_I2S_ReceiveData(SPI1); // Read received data
}

// Alternative function to receive a single byte via SPI
uint8_t SPI_Receive1Byte(void)
{
    uint8_t temp;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1);		// Wait until SPI is not busy
    temp = (uint8_t)SPI_I2S_ReceiveData(SPI1);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == 0);	// Wait until data is received
    return temp;
}

uint8_t received_data = 0; // Variable to store received data

int main(void)
{
    SPI1_Slave_Init(); // Initialize SPI1 in Slave mode

    while (1)
    {
        // Check if CS (PA4) is low, indicating data transfer
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
        {
            // Loop to receive data 10 times
            for (int i = 0; i < 10; i++)
            {
                received_data = SPI1_Slave_Receive(); // Receive data and store in received_data
            }
        }
    }
}
