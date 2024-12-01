#include "stm32f10x.h"
#include <stdbool.h>

void GPIO_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Timer2_Init(void)
{
    // K�ch ho?t clock cho Timer 2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // C?u h�nh Timer 2
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;  // �? c� 1 ms (1 kHz)
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; // Gi? s? Clock c?a h? th?ng l� 72 MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // K�ch ho?t Timer 2
    TIM_Cmd(TIM2, ENABLE);

    // C?u h�nh c? Update (UD) c?a Timer 2
    TIM_ClearFlag(TIM2, TIM_FLAG_Update); // X�a c? update ban d?u
}

void Delay_ms(uint32_t ms)
{
    // Reset l?i b? d?m c?a Timer 2
    TIM_SetCounter(TIM2, 0);

    while (TIM_GetCounter(TIM2) < ms)
    {
        // �?i cho d?n khi d?m d? s? ms y�u c?u
    }
}

volatile uint32_t i = 0;
volatile bool isPress = false;
int main(void)
{

    GPIO_Config();
    Timer2_Init();

    while (1)
    {
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == RESET)
        {
            Delay_ms(20);
            if ((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == RESET) && (isPress == false))
            {
                i++;
                isPress = true;
            }
        }
        else
            isPress = false;
    }
}
