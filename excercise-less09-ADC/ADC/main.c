#include "stm32f10x.h"      // Device header
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_adc.h"  // Keil::Device:StdPeriph Drivers:ADC
#include "delay.h"
#include "math.h"

void RCC_Config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Config()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void ADC_Config()
{
    ADC_InitTypeDef ADC_InitStruct;

    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;

    ADC_Init(ADC1, &ADC_InitStruct);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_Cmd(ADC1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// Global variables for Kalman Filter
float _err_measure = 1;  // Measurement error (initial value)
float _err_estimate = 1; // Estimation error (initial value)
float _q = 0.01;         // Process noise
float _kalman_gain = 0;
float _current_estimate = 0; // Current estimated value
float _last_estimate = 0;    // Previous estimated value

// Kalman Filter initialization function
void SimpleKalmanFilter(float mea_e, float est_e, float q)
{
    _err_measure = mea_e;
    _err_estimate = est_e;
    _q = q;
}

// Kalman Filter update function
float updateEstimate(float mea)
{
    _kalman_gain = _err_estimate / (_err_estimate + _err_measure);
    _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
    _err_estimate = (1.0 - _kalman_gain) * _err_estimate + fabs(_last_estimate - _current_estimate) * _q;
    _last_estimate = _current_estimate;
    return _current_estimate;
}

uint16_t adc_raw, sum;
float adc_filtered;

int main()
{
    RCC_Config();
    TIM_Config();
    GPIO_Config();
    ADC_Config();

    // Initialize Kalman Filter with default values
    SimpleKalmanFilter(2, 2, 0.01); // Measurement error and estimation error set to 2, process noise set to 0.01

    while (1)
    {

        // Using Average - Not recommended!
        /*
        for(int i=0; i<10; i++)
        {
            adc_raw = ADC_GetConversionValue(ADC1);
            delay_us(100);
            sum+=adc_raw;
        }
        sum = sum/10;
        Delay_Ms(100);
        sum = 0;
        */

        // Using Kalman Filter - Recommended
        // Read raw ADC value
        adc_raw = ADC_GetConversionValue(ADC1);

        // Filter ADC value using Kalman Filter
        adc_filtered = updateEstimate((float)adc_raw);
    }
}
