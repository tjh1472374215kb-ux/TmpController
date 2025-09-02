/******************************************************************************
* @file  drvForADC.c
* @author JSCC
* @version  V1.0.0
* @date  30/07/2025
* @brief  This file provides all the ADC firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 
#include "drvForMain.h"

/*********************************/
//void ADCConfiguration(void)
/*********************************/
void ADCConfiguration(void)
{
	// 使能操作 ADC1 寄存器
	ADC->LOCK = 0x55;
    
	// 设置 ADC0 控制寄存器
	ADC->CON = 0x0B;
    
	// 设置ADC 采样时间： 64.5时钟周期
	ADC->CON |= 0x40UL<<16;
	
	// 设置 ADC8/ADC16/ADC17 采样通道
	ADC->SCAN = (1<<8)|(1<<16)|(1<<17);
    
	// 设置 ADC0 硬件触发控制
	ADC->HWTG = (1<<15)|(1<<12);
	
	// 使能 ADC1 控制位
	ADC->CON |= 0x10;
	
	// 使能操作 ADC0 寄存器
	ADC->LOCK = 0x55;
}


