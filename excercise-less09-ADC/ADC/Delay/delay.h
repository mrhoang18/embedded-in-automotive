#ifndef DELAY_H
#define DELAY_H

#include "stm32f10x.h"              // Include file cho STM32F10x
#include "stm32f10x_tim.h"          // Include file để sử dụng Timer

// Khai báo các hàm delay
void DelayMs(void);                             // Hàm tạo độ trễ 1 ms
void delay_us(uint32_t delay);                  // Hàm tạo độ trễ theo micro giây
void Delay_Ms(uint32_t u32DelayInMs);           // Hàm tạo độ trễ theo mili giây
void TIM_Config(void);                          // Cấu hình Timer

#endif  // Kết thúc kiểm tra DELAY_H
