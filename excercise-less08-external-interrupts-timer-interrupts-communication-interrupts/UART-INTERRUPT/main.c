#include "stm32f10x.h"  // Include the necessary headers

void UART_Config(void);
void NVIC_Config(void);

int main()
{
	UART_Config();
	NVIC_Config();
	
	while(1)
	{
	}
}
void UART_Config(void) {
    // Enable Clock for USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // Enable Clock for GPIOA (for TX and RX pins)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure PA9 (TX) as Alternate Function Push-Pull
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure PA10 (RX) as Floating Input
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure UART parameters
    USART_InitTypeDef UART_InitStruct;
    UART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    UART_InitStruct.USART_BaudRate = 9600;
    UART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    UART_InitStruct.USART_WordLength = USART_WordLength_8b;
    UART_InitStruct.USART_StopBits = USART_StopBits_1;
    UART_InitStruct.USART_Parity = USART_Parity_No;
    USART_Init(USART1, &UART_InitStruct);

    // Enable RX interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // Enable USART1
    USART_Cmd(USART1, ENABLE);
}

void NVIC_Config(void) {
    // Set interrupt priority grouping configuration
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 2 bits for Preemption Priority, 2 bits for Sub Priority

    // Configure NVIC for USART1 interrupt
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;            // Select UART1 interrupt channel
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;    // Set preemption priority level
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;           // Set sub-priority level
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;              // Enable interrupt channel in NVIC
    NVIC_Init(&NVIC_InitStruct);
}

uint8_t data = 0x00;
void USART1_IRQHandler()
{

    // Check if RXNE interrupt (data received) is triggered
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        // Read data from the receive buffer
        data = USART_ReceiveData(USART1);

        // Wait until the transmit buffer is empty (TXE flag set)
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

        // Send the received data back
        USART_SendData(USART1, data);

        // Clear the RXNE interrupt flag
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}


