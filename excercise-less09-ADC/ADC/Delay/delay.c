#include "delay.h"

void delay_us(uint32_t delay) {
    TIM_SetCounter(TIM2, 0);  // Reset Timer counter to 0
    while (TIM_GetCounter(TIM2) < delay);  // Wait until the counter reaches the delay value
}

void DelayMs(void) {
    TIM_SetCounter(TIM2, 0);  // Reset Timer counter to 0
    while (TIM_GetCounter(TIM2) < 1000);  // Wait until counter reaches 1000 (1 millisecond)
}

void Delay_Ms(uint32_t u32DelayInMs) {
    while (u32DelayInMs) {
        DelayMs();  // Delay for 1 millisecond
        --u32DelayInMs;  // Decrement delay count
    }
}

void TIM_Config(){
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;  
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;   
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    TIM_Cmd(TIM2, ENABLE);
}
