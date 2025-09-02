/******************************************************************************
* @file  McFuncPid.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the LED Showing firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "McFuncPid.h"
#include "MappCode.h"

tPICParm tHeatPic,tCoolPic;
tPICParm tSubT2Pic;

/*---------------------------------------------------------------------
;Functional prototype: void CalcPidFunc( void )
;
;On Entry:   tPICParm structure must contain qRef, qFed.
;
;On Exit:    tPICParm will contain qOutput  
;
; Return:		Void
---------------------------------------------------------------------*/
void CalcPicFunc(tPICParm *v)
{
	v->qErr = v->qRef - v->qFdb;
	
	if(v->SatFlag == 0)
		v->qSatErr += v->qErr;
	else if(v->SatFlag==1&&v->qErr<0)
		v->qSatErr += v->qErr;
	else if(v->SatFlag==2&&v->qErr>0)
		v->qSatErr += v->qErr;

	v->qUp = ((long)v->iKp*v->qErr)>>PidQBit;
	
	v->qUi = ((long)v->iKi*v->qSatErr)>>PidQBit;
	
	v->qOutSat = v->qUp + v->qUi;

	// Saturate the output
	if (v->qOutSat > v->qOutMax)
	{
	    v->SatFlag = 1;
	    v->qOutput =  v->qOutMax;
	}
	else if (v->qOutSat < v->qOutMin)
	{
	    v->SatFlag = 2;
	    v->qOutput =  v->qOutMin;
	}
	else
	{
	    v->SatFlag = 0;
	    v->qOutput = v->qOutSat;
	}
}

/*---------------------------------------------------------------------
;Functional prototype: void CalcPidFunc( void )
;
;On Entry:   tPICParm structure must contain qRef, qFed.
;
;On Exit:    tPICParm will contain qOutput  
;
; Return:		Void
---------------------------------------------------------------------*/
void CalcPidFunc(tPIDParm *v)
{
	v->qErr = v->qRef - v->qFdb;
	
	v->qErrD2 = v->qErrD1;
	v->qErrD1 = v->qErrD0;
	v->qErrP1 = v->qErrP0;
	
	v->qErrP0 = ((long)v->qErr*v->iKP)>>PidQBit;
	v->qErrI0 = ((long)v->qErr*v->iKI)>>PidQBit;
	v->qErrD0 = ((long)v->qErr*v->iKD)>>PidQBit;

	v->qErr = v->qErrP0+v->qErrI0+v->qErrD0+v->qErrD2-v->qErrP1-(v->qErrD1<<1);
	
	if(v->qErr>v->qErrMax)
		v->qErr = v->qErrMax;
	else if(v->qErr<v->qErrMin)
		v->qErr = v->qErrMin;
	
	v->qOutput = v->qOutput + v->qErr;

	if(v->qOutput > v->qRangeH)
	{
		v->qOutput = v->qRangeH;
	}
	else if(v->qOutput < v->qRangeL)
	{
		v->qOutput = v->qRangeL;
	}
}

/*---------------------------------------------------------------------
;Functional prototype: void CalcPidFunc( void )
;
;On Entry:   tPICParm structure must contain qRef, qFed.
;
;On Exit:    tPICParm will contain qOutput  
;
; Return:		Void
---------------------------------------------------------------------*/
void InitPidParm(void)
{
	tHeatPic.iKp = ((long)gParcode.Item.qPID01Kp<<PidQBit)/1000;
	tHeatPic.iKi = ((long)gParcode.Item.qPID01Ki<<PidQBit)/1000;

	tHeatPic.qOutMax = 0x7fff;
	tHeatPic.qOutMin = 0;
		
	tCoolPic.iKp = ((long)gParcode.Item.qPID02Kp<<PidQBit)/1000;
	tCoolPic.iKi = ((long)gParcode.Item.qPID02Ki<<PidQBit)/1000;

	tCoolPic.qOutMax = 0x7fff;
	tCoolPic.qOutMin = 0;

	tSubT2Pic.iKp = ((long)gParcode.Item.qPID11Kp<<PidQBit)/1000;
	tSubT2Pic.iKi = ((long)gParcode.Item.qPID11Ki<<PidQBit)/1000;

	tSubT2Pic.qOutMax = 0x7fff;
	tSubT2Pic.qOutMin = 0;
}

