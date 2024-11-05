/***************************************************************************
 * @file    Delay.h
 * @brief   
 * @details 
 *          
 * @version 
 * @date    2024-09-11
 * @author  TONG XUAN HOANG
 * @website 
 ***************************************************************************/

#include "stm32f10x.h"                  // Device header
#include "Delay.h"

// Delay function using Timer2
void Delay_Us(uint32_t Time)
{
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < Time);
}

void Delay_Ms(uint32_t Time) {
    for (uint32_t i = 0; i < Time; i++) {
        Delay_Us(500); // 1000 µs = 1 ms
    }
}

// Configure TIM2 to use as a microsecond delay timer
void TIM2_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; // Prescale to 1 MHz (1 µs period)
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;    // Maximum period for counting
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    TIM_Cmd(TIM2, ENABLE);
}
