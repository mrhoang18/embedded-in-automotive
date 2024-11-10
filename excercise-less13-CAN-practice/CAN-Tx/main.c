#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

uint8_t data[8] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};

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

void CAN_Transmitdata(uint8_t *data, uint8_t len, uint32_t id){
	CanTxMsg TxMessage;

    TxMessage.StdId = id; // 11bit ID voi che do std
    TxMessage.ExtId = 0x00;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.IDE = CAN_ID_STD;
    TxMessage.DLC = len;
	
    for (int i = 0; i < len; i++)
    {
        TxMessage.Data[i] = data[i];
    }

    CAN_Transmit(CAN1, &TxMessage);

}

int main(){
	RCC_Config();
	GPIO_Config();
	CAN_Config();
	
	CAN_Transmitdata(data, 8, 0x123);
	while(1){
	
	}
}
