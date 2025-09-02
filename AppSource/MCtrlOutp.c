/******************************************************************************
* @file  MCtrlOutp.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the LED Showing firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "MCtrlMain.h"
#include "MappShow.h"

/*---------------------------------------------------------------------
;Functional prototype: void SetOutpDACxV( uint16_t Vdat )
;
;On Entry:   Voltage value (0.0 ~ 10.0V)
;
;On Exit:    void  
;
; Return:		Void
---------------------------------------------------------------------*/
void SetOutpDACxV(uint16_t Vdat)
{
	uint16_t tmpdata;

	//the max input data is 100(10.0V)
	if(Vdat>100) Vdat =100;
	
	tmpdata = (uint32_t)PeriodPwm*Vdat/120;
		
	CCP->LOCK = 0xaa;
	CCP->D0A = tmpdata;
	CCP->LOCK = 0x00;

	SetOut1Flag(Vdat);
}

/*---------------------------------------------------------------------
;Functional prototype: void SetOutpDC12V( uint16_t Vdat )
;
;On Entry:   switch on or off
;
;On Exit:    void  
;
; Return:		Void
---------------------------------------------------------------------*/
void SetOutpDC12V(uint16_t OnOff)
{
	CCP->LOCK = 0xaa;
	if(OnOff>0)
		CCP->D0A = PeriodPwm;
	else
		CCP->D0A = 0;
	CCP->LOCK = 0x00;

	if(OnOff>0)
		SetOut1Flag(100);
	else
		SetOut1Flag(0);
}

/*---------------------------------------------------------------------
;Functional prototype: void SetOutpRelay( uint16_t Vdat )
;
;On Entry:   switch on or off
;
;On Exit:    void  
;
; Return:		Void
---------------------------------------------------------------------*/
void SetOutpRelay(uint16_t OnOff)
{
	if(OnOff>0)
		P_RELAY2_H;
	else
		P_RELAY2_L;

	SetOut2Flag(OnOff);
}

