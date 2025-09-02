/******************************************************************************
* @file  drvForADC.c
* @author JSCC
* @version  V1.0.0
* @date  05/06/2025
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
    
	// 设置 ADC 控制寄存器
	ADC->CON = 0x0B;
    
	// 设置ADC 采样时间： 64.5时钟周期
	ADC->CON |= 0x40UL<<16;
	
	// 设置 ADC8/ADC16/ADC17 采样通道
	ADC->SCAN = (1<<8)|(1<<16)|(1<<17);
    
	// 设置 ADC 硬件触发控制  触发源为ADC转换结束信号
	ADC->HWTG = (1<<15)|(1<<12);
	
	// 使能 ADC 控制位
	ADC->CON |= 0x10;
	
	// 使能操作 ADC 寄存器
	ADC->LOCK = 0x55;
}

/************************************************************
*function		: ADC_GetResult
*description	: 获取ADC转换结果
*On Entry	    : Channel: ADC_CH_0 ~ ADC_CH_26
*On Exit		: 12 bits Value
************************************************************/
uint32_t ADC_GetResult(uint32_t Channel)
{
	return(ADC->DATA[Channel]);
}
/************************************************************
*function		: ADC_Start
*description	: 启动ADC转换
*On Entry	    : 
*On Exit		: 
************************************************************/
void ADC_Start(void)
{
	ADC->LOCK = 0x55;           //解锁
    ADC->CON2 |= (0x0080);      //启动
    ADC->LOCK = 0x00;           //上锁
}
