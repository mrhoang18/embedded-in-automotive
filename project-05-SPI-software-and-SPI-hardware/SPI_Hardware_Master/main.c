#include "stm32f10x.h"

void Delay_Ms(uint16_t Time)
{
    TIM_SetCounter(TIM2, 0);

    while (TIM_GetCounter(TIM2) < Time * 10)
    {
        // Delay timedelay*10*0,1ms = timedelay ms
    }
}

void TIM_Config()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 72MHz, because:72MHz/1=72MHz

    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    TIM_Cmd(TIM2, ENABLE);
}

// Kh?i t?o SPI cho Master và c?u hình chân CS
void SPI1_Master_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // B?t Clock cho SPI1 và GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    // C?u hình chân GPIO cho SPI1: SCK, MISO, MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // C?u hình chân CS (PA4) làm Output Push-Pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4); // Ð?t CS ? m?c cao (không kích ho?t slave)

    // C?u hình SPI ? ch? d? Master
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // B?t SPI1
    SPI_Cmd(SPI1, ENABLE);
}

void SPI1_Master_Transmit(uint8_t data)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_4); // Kéo CS xu?ng m?c th?p d? kích ho?t slave

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;                         // Ch? TXE du?c set
    SPI_I2S_SendData(SPI1, data); // G?i d? li?u
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        ;                      // Ch? d? li?u nh?n
    SPI_I2S_ReceiveData(SPI1); // Ð?c d? li?u nh?n (b? qua)

    GPIO_SetBits(GPIOA, GPIO_Pin_4); // Ð?t CS v? m?c cao sau khi truy?n xong
}

uint8_t Data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

int main(void)
{
    SPI1_Master_Init();
    TIM_Config();
    while (1)
    {
        for (int i = 0; i < 10; i++)
        {
            SPI1_Master_Transmit(Data[i]); // Truy?n d? li?u t? 0 d?n 9
            Delay_Ms(500);
        }
    }
}
