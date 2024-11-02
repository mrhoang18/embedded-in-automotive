#include "stm32f10x.h"

// Kh?i t?o SPI cho Slave
void SPI1_Slave_Init(void)
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

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Ch? d? d?u vào v?i pull-up (n?u c?n)
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // C?u hình SPI ? ch? d? Slave
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard; // Ch? d? NSS c?ng
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // B?t SPI1
    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_Slave_Receive(void)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        ;                             // Ch? nh?n d? li?u
    return SPI_I2S_ReceiveData(SPI1); // Ð?c d? li?u nh?n du?c
}

uint8_t SPI_Receive1Byte(void)
{
    uint8_t temp;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == 1)
        ;
    temp = (uint8_t)SPI_I2S_ReceiveData(SPI1);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == 0)
        ;
    return temp;
}

uint8_t received_data = 0;

int main(void)
{
    SPI1_Slave_Init();

    while (1)
    {
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
        {
            for (int i = 0; i < 10; i++)
            {
                received_data = SPI1_Slave_Receive();
            }
        }
    }
}
