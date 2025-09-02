/******************************************************************************
* @file  MCtrlXnIN.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all MCtrlXnIN functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 
#include "MCtrlMain.h"
#include "MappCode.h"

XnFilterCnt tXnFilter;
/*---------------------------------------------------------------------
;Functional prototype: void DoXnScanLoop(void)
;
;description:
;
;On Entry:   P_X1_IN,P_X2_IN;
;
;On Exit:    void.
---------------------------------------------------------------------*/
void DoXnScanLoop(void)
{
#if(TempCtrlType==TempCtrlWF96||TempCtrlType==TempCtrlWF96R||\
		TempCtrlType==TempCtrlWF49||TempCtrlType==TempCtrlWF49R)

	if(P_X1_IN == 0)
	{
		if(tXnFilter.InMX1HCnt<IO_FILFER)
		{
			tXnFilter.InMX1HCnt++;
			if(tXnFilter.InMX1HCnt==IO_FILFER)
			{
				tXnFilter.InnerTemp = 0;
			}
		}
		tXnFilter.InMX1LCnt= 0;
	}
	else
	{
		if(tXnFilter.InMX1LCnt<IO_FILFER)
		{
			tXnFilter.InMX1LCnt++;
			if(tXnFilter.InMX1LCnt==IO_FILFER)
			{
				tXnFilter.InnerTemp = 1;

				SetOutpTmpToRef(gParcode.Item.InnerTemp*10);
			}
		}
		tXnFilter.InMX1HCnt=0;
	}

	if(P_X2_IN == 0)
	{
		if(tXnFilter.InMX2HCnt<IO_FILFER)
		{
			tXnFilter.InMX2HCnt++;
			if(tXnFilter.InMX2HCnt==IO_FILFER)
			{
				if(gParcode.Item.XRunSwitch==2)
					SetOutputSwitch(0);
			}
		}
		tXnFilter.InMX2LCnt= 0;
	}
	else
	{
		if(tXnFilter.InMX2LCnt<IO_FILFER)
		{
			tXnFilter.InMX2LCnt++;
			if(tXnFilter.InMX2LCnt==IO_FILFER)
			{
				if(gParcode.Item.XRunSwitch==2)
					SetOutputSwitch(1);
			}
		}
		tXnFilter.InMX2HCnt=0;
	}
#endif
}

/*---------------------------------------------------------------------
;Functional prototype: void GetInnerTempFlag(void)
;
;description:
;
;On Entry:   void;
;
;On Exit:    InnerTempFlag.
---------------------------------------------------------------------*/
uint16_t GetInnerTempFlag(void)
{
#if(TempCtrlType==TempCtrlWF96||TempCtrlType==TempCtrlWF96R||\
		TempCtrlType==TempCtrlWF49||TempCtrlType==TempCtrlWF49R)
	return tXnFilter.InnerTemp;
#else
	return 0;
#endif
}

