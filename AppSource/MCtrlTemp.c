/******************************************************************************
* @file  MCtrlTemp.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the LED Showing firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 
#include "MCtrlMain.h"
#include "MappCode.h"

Temp_Ramp tHeatRamp,tCoolRamp;
/*---------------------------------------------------------------------
;Functional prototype: void CalcTempRamp( void )
;
;On Entry:   Temp_Ramp structure must contain dSetTmp,dAccTmp,dDecTmp.
;
;On Exit:    Temp_Ramp will contain dOutTmp  
;
; Return:		Void
---------------------------------------------------------------------*/
void CalcTempRamp(Temp_Ramp* v)
{
	int16_t tmpDeta;

	tmpDeta = v->qSetTmp - v->qOutTmp;

	if(tmpDeta == 0)
	{
		v->Equation = 1;
	}
	else if(tmpDeta > 0)
	{
		if(tmpDeta > v->qAccTmp)
		{
			v->qOutTmp += v->qAccTmp;
			v->Equation = 0;
		}
		else
		{
			v->qOutTmp = v->qSetTmp;
			v->Equation = 1;
		}
	}
	else //if(tmpDeta < 0)
	{
		if(tmpDeta < v->qDecTmp)
		{
			v->qOutTmp += v->qDecTmp;
			v->Equation = 0;
		}
		else
		{
			v->qOutTmp = v->qSetTmp;
			v->Equation = 1;
		}
	}
}

/*---------------------------------------------------------------------
;Functional prototype: void SetHeatTmpToRef( void )
;
;On Entry:  Ref/Fed Temperature.
;
;On Exit:   void  
;
; Return:		Void
---------------------------------------------------------------------*/
void SetHeatTmpToRef(int16_t tmp)
{
	tHeatRamp.qRefTmp = ((long)(tmp+TempMinPU)<<15)/tHeatRamp.MaxTmpPU;
}

void SetHeatTmpToFed(int16_t tmp)
{
	tHeatRamp.qFedTmp = ((long)(tmp+TempMinPU)<<15)/tHeatRamp.MaxTmpPU;
}

/*---------------------------------------------------------------------
;Functional prototype: void SetCoolTmpToRef( void )
;
;On Entry:  Ref/Fed Temperature.
;
;On Exit:   void  
;
; Return:		Void
---------------------------------------------------------------------*/
void SetCoolTmpToRef(int16_t tmp)
{
	tCoolRamp.qRefTmp = ((long)(tmp+TempMinPU)<<15)/tCoolRamp.MaxTmpPU;
}

void SetCoolTmpToFed(int16_t tmp)
{
	tCoolRamp.qFedTmp = ((long)(tmp+TempMinPU)<<15)/tCoolRamp.MaxTmpPU;
}

/*---------------------------------------------------------------------
;Functional prototype: void InitTempRamp( void )
;
;On Entry:   Temp_Ramp structure must contain dSetTmp,dAccTmp,dDecTmp.
;
;On Exit:    Temp_Ramp will contain dOutTmp  
;
; Return:		Void
---------------------------------------------------------------------*/
void InitTempRamp(void)
{
	uint32_t tmpLong;
	
	tHeatRamp.MaxTmpPU = TempMaxPU;
	tmpLong = (uint32_t)gParcode.Item.HeatPeriod<<15/6;
	tHeatRamp.qAccTmp = tmpLong*gParcode.Item.HeatAccRate/tHeatRamp.MaxTmpPU;
	tHeatRamp.qDecTmp = tmpLong*gParcode.Item.HeatDecRate/tHeatRamp.MaxTmpPU;
	tHeatRamp.MaxOutPU = gParcode.Item.HeatPeriod*100;

	tHeatRamp.qDecTmp = -tHeatRamp.qDecTmp;
	
	tCoolRamp.MaxTmpPU = TempMaxPU;
	tmpLong = (uint32_t)gParcode.Item.CoolPeriod<<15/6;
	tCoolRamp.qAccTmp = tmpLong*gParcode.Item.CoolAccRate/tCoolRamp.MaxTmpPU;
	tCoolRamp.qDecTmp = tmpLong*gParcode.Item.CoolDecRate/tCoolRamp.MaxTmpPU;
	tCoolRamp.MaxOutPU = gParcode.Item.CoolPeriod*100;

	tCoolRamp.qDecTmp = -tCoolRamp.qDecTmp;
}

