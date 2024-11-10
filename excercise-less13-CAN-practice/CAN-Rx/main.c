#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

uint16_t id = 0x123;
uint32_t ID;
uint8_t TestArray[8];

void RCC_Config(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void CAN_Config(){
	CAN_InitTypeDef CAN_InitStruct;
	
	CAN_InitStruct.CAN_TTCM = DISABLE;
	CAN_InitStruct.CAN_ABOM = DISABLE;
	CAN_InitStruct.CAN_AWUM = DISABLE;
	CAN_InitStruct.CAN_NART = DISABLE;
	CAN_InitStruct.CAN_RFLM = DISABLE;
	CAN_InitStruct.CAN_TXFP = DISABLE;
	CAN_InitStruct.CAN_Mode = CAN_Mode_Normal;
	
	//Cau hinh toc do can.
	//36Mhz
	//fCan = 36000000/4 = 9 000 000.
	//1tq = 1/9000000.
	//Toc do CAN = 1/(9tq) = 1Mb/s
	CAN_InitStruct.CAN_Prescaler = 4;
	CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStruct.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStruct.CAN_BS2 = CAN_BS2_5tq;
	
	CAN_Init(CAN1, &CAN_InitStruct);
}

void CAN_Filter_Config(){
	CAN_FilterInitTypeDef FilterInitStruct;
	
	FilterInitStruct.CAN_FilterNumber = 0;
	FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
	FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
	FilterInitStruct.CAN_FilterMaskIdHigh = 0xFF00;
	FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;
	FilterInitStruct.CAN_FilterIdHigh = 0x123 << 5;
	FilterInitStruct.CAN_FilterIdLow = 0x0000;
	FilterInitStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	FilterInitStruct.CAN_FilterActivation = ENABLE;
	
	CAN_FilterInit(&FilterInitStruct);
}


void CAN_Receivedata(){
	CanRxMsg RxMessage;
    while (CAN_MessagePending(CAN1, CAN_FIFO0) <1 );
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	ID = RxMessage.StdId;
	uint8_t RTR = RxMessage.RTR;
    for (int i = 0; i < RxMessage.DLC; i++)
    {
        TestArray[i] = RxMessage.Data[i];
		// TestArray
    }

}
int main(){
	RCC_Config();
	GPIO_Config();
	CAN_Config();
	CAN_Filter_Config();
	while(1){
		CAN_Receivedata();
	}
}