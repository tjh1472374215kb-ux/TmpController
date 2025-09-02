/******************************************************************************
* @file  drvForUART.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the UART firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "drvForMain.h"

/*********************************/
//void UARTConfiguration(void)
/*********************************/
void UARTConfiguration(void)
{
	//P24 as UART1_TXD
	PORT->P24CFG = 0x01;
	//P23 as UART1_RXD
	PORT->UART1RXDCFG = 0x00;
	
	//设置UART格式为 8,e,1
	UART1->LCR = 0x1b;
	//设置UART波特率
	UART1->DLR = SysClockFreq/16/38400;
	//使能UART接收数据中断
	UART1->IER = 0x01;
}

