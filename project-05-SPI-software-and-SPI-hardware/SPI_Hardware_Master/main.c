#include "stm32f10x.h"

// Delay function in milliseconds
void Delay_Ms(uint16_t Time)
{
    TIM_SetCounter(TIM2, 0);

    while (TIM_GetCounter(TIM2) < Time * 10)
    {
        // Delay is approximately Time ms, as Time * 10 * 0.1ms = Time ms
    }
}

// Timer configuration function
void TIM_Config()
{
    // Enable the clock for TIM2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    // Set clock division to 1, resulting in a 72MHz clock frequency
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;

    // Prescaler set to 7200 - 1, dividing the clock by 7200 (72MHz/7200 = 10kHz)
    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    TIM_Cmd(TIM2, ENABLE);
}

// Initialize SPI for Master mode and configure CS pin
void SPI1_Master_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable the clock for SPI1 and GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    // Configure GPIO pins for SPI1: SCK, MISO, MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure CS pin (PA4) as Output Push-Pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4); // Set CS high (inactive, not selecting slave)

    // Configure SPI in Master mode
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  // NSS managed by software
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
	
    SPI_Init(SPI1, &SPI_InitStructure);

    // Enable SPI1
    SPI_Cmd(SPI1, ENABLE);
}

// Transmit a single byte of data via SPI
void SPI1_Master_Transmit(uint8_t data)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_4); // Pull CS low to activate slave

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);		// Wait for TXE (transmit buffer empty) to be set
    SPI_I2S_SendData(SPI1, data); // Send data
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);	// Wait until data is received
    SPI_I2S_ReceiveData(SPI1); // Read received data (discarded)

    GPIO_SetBits(GPIOA, GPIO_Pin_4); // Set CS high after transmission is complete
}

// Data array to transmit
uint8_t Data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

int main(void)
{
    SPI1_Master_Init();  // Initialize SPI1 in Master mode
    TIM_Config();        // Configure the timer
    while (1)
    {
        for (int i = 0; i < 10; i++)
        {
            SPI1_Master_Transmit(Data[i]); // Transmit data from 0 to 9
            Delay_Ms(500);                 // Delay for 500 ms
        }
    }
}
