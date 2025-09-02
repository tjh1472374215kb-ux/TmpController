/******************************************************************************
* @file  MCtrlReset.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all MCtrlReset functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "MCtrlMain.h"
#include "MappCode.h"

/*---------------------------------------------------------------------
;Functional prototype: void DoWarnAutoReset(void)
;
;description:
;
;On Entry:   void;
;
;On Exit:    void.
---------------------------------------------------------------------*/
void DoWarnAutoReset(void)
{
	if(gParcode.Item.WarnReset==1)
	{
		int16_t tmpErr;

		if(GetErrorCode()==2)
		{
			if(tCtrlOutParm.GetTempT1Val<(gParcode.Item.UpLmtWarn*10))
				SetErrorCode(0);
		}
		else if(GetErrorCode()==3)
		{
			if(tCtrlOutParm.GetTempT1Val>(gParcode.Item.DnLmtWarn*10))
				SetErrorCode(0);
		}
		else if(GetErrorCode()==4)
		{
			tmpErr = tHeatRamp.qFedTmp - tHeatRamp.qRefTmp;

			if(tmpErr < tCtrlOutParm.qUpErrWarn)
				SetErrorCode(0);
		}
		else if(GetErrorCode()==5)
		{
			tmpErr = tHeatRamp.qFedTmp - tHeatRamp.qRefTmp;

			if(tmpErr > tCtrlOutParm.qDnErrWarn)
				SetErrorCode(0);
		}
	}
}
/*---------------------------------------------------------------------
;Functional prototype: void SetUpDnErrWarnQv(void)
;
;description:
;
;On Entry:   UpErrWarn and DnErrWarn setting value;
;
;On Exit:    qUpErrWarn,qDnErrWarn.
---------------------------------------------------------------------*/
void SetUpDnErrWarnQv(void)
{
	tCtrlOutParm.qUpErrWarn = ((long)gParcode.Item.UpErrWarn<<15)/TempMaxPU;
	tCtrlOutParm.qDnErrWarn = ((long)gParcode.Item.DnErrWarn<<15)/TempMaxPU;
}


