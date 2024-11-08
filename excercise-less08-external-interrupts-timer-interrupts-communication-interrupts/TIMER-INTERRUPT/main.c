#include "stm32f10x.h"                  // Device header

void TIM_Config(void);
void NVIC_Config(void);

int main()
{
	TIM_Config();
	NVIC_Config();
	
	while(1)
	{
		
	}
}

void TIM_Config(void) 
{
    // Configure Timer 2
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    // Enable Clock for Timer 2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // Configure parameters for Timer 2
    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;         // Prescaler: 72 MHz / 7200 = 10 kHz
    TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;           // Period: 1 second (increments once per second)
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // No additional clock division
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // Up-counting mode
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    // Enable Timer 2 interrupt for update event
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // Activate Timer 2
    TIM_Cmd(TIM2, ENABLE);

    // Configure NVIC for Timer 2
    NVIC_Config();
}

void NVIC_Config(void) 
{
    // Set interrupt priority grouping configuration
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 2 bits for Preemption Priority, 2 bits for Sub Priority

    // Configure interrupt for Timer 2 in NVIC
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;             // Select Timer 2 interrupt channel
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;   // Set preemption priority level
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;          // Set sub-priority level
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;             // Enable interrupt channel in NVIC
    NVIC_Init(&NVIC_InitStruct);
}

uint16_t count;

void delay(int time)
{
    count = 0; 
    while(count < time){}
}

void TIM2_IRQHandler()
{
    // Wait to interrupt
    if(TIM_GetITStatus(TIM2, TIM_IT_Update))
    {
		count++;
    }
    // Clear flag
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}