/******************************************************************************
* @file  drvForCCP.c
* @author JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the CCP firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 
#include "drvForMain.h"

/*********************************/
//void CCPConfiguration(void)
/*********************************/
void CCPConfiguration(void)
{
	//开锁
	CCP->LOCK = 0x55UL;
	
	//清除 CCP0控制寄存器
	CCP->CON0 = 0;
	
	//设置 CCP0重加载寄存器
	CCP->LOAD0 = (1UL<<16)|PeriodPwm;
	
	//设置 CCP0 PWM模式
	CCP->CON0 |= (1<<3);
	
	//清除 CCP0A数据寄存器
	CCP->D0A = 0;
	
	//设置 CCP0运行寄存器
	CCP->RUN = 1;
	
	//使能 CCP0
	CCP->CON0 |=(1<<6);

	//关锁
	CCP->LOCK = 0x00UL;
    
    PORT->P06CFG = 0x02;
}

