#include "stm32f10x.h"       // Device header
#include "stm32f10x_gpio.h"  // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"   // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_usart.h" // Keil::Device:StdPeriph Drivers:USART

#include "Delay.h"

#define TX_Pin GPIO_Pin_9
#define RX_Pin GPIO_Pin_10
#define UART_GPIO GPIOA


void GPIO_Config()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    GPIO_InitTypeDef GPIOInitStruct;
    GPIOInitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIOInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIOInitStruct);
    
    GPIOInitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIOInitStruct);
}

void UART_Config()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
    USART_InitTypeDef USARTInitStruct;
    
    USARTInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USARTInitStruct.USART_BaudRate = 9600;
    USARTInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USARTInitStruct.USART_WordLength = USART_WordLength_8b;
    USARTInitStruct.USART_StopBits = USART_StopBits_1;
    USARTInitStruct.USART_Parity = USART_Parity_No;

    USART_Init(USART1, &USARTInitStruct);
    USART_Cmd(USART1, ENABLE);
}

char UART_ReceiveChar(USART_TypeDef *USARTx)
{
    USARTx->DR = 0x00;
    char tmp = 0x00;
    tmp = USART_ReceiveData(USARTx);

    while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);

    return tmp;
}

void UART_SendChar(USART_TypeDef *USARTx, char data)
{
    USARTx->DR = 0x00;
    USART_SendData(USARTx, data);

    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

void UART_SendString(USART_TypeDef *USARTx, char *str)
{
    while (*str)
    {
        UART_SendChar(USARTx, *str);
        str++;
    }
}

uint8_t data;

int main()
{
    GPIO_Config();
    UART_Config();

    while (1)
    {
        data = UART_ReceiveChar(USART1);
        UART_SendChar(USART1, data);
    }
}
