/******************************************************************************
* @file  MappCode.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the Code firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 
#include "MappCode.h"

ParCode gParcode;

Encryption gEncryption;
/************************************************************
*function		: ParaCodeTAB
*description	: parameter code table.
*On Entry	: default value & min/max value.
*On Exit		: none
************************************************************/
int16_t ParaCodeTAB[][4]={
//default//Min//Max  //Flag
	{1, 		1,		6,		0x00},//F-01
	{2,			1, 		3,		0x00},//F-02
#if(TempCtrlType==TempCtrlWF96||TempCtrlType==TempCtrlWF96R||\
		TempCtrlType==TempCtrlWF49||TempCtrlType==TempCtrlWF49R)
	{1, 		1,		2,		0x00},//F-03
#else
	{1, 		1,		1,		0x00},//F-03
#endif
	{1, 		1,		2,		0x00},//F-04
	{1, 		1,		11,		0x00},//F-05
	{1300, -200,	1800,	0x00},//F-06
	{-200, -200,	1800,	0x00},//F-07
	{25, 		-200,	1800,	0x00},//F-08
	{1999,	-199, 1999,	0x00},//F-09
	{-199, 	-199,	1999,	0x00},//F-10
	{999, 	-999,	999,	0x00},//F-11
	{-999, 	-999,	999,	0x00},//F-12
	{1, 		1,		2,		0x00},//F-13
	{8,			0, 		15,		0x00},//F-14
	{1, 		1,		3,		0x00},//F-15
	{3, 		2,		50,		0x00},//F-16
//----------------------------------------
#if(TempCtrlType==TempCtrlWF48)
	{1, 		1,		2,		0x00},//F-20
#elif (TempCtrlType==TempCtrlWF48C)
	{3, 		3,		3,		0x00},//F-20
#else
	{1, 		1,		3,		0x00},//F-20
#endif
	{2,			2, 		120,	0x00},//F-21
	{100, 	1,		500,	0x00},//F-22
	{100, 	1,		500,	0x00},//F-23
	{10, 		2,		10,		0x00},//F-24
	{0, 		0,		7,		0x00},//F-25
	{0, 		0,		10,		0x00},//F-26
//----------------------------------------
#if(TempCtrlType==TempCtrlWF48)
	{1, 		1,		2,		0x00},//F-30
#elif (TempCtrlType==TempCtrlWF48C)
	{3, 		3,		3,		0x00},//F-30
#else
	{3, 		1,		3,		0x00},//F-30
#endif
	{2, 		2,		120,	0x00},//F-31
	{100, 	1,		500,	0x00},//F-32
	{100, 	1,		500,	0x00},//F-33
	{10,		2,		10, 	0x00},//F-34
	{0, 		0,		7,		0x00},//F-35
	{0, 		0,		10, 	0x00},//F-36
//----------------------------------------
#if(TempCtrlType==TempCtrlWF96||TempCtrlType==TempCtrlWF96R||\
		TempCtrlType==TempCtrlWF49||TempCtrlType==TempCtrlWF49R)
	{1,			1, 		2,		0x00},//F-40
#else
	{1,			1, 		1,		0x00},//F-40
#endif
	{1000,	-199, 1999,	0x00},//F-41
//----------------------------------------
	{0, 		-500,	500,	0x01},//F-50
	{1, 		1,		2,		0x00},//F-51
//----------------------------------------
	{1,			1, 		247,	0x00},//F-60
	{1, 		1,		6,		0x00},//F-61
	{1, 		1,		6,		0x00},//F-62
	{1, 		1,		2,		0x00},//F-63
	{50, 		0,		100,	0x01},//F-64
//----------------------------------------
	{1,			1, 		2,		0x00},//F-70
	{SofeWareVer, 1,100,0x12},//F-71
	{MenuVersion, 1,100,0x12},//F-72
//----------------------------------------
	{1000,	1, 		9999,	0x00},//F-80
	{10, 		1,		9999,	0x00},//F-81
	{999, 	1,		9999,	0x00},//F-82
	{1000,	1, 		9999,	0x00},//F-83
	{10, 		1,		9999,	0x00},//F-84
	{999, 	1,		9999,	0x00},//F-85
	{1000,	1, 		9999,	0x00},//F-86
	{0, 		1,		9999,	0x00},//F-87
	{999, 	1,		9999,	0x00},//F-88
	{1000,	1, 		9999,	0x00},//F-89
	{10, 		1,		9999,	0x00},//F-90
	{999, 	1,		9999,	0x00},//F-91
};
/************************************************************
*function		: WarnOutSwTab
************************************************************/

const int16_t WarnOutSwTab[16] = {
	0,1,10,11,100,101,110,111,1000,
	1001,1010,1011,1100,1101,1110,1111
};

/************************************************************
*function		: GetDotBitno
*description	: get data flag if it has dots?.
*On Entry	: item.
*On Exit		: dot bits
************************************************************/
int16_t GetDotBitno(uint16_t item)
{
	return (ParaCodeTAB[item][3]&0x0f);
}
/************************************************************
*function		: GetIfRWFlag
*description	: get data flag if it can writes?.
*On Entry	: item.
*On Exit		: r/w flag
************************************************************/
int16_t GetIfRWFlag(uint16_t item)
{
	return (ParaCodeTAB[item][3]&0x10);
}
/************************************************************
*function		: GetMaxPara
*description	: get data max limit.
*On Entry	: item.
*On Exit		: max data
************************************************************/
int16_t GetMaxPara(uint16_t item)
{
	return ParaCodeTAB[item][2];
}
/************************************************************
*function		: GetMinPara
*description	: get data min limit.
*On Entry	: item.
*On Exit		: min data
************************************************************/
int16_t GetMinPara(uint16_t item)
{
	return ParaCodeTAB[item][1];
}
/************************************************************
*function		: GetMenuIndex
*description	: get the parcode index.
*On Entry	: uaddr.
*On Exit		: index
************************************************************/
uint16_t GetMenuIndex(uint16_t uaddr)
{
	uint16_t tmpAddr;
	
	if(uaddr>=1&&uaddr<=16)
	{
		tmpAddr = uaddr - 1;
	}
	else if(uaddr>=20&&uaddr<=26)
	{
		tmpAddr = uaddr-20+emHeatOutput;
	}
	else if(uaddr>=30&&uaddr<=36)
	{
		tmpAddr = uaddr-30+emCoolOutput;
	}
	else if(uaddr>=40&&uaddr<=41)
	{
		tmpAddr = uaddr-40+emCascadeSwon;
	}
	else if(uaddr>=50&&uaddr<=51)
	{
		tmpAddr = uaddr-50+emErrCorrectedV;
	}
	else if(uaddr>=60&&uaddr<=64)
	{
		tmpAddr = uaddr-60+emuCommAddr;
	}
	else if(uaddr>=70&&uaddr<=73)
	{
		tmpAddr = uaddr-70+emuRstDefault;
	}
	else if(uaddr>=80&&uaddr<=91)
	{
		tmpAddr = uaddr-80+emqPID01Kp;
	}
	else //error address..
	{
		tmpAddr = 0xffff;
	}
	
	return tmpAddr;
}
/************************************************************
*function		: SetCodeDefault
*description	: set parameter code to default value.
*On Entry	: none.
*On Exit		: noe
************************************************************/
void SetCodeDefault(void)
{
	uint16_t ui;
	
	for(ui=0;ui<ParmMaxItem;ui++)
		gParcode.u16Buff[ui]=ParaCodeTAB[ui][0];
}
/******************************************************************************
*Function 		: void ReadCodeAtRom0(void)
*Description	: It reads datas from flash:
							: 1.parameter code
*******************************************************************************/
void ReadCodeAtRom0(uint32_t *u32Buff)
{
	uint16_t	ui;
	uint32_t 	address = FMC_Address0;

	for(ui=0;ui<ParmMaxBuff;ui++,address+=4)
	{
		u32Buff[ui] = *(__IO uint32_t *)address;
	}
}
void SaveCodeAtRom0(uint32_t *u32Buff)
{
	FMC_ErasePage(FMC_Address0);
	FMC_Program(FMC_Address0,ParmMaxBuff,u32Buff);
}

/******************************************************************************
*Function 		: void ReadCodeAtRom1(void)
*Description	: It reads datas from flash:
							: 1.parameter code
*******************************************************************************/
void ReadCodeAtRom1(uint32_t *u32Buff)
{
	uint16_t	ui;
	uint32_t 	address = FMC_Address1;

	for(ui=0;ui<ParmMaxBuff;ui++,address+=4)
	{
		u32Buff[ui] = *(__IO uint32_t *)address;
	}
}

void SaveCodeAtRom1(uint32_t *u32Buff)
{
	FMC_ErasePage(FMC_Address1);
	
	FMC_Program(FMC_Address1,ParmMaxBuff,u32Buff);
}

/******************************************************************************
*Function 		: void InitPar2Code(void)
*Description	: It reads datas from flash:
							: 1.parameter code
*******************************************************************************/
void ReadCHIPID0Code(void)
{
	gEncryption.u32Buff[0]=FMC_Read(0x00500894);
	gEncryption.u32Buff[1]=FMC_Read(0x00500898);
	gEncryption.u32Buff[2]=FMC_Read(0x0050089C);
}

void ReadCHIPID1Code(void)
{
	gEncryption.u32Buff[3]=*(__IO uint32_t *)(FMC_Address2+12);
	gEncryption.u32Buff[4]=*(__IO uint32_t *)(FMC_Address2+16);
	gEncryption.u32Buff[5]=*(__IO uint32_t *)(FMC_Address2+20);
	gEncryption.u32Buff[6]=*(__IO uint32_t *)(FMC_Address2+24);
}

/******************************************************************************
*Function 		: void SavePar2Code(void)
*Description	: It seves datas to data flash:
							: 1.parameter code
*******************************************************************************/
void SaveCHIPID1Code(void)
{
	FMC_ErasePage(FMC_Address2);

	FMC_Program(FMC_Address2,7,gEncryption.u32Buff);
}

/******************************************************************************
*Function 		: void GetPassWord(void)
*Description	: It seves datas to data flash:
							: 1.parameter code
*******************************************************************************/
uint16_t GetPassWord(uint16_t id,uint16_t pw)
{
	uint16_t uchgpw;

	switch(id)
	{
		case 1:
			uchgpw = (pw-135)/13;
			break;
		case 2:
			uchgpw = (pw-579)/6;
			break;
		case 3:
			uchgpw = pw - 34579;
			break;
		case 4:
			uchgpw = pw - 24680;
			break;
		case 5:
			uchgpw = pw - 13579;
			break;
		case 6:
			uchgpw = pw + 23679;
			break;
		case 7:
			uchgpw = pw + 35791;
			break;
		default:
			uchgpw = 12345;
			break;
	}

	return uchgpw;
}


// added at 2023.04.18
void GetEncryptionCode(void)
{
	uint16_t chkid;
	
	chkid = gEncryption.Item.tPassWordt[6]&0x000f;
	gEncryption.Item.tPassWordc[0] = GetPassWord(chkid,gEncryption.Item.tPassWordt[0]);

	chkid = (gEncryption.Item.tPassWordt[6]>>4)&0x000f;
	gEncryption.Item.tPassWordc[1] = GetPassWord(chkid,gEncryption.Item.tPassWordt[1]);

	chkid = (gEncryption.Item.tPassWordt[6]>>8)&0x000f;
	gEncryption.Item.tPassWordc[2] = GetPassWord(chkid,gEncryption.Item.tPassWordt[2]);

	chkid = (gEncryption.Item.tPassWordt[6]>>12)&0x000f;
	gEncryption.Item.tPassWordc[3] = GetPassWord(chkid,gEncryption.Item.tPassWordt[3]);

	chkid = gEncryption.Item.tPassWordt[7]&0x000f;
	gEncryption.Item.tPassWordc[4] = GetPassWord(chkid,gEncryption.Item.tPassWordt[4]);

	chkid = (gEncryption.Item.tPassWordt[7]>>4)&0x000f;
	gEncryption.Item.tPassWordc[5] = GetPassWord(chkid,gEncryption.Item.tPassWordt[5]);
}

/******************************************************************************
*Function 		: void CheckPassWord(void)
*Description	: it check password if correct:
							: 1.on or off parameter
*******************************************************************************/
uint16_t CheckPassWord(void)
{
	uint16_t iError = 0;

	GetEncryptionCode();
		
	if(gEncryption.Item.tPassWordr[0]!=gEncryption.Item.tPassWordc[0])
		iError++;

	if(gEncryption.Item.tPassWordr[1]!=gEncryption.Item.tPassWordc[1])
		iError++;

	if(gEncryption.Item.tPassWordr[2]!=gEncryption.Item.tPassWordc[2])
		iError++;

	if(gEncryption.Item.tPassWordr[3]!=gEncryption.Item.tPassWordc[3])
		iError++;

	if(gEncryption.Item.tPassWordr[4]!=gEncryption.Item.tPassWordc[4])
		iError++;

	if(gEncryption.Item.tPassWordr[5]!=gEncryption.Item.tPassWordc[5])
		iError++;

	return iError;
}

