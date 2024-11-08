#include "stm32f10x.h"

// Function to initialize GPIO for the LED
void LED_Init(void) {
    // Enable Clock for GPIOC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // Configure PC13 as output push-pull
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// Function to configure Timer 2
void TIM_Config(void) {
    // Enable Clock for Timer 2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // Configure parameters for Timer 2
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;     // Clock division (only applies to Input Capture/Output Compare filter)
    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;             // Prescaler: 72 MHz / 7200 = 10 kHz (Timer frequency, counting frequency)
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;                  // Period: Counts from 0 to 0xFFFF (65535) then resets
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // Up-counting mode
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    // Enable Timer 2
    TIM_Cmd(TIM2, ENABLE);
}

// Function to create a delay in milliseconds using Timer 2
void Delay_ms(uint16_t time_ms) {
    // Reset the Timer 2 counter
    TIM_SetCounter(TIM2, 0);

    // Calculate the required ticks
    uint16_t ticks = time_ms * 10; // With Timer frequency at 10 kHz, each tick equals 0.1 ms

    // Wait until the counter reaches the required ticks
    while (TIM_GetCounter(TIM2) < ticks);
}

// Main function to toggle LED on/off
int main(void) {
    // Initialize LED and Timer
    LED_Init();
    TIM_Config();

    while (1) {
        // Turn on LED (PC13)
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        Delay_ms(500); // Delay for 500 ms

        // Turn off LED (PC13)
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        Delay_ms(500); // Delay for 500 ms
    }
}
