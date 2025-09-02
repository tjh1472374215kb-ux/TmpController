/******************************************************************************
* @file  MappMenu.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the Menu firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "MappMain.h"
#include "MappMenu.h"
#include "MappShow.h"
#include "MappCode.h"
#include "MappComm.h"

#include "MCtrlMain.h"

MainMenu stMenu;

#include "MappMSet.c"
#include "MappMSub.c"

/************************************************************
*function		: MappMainMenu
*description	: deal with the key event and parameter setting.
*On Entry	: key value
*On Exit		: none
************************************************************/
void MappMainMenu(void)
{
	 //no key in.
	if(!GetMsgIfKeyIn()) 
		return;
	
	//waiting for time.
	if(stMenu.u8Wait1S) 
		return;
	
	switch(stMenu.u8MenuID){
		case MENU_MAINSHOW:
			Menu_MainShow();
			break;
		case MENU_MAINPSWD:
			Menu_MainPswd();
			break;
		case MENU_MAINFUNC:
			Menu_MainFunc();
			break;
		case MENU_SUBFUNC:
			Menu_SubFunc();
			break;
		default:
			break;
	}
}
/************************************************************
*function		: Menu_MainShow
*description	: the menu for main show events
*On Entry	: key value
*On Exit		: none
************************************************************/
void Menu_MainShow(void)
{
	switch(GetMsgKeyData()){
		case KEY_MODE:
			if(GetMsgKeyCnt()>LongKey_50)
			{
				stMenu.u8MenuID=MENU_MAINPSWD;
				stMenu.u16Pswd=100;
				stMenu.u8Timeout = 20;
				DispMenuPswd(100);
			}
			break;
		case KEY_UP:
			//run for InnerTemp...
			if(GetInnerTempFlag()>0)
				break;
			
			if(gParcode.Item.TmpSetLock==2)
			{
				long temSet,temMax;
					
				stMenu.TempFlag = 1;
				stMenu.TempTmr = 60;

				temSet = gParcode.Item.TempSetVal;
				
				if(GetMsgKeyCnt()>LongKey_50)
				{
					temSet+=500;
				}
				else if(GetMsgKeyCnt()>LongKey_05)
				{
					temSet+=50;
				}
				else
				{
					if(temSet<10000)
						temSet+=5;
					else
						temSet+=10;
				}

				if(gParcode.Item.TemperatxUnit==2)
				{
					temMax = gParcode.Item.TmpUpLimit*18+320;

					if(temSet>temMax)
						temSet=temMax;

					gParcode.Item.TempSetVal = temSet;

					temSet = (temSet-320)*5/9;
				}
				else
				{
					temMax=gParcode.Item.TmpUpLimit*10;

					if(temSet>temMax)
						temSet=temMax;

					gParcode.Item.TempSetVal = temSet;
				}
				
				SetOutpTmpToRef(temSet);
				DispShowFrc2(gParcode.Item.TempSetVal,1);
			}
			else if(stMenu.LockFlag > 0)
			{
				stMenu.LockTmr=60;
				
				if(GetMsgKeyCnt()>LongKey_50)
				{
					stMenu.LockNum +=100;
				}
				else if(GetMsgKeyCnt()>LongKey_05)
				{
					stMenu.LockNum +=10;
				}
				else
				{
					stMenu.LockNum ++;
				}

				if(stMenu.LockNum>9999)
					stMenu.LockNum=9999;
				
				if(stMenu.LockHTmr<20)
					DispMenuNum1(stMenu.LockNum);
			}
			else
			{
				if(stMenu.LockNum==0)
				{
					long temSet,temMax;
					
					stMenu.TempFlag = 1;
					stMenu.TempTmr = 60;
					stMenu.LockTmr = 60;
					
					temSet = gParcode.Item.TempSetVal;
				
					if(GetMsgKeyCnt()>LongKey_50)
					{
						temSet+=500;
					}
					else if(GetMsgKeyCnt()>LongKey_05)
					{
						temSet+=50;
					}
					else
					{
						if(temSet<10000)
							temSet+=5;
						else
							temSet+=10;
					}

					if(gParcode.Item.TemperatxUnit==2)
					{
						temMax = gParcode.Item.TmpUpLimit*18+320;

						if(temSet>temMax)
							temSet=temMax;

						gParcode.Item.TempSetVal = temSet;

						temSet = (temSet-320)*5/9;
					}
					else
					{
						temMax=gParcode.Item.TmpUpLimit*10;

						if(temSet>temMax)
							temSet=temMax;

						gParcode.Item.TempSetVal = temSet;
					}
					
					SetOutpTmpToRef(temSet);
					if(stMenu.TempHTmr<20)
						DispShowFrc2(gParcode.Item.TempSetVal,1);
				}
				else
				{
					stMenu.LockFlag = 1;
					stMenu.LockNum =123;
					stMenu.LockTmr = 60;
					DispMenuNum1(stMenu.LockNum);
				}
			}
			break;
		case KEY_DOWN:
			//run for InnerTemp...
			if(GetInnerTempFlag()>0)
				break;
			
			if(gParcode.Item.TmpSetLock==2)
			{
				long temSet,temMin;
					
				stMenu.TempFlag = 1;
				stMenu.TempTmr = 60;

				if(GetMsgKeyCnt()>LongKey_50)
				{
					if(gParcode.Item.TempSetVal>-1500)
						gParcode.Item.TempSetVal-=500;
				}
				else if(GetMsgKeyCnt()>LongKey_05){
					if(gParcode.Item.TempSetVal>-1950)
						gParcode.Item.TempSetVal-=50;
				}
				else
				{
					if(gParcode.Item.TempSetVal>-1000)
						gParcode.Item.TempSetVal-=5;
					else
						gParcode.Item.TempSetVal-=10;
				}

				temSet = gParcode.Item.TempSetVal;
				
				if(gParcode.Item.TemperatxUnit==2)
				{
					temMin = gParcode.Item.TmpDnLimit*18+320;

					if(temSet<temMin)
						temSet=temMin;

					gParcode.Item.TempSetVal = temSet;

					temSet = (temSet-320)*5/9;
				}
				else
				{
					temMin=gParcode.Item.TmpDnLimit*10;

					if(temSet<temMin)
						temSet=temMin;

					gParcode.Item.TempSetVal = temSet;
				}
					
				SetOutpTmpToRef(temSet);
				DispShowFrc2(gParcode.Item.TempSetVal,1);
			}
			else if(stMenu.LockFlag > 0)
			{
				stMenu.LockTmr=60;
				
				if(GetMsgKeyCnt()>LongKey_50)
				{
					if(stMenu.LockNum>100)
						stMenu.LockNum -=100;
					else
						stMenu.LockNum = 0;
				}
				else if(GetMsgKeyCnt()>LongKey_05)
				{
					if(stMenu.LockNum>10)
						stMenu.LockNum -=10;
					else
						stMenu.LockNum = 0;
				}
				else
				{
					if(stMenu.LockNum>0)
						stMenu.LockNum --;
				}
				
				if(stMenu.LockHTmr<20)
					DispMenuNum1(stMenu.LockNum);
			}
			else
			{
				if(stMenu.LockNum==0)
				{
					long temSet,temMin;
					
					stMenu.TempFlag = 1;
					stMenu.TempTmr = 60;
					stMenu.LockTmr = 0;
					
					if(GetMsgKeyCnt()>LongKey_50)
					{
						if(gParcode.Item.TempSetVal>-1500)
							gParcode.Item.TempSetVal-=500;
					}
					else if(GetMsgKeyCnt()>LongKey_05)
					{
						if(gParcode.Item.TempSetVal>-1950)
							gParcode.Item.TempSetVal-=50;
					}
					else
					{
						if(gParcode.Item.TempSetVal>-1000)
							gParcode.Item.TempSetVal-=5;
						else
							gParcode.Item.TempSetVal-=10;
					}
				
					temSet = gParcode.Item.TempSetVal;
				
					if(gParcode.Item.TemperatxUnit==2)
					{
						temMin = gParcode.Item.TmpDnLimit*18+320;

						if(temSet<temMin)
							temSet=temMin;

						gParcode.Item.TempSetVal = temSet;

						temSet = (temSet-320)*5/9;
					}
					else
					{
						temMin=gParcode.Item.TmpDnLimit*10;

						if(temSet<temMin)
							temSet=temMin;

						gParcode.Item.TempSetVal = temSet;
					}
						
					SetOutpTmpToRef(temSet);
					if(stMenu.TempHTmr<20)
						DispShowFrc2(gParcode.Item.TempSetVal,1);
				}
				else
				{
					stMenu.LockFlag = 1;
					stMenu.LockNum =123;
					stMenu.LockTmr = 60;
					DispMenuNum1(stMenu.LockNum);
				}
			}
			break;
		case KEY_ENTER:
			if(stMenu.LockFlag>0&&stMenu.LockNum==0)
			{
				stMenu.LockFlag = 0;

				stMenu.TempFlag = 1;
				stMenu.TempTmr = 60;
				
				DispMenuNum1(stMenu.LockNum);
				
				//SetOutpTmpToRef(gParcode.Item.TempSetVal);
				DispShowFrc2(gParcode.Item.TempSetVal,1);
			}
			else
			{	//Manual Control
				if(gParcode.Item.XRunSwitch==1&&gParcode.Item.CtrlMethod==2)
				{
					if(GetOutputFlag()>0)
						SetOutputSwitch(0);
					else
						SetOutputSwitch(1);
				}
			}
			break;
		default:
			break;
	}
}
/************************************************************
*function		: Menu_MainShow
*description	: the menu for main show events
*On Entry	: key value
*On Exit		: none
************************************************************/
void Menu_MainPswd(void)
{
	stMenu.u8Timeout = 10;
	
	switch(GetMsgKeyData())
	{
		case KEY_MODE:
			if(GetMsgKeyCnt()==3){
				stMenu.u8Timeout = 0;
				stMenu.u8MenuID=MENU_MAINSHOW;
			}
			break;
		case KEY_ENTER:
			//check if real password?
			if(stMenu.u16Pswd==123)
			{
				stMenu.u8MenuID=MENU_MAINFUNC;
				stMenu.u8FuncID = 0;
				DispMenuFunc(stMenu.u8FuncID);
			}
			else if(stMenu.u16Pswd==789)
			{
				stMenu.u8MenuID=MENU_MAINFUNC;
				stMenu.u8FuncID = emqPID01Kp;
				DispMenuFunc(stMenu.u8FuncID);
			}
			break;
		case KEY_UP:
			if(GetMsgKeyCnt()>LongKey_05){
				if(stMenu.u16Pswd<989)
					stMenu.u16Pswd+=10;
				else
					stMenu.u16Pswd=999;
			}
			else{
				if(stMenu.u16Pswd<999)
					stMenu.u16Pswd++;
				else
					stMenu.u16Pswd=999;
			}
			DispMenuPswd(stMenu.u16Pswd);
			break;
		case KEY_DOWN:
			if(GetMsgKeyCnt()>LongKey_05){
				if(stMenu.u16Pswd>10)
					stMenu.u16Pswd-=10;
				else
					stMenu.u16Pswd=0;
			}
			else{
				if(stMenu.u16Pswd>0)
					stMenu.u16Pswd--;
			}
			DispMenuPswd(stMenu.u16Pswd);
			break;
		default:
			break;
	}
}
/************************************************************
*function 	: Menu_MainFunc
*description	: the menu for function item show event
*On Entry : key value
*On Exit		: none
************************************************************/
void Menu_MainFunc(void)
{
	stMenu.u8Timeout = 10;
	
	switch(GetMsgKeyData()){
		case KEY_MODE:
			if(GetMsgKeyCnt()!=3) break;
			stMenu.u8Timeout = 0;
			stMenu.u8MenuID=MENU_MAINSHOW;
			break;
		case KEY_ENTER:
			if(GetMsgKeyCnt()!=3) break;
			
			stMenu.u8MenuID=MENU_SUBFUNC;
		
			stMenu.u16Para = gParcode.u16Buff[stMenu.u8FuncID];

			stMenu.u8DatDot = GetDotBitno(stMenu.u8FuncID);
			stMenu.u16DatMax = GetMaxPara(stMenu.u8FuncID);
			stMenu.u16DatMin = GetMinPara(stMenu.u8FuncID);

			if(stMenu.u8FuncID==emInnerTemp)
			{
				stMenu.u16DatMin=gParcode.u16Buff[emTmpDnLimit];
				stMenu.u16DatMax=gParcode.u16Buff[emTmpUpLimit];
			}
			else if(stMenu.u8FuncID==emTmpUpLimit)
			{
				if(stMenu.u16DatMin<gParcode.u16Buff[emTmpDnLimit])
					stMenu.u16DatMin=gParcode.u16Buff[emTmpDnLimit];
			}
			else if(stMenu.u8FuncID==emTmpDnLimit)
			{
				if(stMenu.u16DatMax>gParcode.u16Buff[emTmpUpLimit])
					stMenu.u16DatMax=gParcode.u16Buff[emTmpUpLimit];
			}
			else if(stMenu.u8FuncID==emUpLmtWarn)
			{
				if(stMenu.u16DatMin<gParcode.u16Buff[emDnLmtWarn])
					stMenu.u16DatMin=gParcode.u16Buff[emDnLmtWarn];
			}
			else if(stMenu.u8FuncID==emDnLmtWarn)
			{
				if(stMenu.u16DatMax>gParcode.u16Buff[emUpLmtWarn])
					stMenu.u16DatMax=gParcode.u16Buff[emUpLmtWarn];
			}
			else if(stMenu.u8FuncID==emUpErrWarn)
			{
				if(stMenu.u16DatMin<gParcode.u16Buff[emDnErrWarn])
					stMenu.u16DatMin=gParcode.u16Buff[emDnErrWarn];
			}
			else if(stMenu.u8FuncID==emDnErrWarn)
			{
				if(stMenu.u16DatMax>gParcode.u16Buff[emUpErrWarn])
					stMenu.u16DatMax=gParcode.u16Buff[emUpErrWarn];
			}
			else if(stMenu.u8FuncID==emHeatAnlgMax)
			{
				if(stMenu.u16DatMin<gParcode.u16Buff[emHeatAnlgMin])
					stMenu.u16DatMin=gParcode.u16Buff[emHeatAnlgMin];
			}
			else if(stMenu.u8FuncID==emHeatAnlgMin)
			{
				if(stMenu.u16DatMax>gParcode.u16Buff[emHeatAnlgMax])
					stMenu.u16DatMax=gParcode.u16Buff[emHeatAnlgMax];
			}
			else if(stMenu.u8FuncID==emCoolAnlgMax)
			{
				if(stMenu.u16DatMin<gParcode.u16Buff[emCoolAnlgMin])
					stMenu.u16DatMin=gParcode.u16Buff[emCoolAnlgMin];
			}
			else if(stMenu.u8FuncID==emCoolAnlgMin)
			{
				if(stMenu.u16DatMax>gParcode.u16Buff[emCoolAnlgMax])
					stMenu.u16DatMax=gParcode.u16Buff[emCoolAnlgMax];
			}

			if(stMenu.u8FuncID==emWarnSwitch)
				DispMenuNum2(WarnOutSwTab[stMenu.u16Para]);
			else
				DispShowFrc2(stMenu.u16Para,stMenu.u8DatDot);
			break;
		case KEY_UP:
			if(stMenu.u16Pswd==123)
			{
				if(stMenu.u8FuncID<emuMnVertion)
					stMenu.u8FuncID++;
			}
			else if(stMenu.u16Pswd==789)
			{
				if(stMenu.u8FuncID<emqPID12Kd)
					stMenu.u8FuncID++;
			}
			DispMenuFunc(stMenu.u8FuncID);
			break;
		case KEY_DOWN:
			if(stMenu.u16Pswd==123)
			{
				if(stMenu.u8FuncID>0)
					stMenu.u8FuncID--;
			}
			else if(stMenu.u16Pswd==789)
			{
				if(stMenu.u8FuncID>emqPID01Kp)
					stMenu.u8FuncID--;
			}
			DispMenuFunc(stMenu.u8FuncID);
			break;
		default:
			break;
	}
}
/************************************************************
*function		: Menu_SubFunc
*description	: the menu for parameter setting event
*On Entry	: key value
*On Exit		: none
************************************************************/
void Menu_SubFunc(void)
{
	stMenu.u8Timeout = 10;
	
	switch(GetMsgKeyData()){
		case KEY_MODE:
			stMenu.u8MenuID=MENU_MAINFUNC;
			DispMenuFunc(stMenu.u8FuncID);
			break;
		case KEY_ENTER:
			if(GetMsgKeyCnt()!=3) break;
			
			stMenu.u8Wait1S = 10;
		  stMenu.uSetError = 0;
		
			SetParameterCode(stMenu.u8FuncID);

			if(stMenu.uSetError)
				DispMenuEnd(0);
			else
				DispMenuEnd(1);

			if(stMenu.u16Pswd==123)
			{
				if(stMenu.u8FuncID<emuMnVertion)
					stMenu.u8FuncID++;
			}
			else if(stMenu.u16Pswd==789)
			{
				if(stMenu.u8FuncID<emqPID12Kd)
					stMenu.u8FuncID++;
			}
			break;
		case KEY_UP:
			if(GetIfRWFlag(stMenu.u8FuncID)) 
				break;

			if(GetMsgKeyCnt()>LongKey_50)
			{
				stMenu.u16Para+=100;
			}
			else if(GetMsgKeyCnt()>LongKey_05)
			{
				stMenu.u16Para+=10;
			}
			else
			{
				stMenu.u16Para++;
			}

			if(stMenu.u16Para>stMenu.u16DatMax)
				stMenu.u16Para=stMenu.u16DatMax;

			if(stMenu.u8FuncID==emWarnSwitch)
				DispMenuNum2(WarnOutSwTab[stMenu.u16Para]);
			else
				DispShowFrc2(stMenu.u16Para,stMenu.u8DatDot);
			break;
		case KEY_DOWN:
			if(GetIfRWFlag(stMenu.u8FuncID)) 
				break;

			if(GetMsgKeyCnt()>LongKey_50)
			{
				if(stMenu.u16Para>(stMenu.u16DatMin+100))
					stMenu.u16Para-=100;
				else
					stMenu.u16Para = stMenu.u16DatMin;
			}
			else if(GetMsgKeyCnt()>LongKey_05){
				if(stMenu.u16Para>(stMenu.u16DatMin+10))
					stMenu.u16Para-=10;
				else
					stMenu.u16Para = stMenu.u16DatMin;
			}
			else
			{
				if(stMenu.u16Para>stMenu.u16DatMin)
					stMenu.u16Para--;
			}

			if(stMenu.u16Para<stMenu.u16DatMin)
				stMenu.u16Para=stMenu.u16DatMin;
			
			if(stMenu.u8FuncID==emWarnSwitch)
				DispMenuNum2(WarnOutSwTab[stMenu.u16Para]);
			else
				DispShowFrc2(stMenu.u16Para,stMenu.u8DatDot);
			break;
		default:
 			break;
	}
}
/************************************************************
*function		: MenuTime100MS
*description	: supply 100ms for menu events.
*On Entry	: none
*On Exit		: none
************************************************************/
void MappMenu100MS(void)
{
	if(stMenu.u8Wait1S>0)
	{
		stMenu.u8Wait1S--;
		
		if(stMenu.u8Wait1S==0)
		{
			stMenu.u8MenuID=MENU_MAINFUNC;
			DispMenuFunc(stMenu.u8FuncID);
		}
	}

	if(stMenu.u8MenuID==1
		&&dtMain.uError==0)
	{
		if(stMenu.LockFlag>0)
		{
			if(stMenu.LockHTmr<20)
			{
				stMenu.LockHTmr++;
				if(stMenu.LockHTmr==20)
					stMenu.LockLTmr=0;

				DispMenuNum1(stMenu.LockNum);
			}
			else if(stMenu.LockLTmr<5)
			{
				stMenu.LockLTmr++;
				if(stMenu.LockLTmr==5)
					stMenu.LockHTmr=0;
				DispMenuOff1();
			}
		}
		else if(stMenu.TempFlag>0)
		{
			if(stMenu.TempHTmr<20)
			{
				stMenu.TempHTmr++;
				if(stMenu.TempHTmr==20)
					stMenu.TempLTmr=0;

				DispShowFrc2(gParcode.Item.TempSetVal,1);

				//added at 2025.07.30
				SetLockFlag(1);
			}
			else if(stMenu.TempLTmr<5)
			{
				stMenu.TempLTmr++;
				if(stMenu.TempLTmr==5)
					stMenu.TempHTmr=0;
				
				DispMenuOff2();

				//added at 2025.07.30
				SetLockFlag(0);
			}
		}
		else
		{
			if(gParcode.Item.TemperatxUnit==2)
			{
				long tmpdata;

				tmpdata = (long)tCtrlOutParm.GetTempT1Val*9/5 + 320;
				
				if(gParcode.Item.CtrlMethod==2&&
					 gParcode.Item.XRunSwitch==1&&
						GetOutputFlag()==0)
				{
					//added at 2025.07.30
					if(stMenu.StopHTmr<20)
					{
						stMenu.StopHTmr++;
						if(stMenu.StopHTmr==20)
							stMenu.StopLTmr=0;
						DispShowFrc1(tmpdata,1);
					}
					else if(stMenu.StopLTmr<5)
					{
						stMenu.StopLTmr++;
						if(stMenu.StopLTmr==5)
							stMenu.StopHTmr=0;
						
						DispMenuOff1();
					}
				}
				else
				{
					DispShowFrc1(tmpdata,1);
				}

				if(GetInnerTempFlag()>0)
				{
					tmpdata = (long)gParcode.Item.InnerTemp*18 + 320;

					//added at 2025.07.30
					if(stMenu.TempHTmr<20)
					{
						stMenu.TempHTmr++;
						if(stMenu.TempHTmr==20)
							stMenu.TempLTmr=0;
						DispShowFrc2(tmpdata,1);
					}
					else if(stMenu.TempLTmr<5)
					{
						stMenu.TempLTmr++;
						if(stMenu.TempLTmr==5)
							stMenu.TempHTmr=0;
						
						DispMenuOff2();
					}
				}
				else
				{
					DispShowFrc2(gParcode.Item.TempSetVal,1);
				}
			}
			else
			{
				if(gParcode.Item.CtrlMethod==2&&
					 gParcode.Item.XRunSwitch==1&&
						GetOutputFlag()==0)
				{
					//added at 2025.07.30
					if(stMenu.StopHTmr<20)
					{
						stMenu.StopHTmr++;
						if(stMenu.StopHTmr==20)
							stMenu.StopLTmr=0;
						DispShowFrc1(tCtrlOutParm.GetTempT1Val,1);
                        //DispShowFrc1(LHAData.T1ShowTemp,LHAData.T1ShowTempBit);
					}
					else if(stMenu.StopLTmr<5)
					{
						stMenu.StopLTmr++;
						if(stMenu.StopLTmr==5)
							stMenu.StopHTmr=0;
						
						DispMenuOff1();
					}
				}
				else
				{
					DispShowFrc1(tCtrlOutParm.GetTempT1Val,1);
                    //DispShowFrc1(LHAData.T1ShowTemp,LHAData.T1ShowTempBit);
				}

				if(GetInnerTempFlag()>0)
				{
					//added at 2025.07.30
					if(stMenu.TempHTmr<20)
					{
						stMenu.TempHTmr++;
						if(stMenu.TempHTmr==20)
							stMenu.TempLTmr=0;
						DispShowFrc2(gParcode.Item.InnerTemp*10,1);
					}
					else if(stMenu.TempLTmr<5)
					{
						stMenu.TempLTmr++;
						if(stMenu.TempLTmr==5)
							stMenu.TempHTmr=0;
						
						DispMenuOff2();
					}
				}
				else
				{
					//DispShowFrc2(gParcode.Item.TempSetVal,1);
                    DispShowFrc2(10*ADCData.ColdMesTemp,1);
				}
			}
		}
	}
}

/************************************************************
*function		: MenuTime1000MS
*description	: supply 1000ms for menu events.
*On Entry	: none
*On Exit		: none
************************************************************/
void MenuTime1000MS(void)
{
	if(stMenu.u8Timeout>0)
	{
		stMenu.u8Timeout--;
		
		if(stMenu.u8Timeout==0)
		{
			stMenu.u8MenuID=1;
		}
	}

	if(stMenu.LockTmr>0)
	{
		stMenu.LockTmr--;

		if(stMenu.LockTmr==0)
		{
			stMenu.LockFlag = 0;
			//added at 2025.07.30
			stMenu.LockNum =123;
			
			if(gParcode.Item.TemperatxUnit==2)
			{
				long tmpdata;
			
				tmpdata = (long)tCtrlOutParm.GetTempT1Val*9/5 + 320;

				DispShowFrc1(tmpdata,1);
			}
			else
			{
				DispShowFrc1(tCtrlOutParm.GetTempT1Val,1);
			}
		}
	}

	if(stMenu.TempTmr>0)
	{
		stMenu.TempTmr--;

		if(stMenu.TempTmr==0)
		{
			stMenu.TempFlag = 0;

			DispShowFrc2(gParcode.Item.TempSetVal,1);

			//added at 2025.07.30
			SetLockFlag(1);
		}
	}
}
/************************************************************
*function		: MappMenu_Init
*description	: initial data about menu parameters.
*On Entry	: none
*On Exit		: none
************************************************************/
void MappMenu_Init(void)
{
	stMenu.u8MenuID = 1;
	stMenu.u8FuncID = 0;
	stMenu.u8Timeout = 0;
	stMenu.u8Wait1S = 0;
	stMenu.u8DatSave = 0;
	stMenu.LockFlag = 0;
	stMenu.LockNum = 123;
}

