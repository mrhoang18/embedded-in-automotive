#include "stm32f10x.h"

#define BUFFER_SIZE 10
uint8_t rx_buffer[BUFFER_SIZE]; // Buffer to store received data rx_buffer

// Initialize SPI1 in Slave mode with DMA
void SPI1_Slave_DMA_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // Enable Clock for SPI1, GPIOA, and DMA1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

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

    // Configure DMA1 Channel2 for SPI1_RX
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR; // SPI1 data register
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rx_buffer;     // Buffer to store received data
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;             // Data from peripheral to memory
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;                // Size of the buffer
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                  // Normal mode (not circular)
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                   // Memory-to-memory disabled
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    // Enable DMA1 Channel2
    DMA_Cmd(DMA1_Channel2, ENABLE);

    // Enable SPI1 RX DMA request
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
}

// Function to check if DMA transfer is complete
uint8_t Is_DMA_Transfer_Complete(void)
{
    return DMA_GetFlagStatus(DMA1_FLAG_TC2); // Check transfer complete flag for DMA1 Channel2
}

int main(void)
{
    SPI1_Slave_DMA_Init(); // Initialize SPI1 in Slave mode with DMA

    while (1)
    {
        // Check if CS (PA4) is low, indicating data transfer
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
        {
            // Start the DMA transfer
            DMA_Cmd(DMA1_Channel2, ENABLE);

            // Wait for the transfer to complete
            while (!Is_DMA_Transfer_Complete());

            // Clear DMA transfer complete flag
            DMA_ClearFlag(DMA1_FLAG_TC2);

            // Process received data in rx_buffer
            // Add your processing code here
        }
    }
}
