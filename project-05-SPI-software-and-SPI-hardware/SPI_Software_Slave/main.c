#include "stm32f10x.h"      // Device header
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h"  // Keil::Device:StdPeriph Drivers:TIM

#define SPI_SCK_Pin GPIO_Pin_0
#define SPI_MISO_Pin GPIO_Pin_1
#define SPI_MOSI_Pin GPIO_Pin_2
#define SPI_CS_Pin GPIO_Pin_3

#define SPI_GPIO GPIOA

#define SPI_RCC RCC_APB2Periph_GPIOA

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
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 72MHz, because:72MHz/1=72MHz

    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    TIM_Cmd(TIM2, ENABLE);
}
void RCC_Config()
{
    RCC_APB2PeriphClockCmd(SPI_RCC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

// For slave
void GPIO_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SPI_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPI_MISO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
	
	
}

void Clock()
{
    GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_SET);
    Delay_Ms(4);
    GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_RESET);
    Delay_Ms(4);
}

void SPI_Config()
{
    GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_RESET);
    GPIO_WriteBit(SPI_GPIO, SPI_CS_Pin, Bit_SET);
    GPIO_WriteBit(SPI_GPIO, SPI_MISO_Pin, Bit_RESET);
    GPIO_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_RESET);
}

uint8_t SPI_Slave_Receive(void)
{
    uint8_t dataReceive = 0x00; // 0b11000000
    uint8_t temp = 0x00, i = 0;

    while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));

    while (!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));

    for (i = 0; i < 8; i++)
    {
        if (GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin))
        {
            while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin))
			{
                temp = GPIO_ReadInputDataBit(SPI_GPIO, SPI_MOSI_Pin);
			}
            dataReceive = dataReceive << 1;
            dataReceive = dataReceive | temp;
        }
        while (!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
    }
    return dataReceive;
}

uint8_t Data;

int main()
{
    RCC_Config();
    GPIO_Config();
    TIM_Config();
    SPI_Config();

    while (1)
    {
        if (!(GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin)))
        {
            for (int i = 0; i < 10; i++)
            {
				Data = SPI_Slave_Receive();
            }
        }
    }
}
