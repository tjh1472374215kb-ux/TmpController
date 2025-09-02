/******************************************************************************
* @file  MappMain.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the main loop firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "MappMain.h"
#include "MappShow.h"
#include "MappMenu.h"
#include "MappCode.h"
#include "MappComm.h"

#include "MCtrlMain.h"

tSysTime dtSysTime;

tMainPara dtMain;

int main(void)
{	
	uint16_t delayCnt;

	delayCnt = 0x0fff;
	while(delayCnt--);

	SYSConfiguration();
	
	TMRConfiguration();
	
	GPIOConfiguration();
	UARTConfiguration();
	
	ADCConfiguration();
	CCPConfiguration();

	DispShowFrc1(1234,1);
    
	DispShowFrc2(5678,1);

	//DispShowLedx(0x08);
	
	dtMain.U8PwrOn = 0;
	dtMain.U8InitStp=0;
	dtMain.U8InitDly=200;
//-------------------------Test Function----------------------------------
	while(1)
	{
		
		
		break;
	}

//--------------------------Power on----------------------------------
	while(1)
	{
		if(dtMain.U8PwrOn==0) //power on
		{
            if(dtSysTime.u250usTick>0)
            {
                dtSysTime.u250usCount++;
                if(dtSysTime.u250usCount>=2)
                {
                    dtSysTime.u250usCount = 0;
                    dtSysTime.uTimeTick++;
                }
                DoLedBitScan();
                DoLedKeyScan();
            }
			if(dtSysTime.uTimeTick>0)
			{
				dtSysTime.uTimeTick=0;
				dtSysTime.uTimeStep++;
				
				if(dtSysTime.uTimeStep&0x01)
				{
					MappPowerOn();
					//DoLedKeyScan();
				}

				if(dtSysTime.uTimeStep&0x03)
					MappTimeCnt();
				
				if(dtSysTime.uTimeStep>3)
					dtSysTime.uTimeStep=0;
			}
		}
		else
		{
			break;
		}
	}

//-----------------------MainLoop----------------------------------------
	while(1)
	{
	#if (TempCtrlType>=TempCtrlWF72R)
		DoModbusScan();
	#endif
        
		if(dtSysTime.u250usTick>0)
		{
            dtSysTime.u250usTick=0;
			dtSysTime.u250usCount++;
			if(dtSysTime.u250usCount>=2)
			{
				dtSysTime.u250usCount = 0;
				dtSysTime.uTimeTick++;
			}
			DoLedBitScan();
			DoLedKeyScan();
			
		}
		if(dtSysTime.uTimeTick>0)
		{
			dtSysTime.uTimeTick=0;
			dtSysTime.uTimeStep++;

			MappUxComTime();

			if(dtMain.uError==0)
				DoXnScanLoop();
			
			if(dtSysTime.uTimeStep&0x01)
			{
				//DoLedKeyScan();
			}
			else
			{
				//DoLedBitScan();
			}

			if(dtSysTime.uTimeStep==1)
			{
				MappTimeCnt();
			}
			else if(dtSysTime.uTimeStep==2)
			{
				if(dtMain.uError<2)
					MappMainMenu();
			}
			else if(dtSysTime.uTimeStep==3)
			{
			
			}
			else 
			{
				dtSysTime.uTimeStep = 0;
			}
		}
		
		if(dtMain.uError>0)
		{
			DoWarnAutoReset();

			if(dtSysTime.u810MS)
			{
				dtSysTime.u810MS=0;

				if(dtMain.LHAInitTmr>2)
                {
                    ADCPanelUpdate();
                }
			}
			
			if(dtSysTime.u8100MS>0)
			{
				dtSysTime.u8100MS=0;
				
				if(dtMain.uError==1)
					MappMenu100MS();
                
                CalcTempValue();
                
				SetRxINTmpToFed(T1TempValue());
                
				SetRxINTmpToSub(T2TempValue());
			}

			if(dtSysTime.u81000MS>0)
			{
				dtSysTime.u81000MS=0;
                
				//DoMainCtrlLoop();
                
				if(stMenu.u8MenuID==1)
				{
					if(dtMain.U8ErrSlip)
					{
						dtMain.U8ErrSlip=0;
						DispErrorCode(dtMain.uError-1);
					}
					else{
						dtMain.U8ErrSlip=1;
						DispErrorCode(0xefff);
					}
				}
			}
			//Press Reset Key
			if(GetMsgKeyData()==KEY_ENTER&&GetMsgKeyCnt()>100)
			{
				if(gParcode.Item.WarnReset==2)
					SetErrorCode(0);
			}
		}
		else
		{
			if(dtSysTime.u810MS)
			{
				dtSysTime.u810MS=0;
                
				Do485Time10MS();
                
				if(dtMain.LHAInitTmr>2)
                {
                    ADCPanelUpdate();
                }

			}

			if(dtSysTime.u8100MS)
			{
				dtSysTime.u8100MS=0;
				
				MappMenu100MS();
                
                CalcTempValue();
                
				SetRxINTmpToFed(T1TempValue());
                
				SetRxINTmpToSub(T2TempValue());
                
                //CalcT1ShowTemp();    //计算显示温度
			}

			if(dtSysTime.u81000MS)
			{
				dtSysTime.u81000MS=0;
				DoMainCtrlLoop();
				MenuTime1000MS();
                
				if(dtMain.LHAInitTmr<10)
					dtMain.LHAInitTmr++;
                
                ADCData.LB_Time_Tick++;
			}
		}
	}
}
/************************************************************
*function 	: MappTimerCnt
*description	: It supplys the times to system application.
*On Entry : 2ms
*On Exit		: none
************************************************************/
void MappTimeCnt(void)
{
	if(dtSysTime.u8Tm1Cnt<4)
		dtSysTime.u8Tm1Cnt++;
	else{
		dtSysTime.u8Tm1Cnt=0;
		dtSysTime.u810MS =1;
		if(dtSysTime.u8Tm2Cnt<9)
			dtSysTime.u8Tm2Cnt++;
		else{
			dtSysTime.u8Tm2Cnt=0; 
			dtSysTime.u8100MS =1;
			if(dtSysTime.u8Tm3Cnt<9)
				dtSysTime.u8Tm3Cnt++;
			else{
				dtSysTime.u8Tm3Cnt=0;
				dtSysTime.u81000MS =1;
			}
		}
	}
}

/************************************************************
*function 	:MappPowerOn
*description	: Set the setting parameter for default value.
*On Entry : none
*On Exit		: none
************************************************************/
void MappPowerOn(void)
{
	switch(dtMain.U8InitStp)
	{
		case 0:
			if(dtMain.U8InitDly>0)
				dtMain.U8InitDly--;
			else{
				MappMenu_Init();

			//Test Function Define...
				//SetParaDefault();
				dtMain.U8InitStp=1;
			}
			break;
		case 1:
		{
			uint16_t tmpdata;
			
			ReadCodeAtRom0(gParcode.u32Buff);           //从FLASH中读取数据赋值给 gParcode.u32Buff

			tmpdata=GetParaChkSum(gParcode.u16Buff);

			if(tmpdata==gParcode.Item.uCheckSum0)
			{
				dtMain.U8InitStp=2;
			}
			else
			{
				dtMain.U8InitStp=3;
			}
		}
			break;
		case 2:
		{
			uint16_t ui;
			ParCode tmpcode;
			
			ReadCodeAtRom1(tmpcode.u32Buff);

			for(ui=0;ui<9;ui++)
			{
				if(gParcode.u32Buff[ui]!=tmpcode.u32Buff[ui])
					break;
			}

			if(ui<9)
			{
				SaveCodeAtRom1(gParcode.u32Buff);
			}
			dtMain.U8InitStp=4;
		}
			break;
		case 3:
		{
			uint16_t tmpdata;
			
			ReadCodeAtRom1(gParcode.u32Buff);

			tmpdata=GetParaChkSum(gParcode.u16Buff);

			if(tmpdata==gParcode.Item.uCheckSum0)
			{
				SaveCodeAtRom0(gParcode.u32Buff);

				dtMain.U8InitStp=4;
			}
			else
			{
				SetErrorCode(1);

				dtMain.U8InitStp=11;
			}
			
		}
			break;
		case 4:
			NVICConfiguration();
			dtMain.U8InitStp++;
			break;
		case 5:
			dtMain.U8InitStp++;
			break;
		case 6:
			dtMain.U8InitStp++;
			break;
		case 7:
			SetCodeInitial();
			dtMain.U8InitStp++;
			break;
		case 8:
			dtMain.U8InitStp++;
			break;
		case 9:
			dtMain.U8InitStp++;
			break;
		case 10:
			dtMain.U8InitStp++;
			break;
		case 11:
			LVDConfiguration();
			dtMain.U8InitStp++;
			break;
		case 12:
			LHA7918_Init();
			dtMain.U8PwrOn=1;
			dtMain.U8InitStp++;
			dtMain.LHAInitTmr=0;
			break;
		default:
			break;
	}
}
/************************************************************
*function 	: SetErrorCode
*description	: Set the setting parameter for default value.
*On Entry : Error Code
*On Exit		: none
************************************************************/
void SetErrorCode(uint16_t uError)
{
	dtMain.uError = uError;

	if(dtMain.uError>0)
	{
		uint16_t BitError,BakupOut;
		
		DispErrorCode(uError-1);

		BakupOut = GetOutputFlag();
		
		SetOutputSwitch(0);

		SetAlamFlag(1);
		
		//ERR-1 温度上限报警
		if(dtMain.uError==2)
		{
			BitError = WarnOutSwTab[gParcode.Item.WarnSwitch]/1000;
			
			if(BitError>0)
				P_RELAY1_H;
		}//ERR-2 温度下限报警
		else if(dtMain.uError==3)
		{
			BitError = (WarnOutSwTab[gParcode.Item.WarnSwitch]%1000)/100;
			
			if(BitError>0)
				P_RELAY1_H;
		}//ERR-3 温度偏差上限报警
		else if(dtMain.uError==4)
		{
			BitError = (WarnOutSwTab[gParcode.Item.WarnSwitch]%100)/10;
			
			if(BitError>0)
				P_RELAY1_H;
		}//ERR-4 温度偏差下限报警
		else if(dtMain.uError==5)
		{
			BitError = WarnOutSwTab[gParcode.Item.WarnSwitch]%10;
			
			if(BitError>0)
				P_RELAY1_H;
		}//ERR-5/6 传感器断线报警/通信超时报警
		else if(dtMain.uError==6||dtMain.uError==7)
		{
			if(BakupOut>0)
			{
				if(gParcode.Item.HeatOutput==2&&gParcode.Item.CtrlStyle==1)
				{
					SetOutpDACxV(gParcode.Item.HeatAnlgErr*10);
				}

				if(gParcode.Item.CoolOutput==2&&gParcode.Item.CtrlStyle==2)
				{
					SetOutpDACxV(gParcode.Item.CoolAnlgErr*10);
				}
			}
		}
	}
	else
	{
		P_RELAY1_L;

		SetAlamFlag(0);
		
		dtMain.uReset = 0;
	}
}

uint16_t GetErrorCode(void)
{
	return dtMain.uError;
}

void ResetErrCode(void)
{
	dtMain.uReset = 1;
}

/******************************************************************************
*Function 		: void TMR0_IRQHandler(ui08 on)
*Description	: it sets as a regular time:
							: 1.0.5 ms Handler
*******************************************************************************/
void SysTick_Handler(void)
{
	//dtSysTime.uTimeTick++;
    dtSysTime.u250usTick++;
}
/***********************************************************************************************************************
* Function Name: LVI_IRQHandler
* Description  : LVD Handler
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void LVI_IRQHandler(void)
{
	SaveParaToRom();
}

