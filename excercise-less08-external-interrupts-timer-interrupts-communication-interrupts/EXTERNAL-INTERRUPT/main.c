#include "stm32f10x.h"                  // Device header

void LEDPC13_Config(void);
void EXTI_Config(void);


int main()
{
	LEDPC13_Config();
	EXTI_Config();
	
	while(1)
	{
		
	}
	
}

// Function to configure PC13 as output for LED
void LEDPC13_Config(void) {
    // Enable Clock for GPIOC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // Configure PC13 as output push-pull
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    // Output push-pull mode
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;    // Set speed to 2 MHz (sufficient for LED)
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// Function to configure EXTI line 0 for PA0 with Pull-Up
void EXTI_Config(void) {
    // Enable Clock for GPIOA and AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // Configure PA0 as Input Pull-Up
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // Input Pull-Up
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure EXTI line 0 to trigger interrupt from PA0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;                 // Select EXTI line 0
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;        // Interrupt mode
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;    // Trigger interrupt on falling edge
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;                  // Enable EXTI line 0
    EXTI_Init(&EXTI_InitStruct);

    // Configure interrupt priority grouping in NVIC
    // 2 bits for Preemption Priority, 2 bits for Sub Priority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         

    // Configure interrupt priority in NVIC for EXTI line 0
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;           // EXTI0 interrupt channel
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  // Preemption priority (can be customized)
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;         // Sub priority (can be customized)
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;            // Enable interrupt channel in NVIC
    NVIC_Init(&NVIC_InitStruct);
}

// Interrupt Service Routine for EXTI line 0
void EXTI0_IRQHandler(void) 
	{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) 
	{
         for (volatile int i = 0; i < 30000; i++); // Debounce button
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET) {
            GPIOC->ODR ^= GPIO_Pin_13;	// Toggle
        }
        EXTI_ClearITPendingBit(EXTI_Line0); 	// Clear flag
    }
}
	