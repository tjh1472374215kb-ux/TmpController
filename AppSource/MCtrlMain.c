/******************************************************************************
* @file  MCtrlMain.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the LED Showing firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "MCtrlMain.h"
#include "MappCode.h"
#include "MappShow.h"

CtrlOut_PARM tCtrlOutParm;
/*---------------------------------------------------------------------
;Functional prototype: void DoMainCtrlLoop(void)
;
;description:
;
;On Entry:   OutpSwitch,Calc Heat/Cool TmpOutput;
;
;On Exit:    Output Duty Ratio .
---------------------------------------------------------------------*/
void DoMainCtrlLoop(void)
{
	if(tCtrlOutParm.OutpSwitch>0)
	{
		if(gParcode.Item.CtrlStyle==1)
		{
			tCtrlOutParm.HeatPeriod++;
			
			if(tCtrlOutParm.HeatPeriod>=gParcode.Item.HeatPeriod)
			{
				tCtrlOutParm.HeatPeriod = 0;
				CalcHeatTmpOutput();
			}
		}
		else if(gParcode.Item.CtrlStyle==2)
		{
			tCtrlOutParm.CoolPeriod++;
			
			if(tCtrlOutParm.CoolPeriod>=gParcode.Item.CoolPeriod)
			{
				tCtrlOutParm.CoolPeriod = 0;
				CalcCoolTmpOutput();
			}
		}
		else if(gParcode.Item.CtrlStyle==3)
		{
			int16_t tmpErr;

			tmpErr = tHeatRamp.qRefTmp - tHeatRamp.qFedTmp;
			
			if(tmpErr > tCtrlOutParm.qHeatCoolErr)
			{//start to heat...
				if(tCtrlOutParm.HeatCoolSw==0)
				{
					tCtrlOutParm.HeatCoolSw = 1;

					tCtrlOutParm.HeatPeriod=gParcode.Item.HeatPeriod;

					//initial the current output temperature value...
					tHeatRamp.qOutTmp = tHeatRamp.qFedTmp;
				}
			}
			else if(tmpErr < -tCtrlOutParm.qHeatCoolErr)
			{//start to Cool...
				if(tCtrlOutParm.HeatCoolSw==0)
				{
					tCtrlOutParm.HeatCoolSw = 2;

					tCtrlOutParm.CoolPeriod=gParcode.Item.CoolPeriod;

					//initial the current output temperature value...
					tCoolRamp.qOutTmp = tCoolRamp.qFedTmp;
				}
			}

			if(tCtrlOutParm.HeatCoolSw==1)
			{
				tCtrlOutParm.HeatPeriod++;
		
				if(tCtrlOutParm.HeatPeriod>=gParcode.Item.HeatPeriod)
				{
					tCtrlOutParm.HeatPeriod = 0;
					CalcHeatTmpOutput();

					if(tHeatRamp.Equation==1)
					{	//close to heat...
						if(tHeatRamp.qFedTmp>=tHeatRamp.qRefTmp)
							tCtrlOutParm.HeatCoolSw=0;
					}
				}
			}
			else if(tCtrlOutParm.HeatCoolSw==2)
			{
				tCtrlOutParm.CoolPeriod++;
			
				if(tCtrlOutParm.CoolPeriod>=gParcode.Item.CoolPeriod)
				{
					tCtrlOutParm.CoolPeriod = 0;
					CalcCoolTmpOutput();

					if(tCoolRamp.Equation==1)
					{//close to Cool...
						if(tHeatRamp.qFedTmp<=tHeatRamp.qRefTmp)
							tCtrlOutParm.HeatCoolSw=0;
					}
				}
			}
		}
	}
}

/*---------------------------------------------------------------------
;Functional prototype: void SetOutputSwitch(void)
;
;description:
;
;On Entry:   ON or OFF;
;
;On Exit:    void .
---------------------------------------------------------------------*/
void SetOutputSwitch(uint16_t OnOff)
{
	tCtrlOutParm.OutpSwitch = OnOff;

	if(tCtrlOutParm.OutpSwitch>0)
	{
		tCtrlOutParm.HeatCoolSw = 0;

		//make sure to run in next second..
		tCtrlOutParm.HeatPeriod = gParcode.Item.HeatPeriod;
		tCtrlOutParm.CoolPeriod = gParcode.Item.CoolPeriod;

		//initial the current output temperature value...
		tHeatRamp.qOutTmp = tHeatRamp.qFedTmp;
		tCoolRamp.qOutTmp = tCoolRamp.qFedTmp;

		if(gParcode.Item.CtrlStyle==3)
		{	//ERROR 13
//			if(gParcode.Item.HeatOutput==gParcode.Item.CoolOutput)
//				SetErrorCode(14);
//			else if(gParcode.Item.HeatOutput<3&&gParcode.Item.CoolOutput<3)
//				SetErrorCode(14);
		}
	}
	else
	{
		tCtrlOutParm.HeatPwmSwOn = 0;
		tCtrlOutParm.CoolPwmSwOn = 0;

		SetOutpDC12V(0);
		SetOutpRelay(0);

		SetOut1Flag(0);
		SetOut2Flag(0);
	}
}

/*----------------------------------------*/
/*----------------------------------------*/
uint16_t GetOutputFlag(void)
{
	return tCtrlOutParm.OutpSwitch;
}
/*---------------------------------------------------------------------
;Functional prototype: void SetOutpTmpToRef( void )
;
;On Entry:  Ref/Fed Temperature.
;
;On Exit:   void  
;
; Return:		Void
---------------------------------------------------------------------*/
void SetOutpTmpToRef(int16_t tmp)
{
	tCtrlOutParm.SetTempValue = tmp;
	
	SetHeatTmpToRef(tCtrlOutParm.SetTempValue);

	SetCoolTmpToRef(tCtrlOutParm.SetTempValue);
}

void SetRxINTmpToFed(int16_t tmp)
{
	tCtrlOutParm.GetTempT1Val = tmp + gParcode.Item.ErrCorrectedV;

//	if(tCtrlOutParm.GetTempT1Val>(gParcode.Item.UpLmtWarn*10))
//		SetErrorCode(2);
//	else if(tCtrlOutParm.GetTempT1Val<(gParcode.Item.DnLmtWarn*10))
//		SetErrorCode(3);
    
    //TempErrorWarn();
	
	SetHeatTmpToFed(tCtrlOutParm.GetTempT1Val);

	SetCoolTmpToFed(tCtrlOutParm.GetTempT1Val);
}

void SetRxINTmpToSub(int16_t tmp)
{
	tCtrlOutParm.GetTempT2Val = tmp + gParcode.Item.ErrCorrectedV;

	if(tCtrlOutParm.GetTempT2Val>=(gParcode.Item.CscdT2Limit*10))
		tCtrlOutParm.GetTempT2Val=(gParcode.Item.CscdT2Limit*10);
		
	tCtrlOutParm.qTempT2Value = ((long)(tCtrlOutParm.GetTempT2Val+TempMinPU)<<15)/TempMaxPU;
}

/*---------------------------------------------------------------------
;Functional prototype: void CalcHeatTmpOutput(void)
;
;description:
;
;On Entry:   tHeatRamp structure and tPICParm structure;
;
;On Exit:    Heat Output Duty Ratio .
---------------------------------------------------------------------*/
void CalcHeatTmpOutput(void)
{
	int16_t qTmpOut;
		
	tHeatRamp.qSetTmp=tHeatRamp.qRefTmp;

	if(tHeatRamp.Equation==1)
	{
		int16_t tmpErr;

		tmpErr = tHeatRamp.qFedTmp - tHeatRamp.qOutTmp;

		if(tmpErr > tCtrlOutParm.qUpErrWarn)
		{//ERROR 3
			SetErrorCode(4);
		}
		else if(tmpErr < tCtrlOutParm.qDnErrWarn)
		{//ERROR 4
			SetErrorCode(5);
		}
	}
		
	CalcTempRamp(&tHeatRamp);
	
	tHeatPic.qRef = tHeatRamp.qOutTmp;
	tHeatPic.qFdb = tHeatRamp.qFedTmp;

	CalcPicFunc(&tHeatPic);

	#if(TempCtrlType==TempCtrlWF96||TempCtrlType==TempCtrlWF96R||\
			TempCtrlType==TempCtrlWF49||TempCtrlType==TempCtrlWF49R)
	if(gParcode.Item.CascadeSwon==2)
	{
		tSubT2Pic.qRef = tHeatPic.qOutput;
		tSubT2Pic.qFdb = tCtrlOutParm.qTempT2Value;
	
		CalcPicFunc(&tSubT2Pic);

		qTmpOut = tSubT2Pic.qOutput;
	}
	else
	{
//		qTmpOut = tHeatPic.qOutput;
        qTmpOut = 32767;
	}
	#else
	qTmpOut = tHeatPic.qOutput;
	#endif
	
	if(gParcode.Item.HeatOutput==2)
	{
		uint16_t tmpdata;

		tmpdata = (100UL*qTmpOut)>>15;
		
		if(tmpdata > (gParcode.Item.HeatAnlgMax*10))
			tmpdata = gParcode.Item.HeatAnlgMax*10;
		else if(tmpdata < (gParcode.Item.HeatAnlgMin*10))
			tmpdata = gParcode.Item.HeatAnlgMin*10;
		
		SetOutpDACxV(tmpdata);
	}
	else
	{
		tCtrlOutParm.HeatPwmHCnt = ((uint32_t)tHeatRamp.MaxOutPU*qTmpOut)>>15;
		tCtrlOutParm.HeatPwmLCnt = tHeatRamp.MaxOutPU-tCtrlOutParm.HeatPwmHCnt;

		if(gParcode.Item.HeatOutput==1)
			tCtrlOutParm.HeatPwmSwOn = 1;
		else
			tCtrlOutParm.HeatPwmSwOn = 2;
	}
}
/*---------------------------------------------------------------------
;Functional prototype: void CalcCoolTmpOutput(void)
;
;description:
;
;On Entry:   tCoolRamp structure and tPICParm structure;
;
;On Exit:    Cool Output Duty Ratio .
---------------------------------------------------------------------*/
void CalcCoolTmpOutput(void)
{
	int16_t qTmpOut;
	
	tCoolRamp.qSetTmp=tCoolRamp.qRefTmp;

	if(tCoolRamp.Equation==1)
	{
		int16_t tmpErr;

		tmpErr = tCoolRamp.qFedTmp - tCoolRamp.qOutTmp;

		if(tmpErr > tCtrlOutParm.qUpErrWarn)
		{//ERROR 3
			SetErrorCode(4);
		}
		else if(tmpErr < tCtrlOutParm.qDnErrWarn)
		{//ERROR 4
			SetErrorCode(5);
		}
	}
	
	CalcTempRamp(&tCoolRamp);
	
	//tCoolPic.qRef = tCoolRamp.qOutTmp;
	//tCoolPic.qFdb = tCoolRamp.qFedTmp;
	tCoolPic.qRef = tCoolRamp.qFedTmp;
	tCoolPic.qFdb = tCoolRamp.qOutTmp;
	
	CalcPicFunc(&tCoolPic);

	#if(TempCtrlType==TempCtrlWF96||TempCtrlType==TempCtrlWF96R||\
			TempCtrlType==TempCtrlWF49||TempCtrlType==TempCtrlWF49R)
	if(gParcode.Item.CascadeSwon==2)
	{
		tSubT2Pic.qRef = tCtrlOutParm.qTempT2Value;
		tSubT2Pic.qFdb = tCoolPic.qOutput;
	
		CalcPicFunc(&tSubT2Pic);

		qTmpOut = tSubT2Pic.qOutput;
	}
	else
	{
//		qTmpOut = tHeatPic.qOutput;
        qTmpOut = 32767;
	}
	#else
	qTmpOut = tHeatPic.qOutput;
	#endif
	
	if(gParcode.Item.CoolOutput==2)
	{
		uint16_t tmpdata;

		tmpdata = (100UL*qTmpOut)>>15;

		if(tmpdata > (gParcode.Item.CoolAnlgMax*10))
			tmpdata = gParcode.Item.CoolAnlgMax*10;
		else if(tmpdata < (gParcode.Item.CoolAnlgMin*10))
			tmpdata = gParcode.Item.CoolAnlgMin*10;
		
		SetOutpDACxV(tmpdata);
	}
	else
	{
		tCtrlOutParm.CoolPwmHCnt = ((uint32_t)tCoolRamp.MaxOutPU*qTmpOut)>>15;
		tCtrlOutParm.CoolPwmLCnt = tCoolRamp.MaxOutPU-tCtrlOutParm.CoolPwmHCnt;

		if(gParcode.Item.CoolOutput==1)
			tCtrlOutParm.CoolPwmSwOn = 1;
		else
			tCtrlOutParm.CoolPwmSwOn = 2;
	}
}
/*---------------------------------------------------------------------
;Functional prototype: void SetHeatCoolErrQv(void)
;
;description:
;
;On Entry:   HeatCoolErr;
;
;On Exit:    qHeatCoolErr .
---------------------------------------------------------------------*/
void SetHeatCoolErrQv(void)
{
	tCtrlOutParm.qHeatCoolErr = ((long)gParcode.Item.HeatCoolErr<<15)/TempMaxPU;
}
/******************************************************************************
*Function 		: void TIMER0_IRQHandler(void)
*Description	: it sets as a regular time:
							: it is 10 ms period Handler
*******************************************************************************/
void TIMER0_IRQHandler(void)
{
	if(TMR0->RIS&0x01)
	{
		if(tCtrlOutParm.HeatPwmSwOn>0)
		{
			if(tCtrlOutParm.HeatPwmHCnt>0)
			{
				tCtrlOutParm.HeatPwmHCnt--;

				if(tCtrlOutParm.HeatPwmSwOn==1)
					SetOutpDC12V(1);
				else
					SetOutpRelay(1);
			}
			else if(tCtrlOutParm.HeatPwmLCnt>0)
			{
				tCtrlOutParm.HeatPwmLCnt--;

				if(tCtrlOutParm.HeatPwmSwOn==1)
					SetOutpDC12V(0);
				else
					SetOutpRelay(0);
			}
			else
			{
				tCtrlOutParm.HeatPwmSwOn=0;
			}
		}

		if(tCtrlOutParm.CoolPwmSwOn>0)
		{
			if(tCtrlOutParm.CoolPwmHCnt>0)
			{
				tCtrlOutParm.CoolPwmHCnt--;

				if(tCtrlOutParm.CoolPwmSwOn==1)
					SetOutpDC12V(1);
				else
					SetOutpRelay(1);
			}
			else if(tCtrlOutParm.CoolPwmLCnt>0)
			{
				tCtrlOutParm.CoolPwmLCnt--;

				if(tCtrlOutParm.CoolPwmSwOn==1)
					SetOutpDC12V(0);
				else
					SetOutpRelay(0);
			}
			else
			{
				tCtrlOutParm.CoolPwmSwOn=0;
			}
		}
		TMR0->ICLR = 1;
	}
}

