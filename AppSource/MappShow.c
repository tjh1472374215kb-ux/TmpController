/******************************************************************************
* @file  MappShow.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the LED Showing firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "MappShow.h"
#include "MappCode.h"

#define CH_A 	0x00
#define CH_b 	0x01
#define CH_C 	0x02
#define CH_d 	0x03
#define CH_E 	0x04
#define CH_F 	0x05
#define CH_n 	0x06
#define CH_r 	0x07
#define CH_H 	0x08
#define CH__ 	0x09
#define CH_P 	0x0a

//#define CH_DOT 	0x80
#define CH_DOT 	0x01

//-------------------------0,---1,---2,---3,---4,---5,---6,---7,---8,---9--
//const uint8_t strNo[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
const uint8_t strNo[10]={0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6};

//------------------------A,---b,---C,---d,---E,---F,---n,---r,--'-'---'_'---P--
//const uint8_t strCh[11]={0x77,0x7c,0x39,0x5e,0x79,0x71,0x54,0x50,0x40,0x08,0x73};
const uint8_t strCh[11]={0xee,0x3e,0x9c,0x7a,0x9e,0x8e,0x2a,0x0a,0x02,0x10,0xce};

extern uint8_t gLedBuff[9];

ShowBit tShowBit;
/************************************************************
*function		: DoLedBitScan
*description	: Scan for bIT led light.
*On Entry	: none
*On Exit		: none
************************************************************/
void DoLedBitScan(void)
{
	if(tShowBit.BitDot1>0)
		gLedBuff[2] |= CH_DOT;

	if(tShowBit.BitDot2>0)
		gLedBuff[6] |= CH_DOT;

	if(tShowBit.BitLock>0)
		gLedBuff[0] |= CH_DOT;
	else
		gLedBuff[0] &= 0xfe;

	if(tShowBit.BitComm>0)
		gLedBuff[4] |= CH_DOT;

	if(tShowBit.BitTemp>0)
	{
		gLedBuff[1] &= 0xfe;
		gLedBuff[5] &= 0xfe;
		
		gLedBuff[3] |= CH_DOT;
		gLedBuff[7] |= CH_DOT;
	}
	else
	{
		gLedBuff[3] &= 0xfe;
		gLedBuff[7] &= 0xfe;
		
		gLedBuff[1] |= CH_DOT;
		gLedBuff[5] |= CH_DOT;
	}
}

/************************************************************
*function		: SetTempFlag
*description	: light Temperature Flag led.
*On Entry	: flag.
*On Exit		: none
************************************************************/
void SetTempFlag(uint8_t flag)
{
	tShowBit.BitTemp = flag;
}

uint8_t GetTempFlag(void)
{
	return tShowBit.BitTemp;
}
/************************************************************
*function		: SetLockFlag
*description	: light Lock Flag led.
*On Entry	: flag.
*On Exit		: none
************************************************************/
void SetLockFlag(uint8_t flag)
{
	tShowBit.BitLock = flag;
}
/************************************************************
*function		: SetCommFlag
*description	: light communication Flag led.
*On Entry	: flag.
*On Exit		: none
************************************************************/
void SetCommFlag(uint8_t flag)
{
	tShowBit.BitComm = flag;
}
/************************************************************
*function		: SetOut1Flag
*description	: light out1 Flag led.
*On Entry	: flag.
*On Exit		: none
************************************************************/
void SetOut1Flag(uint8_t flag)
{
	gLedBuff[8] &= 0x03;

	if(flag>=95)
		gLedBuff[8] |= 0xfc;
	else if(flag>=80)
		gLedBuff[8] |= 0xf8;
	else if(flag>=60)
		gLedBuff[8] |= 0xf0;
	else if(flag>=40)
		gLedBuff[8] |= 0xe0;
	else if(flag>=20)
		gLedBuff[8] |= 0xc0;
	else if(flag>=1)
		gLedBuff[8] |= 0x80;
}
/************************************************************
*function		: SetOut2Flag
*description	: light out2 Flag led.
*On Entry	: flag.
*On Exit		: none
************************************************************/
void SetOut2Flag(uint8_t flag)
{
	if(flag > 0)
		gLedBuff[8] |= 0x01;
	else
		gLedBuff[8] &= 0xfe;
}
/************************************************************
*function		: SetAlamFlag
*description	: light alam Flag led.
*On Entry	: flag.
*On Exit		: none
************************************************************/
void SetAlamFlag(uint8_t flag)
{
	if(flag > 0)
		gLedBuff[8] |= 0x02;
	else
		gLedBuff[8] &= 0xfd;
}

/************************************************************
*function		: DispShowLedx
*description	: light status led data.
*On Entry	: status led.
*On Exit		: none
************************************************************/
void DispShowLedx(uint8_t led)
{
	gLedBuff[8] = led;
}
/************************************************************
*function		: DispShowNo1
*description	: light number data.
*On Entry	: number data.
*On Exit		: none
************************************************************/
void DispShowNum1(long num)
{
	tShowBit.BitDot1 = 0;
	
	gLedBuff[3] = strNo[num%10];
	
	if(num>=10)
		gLedBuff[2] = strNo[(num%100)/10];
	else
		gLedBuff[2] = 0x00;

	if(num>=100)
		gLedBuff[1] = strNo[(num%1000)/100];
	else
		gLedBuff[1] = 0x00;
	
	if(num>=1000)
		gLedBuff[0] = strNo[num/1000];
	else
		gLedBuff[0] = 0x00;
}

void DispShowNum2(long num)
{
	tShowBit.BitDot2 = 0;
	
	gLedBuff[7] = strNo[num%10];
	
	if(num>=10)
		gLedBuff[6] = strNo[(num%100)/10];
	else
		gLedBuff[6] = 0x00;

	if(num>=100)
		gLedBuff[5] = strNo[(num%1000)/100];
	else
		gLedBuff[5] = 0x00;
	
	if(num>=1000)
		gLedBuff[4] = strNo[num/1000];
	else
		gLedBuff[4] = 0x00;
}
/************************************************************
*function		: DispShowFrac
*description	: light number data.
*On Entry	: number data,bits.
*On Exit		: none
************************************************************/
void DispShowFrc1(long num,uint16_t Bit)
{
	uint16_t uSign;

	if(num<0){
		uSign = 1;
		num = -num;

		if(num>=1000){
			num = num/10;
			Bit = Bit - 1;
		}
	}
	else{
		uSign = 0;
		if(num>=10000){
			num = num/10;
			Bit = Bit - 1;
		}
	}
	
	DispShowNum1(num);
	
	tShowBit.BitDot1 = Bit;

	if(Bit==1)
	{
		if(num < 10)
			gLedBuff[2] = strNo[0];
	}
	
	if(uSign>0)
		gLedBuff[0] = strCh[CH_H];
}

void DispShowFrc2(long num,uint16_t Bit)
{
	uint16_t uSign;

	if(num<0){
		uSign = 1;
		num = -num;

		if(num>=1000){
			num = num/10;
			Bit = Bit - 1;
		}
	}
	else{
		uSign = 0;
		if(num>=10000){
			num = num/10;
			Bit = Bit - 1;
		}
	}
	
	DispShowNum2(num);
	
	tShowBit.BitDot2 = Bit;

	if(Bit==1)
	{
		if(num < 10)
			gLedBuff[6] = strNo[0];
	}
	
	if(uSign>0)
		gLedBuff[4] = strCh[CH_H];
}
/************************************************************
*function		: DispMenuNum1
*description	: light number data.
*On Entry	: number data.
*On Exit		: none
************************************************************/
void DispMenuNum1(long num)
{
	tShowBit.BitDot1 = 0;
	
	gLedBuff[3] = strNo[num%10];
	
	gLedBuff[2] = strNo[(num%100)/10];

	gLedBuff[1] = strNo[(num%1000)/100];
	
	gLedBuff[0] = strNo[num/1000];
}

void DispMenuNum2(long num)
{
	tShowBit.BitDot2 = 0;
	
	gLedBuff[7] = strNo[num%10];
	
	gLedBuff[6] = strNo[(num%100)/10];

	gLedBuff[5] = strNo[(num%1000)/100];
	
	gLedBuff[4] = strNo[num/1000];
}

/************************************************************
*function		: DispMenuPswd
*description	: light number data.
*On Entry	: number data.
*On Exit		: none
************************************************************/
void DispMenuPswd(uint16_t num)
{
	tShowBit.BitDot2 = 0;
	
	gLedBuff[7] = strNo[num%10];
	
	if(num>=10)
		gLedBuff[6] = strNo[(num%100)/10];
	else
		gLedBuff[6] = 0x00;

	if(num>=100)
		gLedBuff[5] = strNo[(num%1000)/100];
	else
		gLedBuff[5] = 0x00;
	
	gLedBuff[4] = strCh[CH_C];
}
/************************************************************
*function		: DispMenuFunc
*description	: light number data.
*On Entry	: number data.
*On Exit		: none
************************************************************/
void DispMenuFunc(uint16_t func)
{
	tShowBit.BitDot2 = 0;
	
	if(func < emHeatOutput)
	{
		func += 1;
	}
	else if(func < emCoolOutput)
	{
		func = func-emHeatOutput+20;
	}
	else if(func < emCascadeSwon)
	{
		func = func-emCoolOutput+30;
	}
	else if(func < emErrCorrectedV)
	{
		func = func-emCascadeSwon+40;
	}
	else if(func < emuCommAddr)
	{
		func = func-emErrCorrectedV+50;
	}
	else if(func < emuRstDefault)
	{
		func = func-emuCommAddr+60;
	}
	else if(func < emqPID01Kp)
	{
		func = func-emuRstDefault+70;
	}
	else if(func < emTempSetVal)
	{
		func = func-emqPID01Kp+80;
	}

	gLedBuff[4] = strCh[CH_F];
	gLedBuff[5] = strCh[CH_H];
	gLedBuff[6] = strNo[func/10];
	gLedBuff[7] = strNo[func%10];
}
/************************************************************
*function		: DispMenuEnd
*description	: light number data.
*On Entry	: number data.
*On Exit		: none
************************************************************/
void DispMenuEnd(uint16_t u8id)
{
	tShowBit.BitDot2 = 0;
	
	if(u8id>0){
		gLedBuff[4] = 0x00;
		gLedBuff[5] = strCh[CH_E];
		gLedBuff[6] = strCh[CH_n];
		gLedBuff[7] = strCh[CH_d];
	}
	else{
		gLedBuff[4] = 0x00;
		gLedBuff[5] = strCh[CH_E];
		gLedBuff[6] = strCh[CH_r];
		gLedBuff[7] = strCh[CH_r];
	}
}

/************************************************************
*function		: DispErrorCode
*description	: light err code.
*On Entry	: err code data.
*On Exit		: none
************************************************************/
void DispErrorCode(uint16_t u8id)
{
	tShowBit.BitDot2 = 0;
	
	if(u8id==0xefff){
		gLedBuff[4] = 0x00;
		gLedBuff[5] = 0x00;
		gLedBuff[6] = 0x00;
		gLedBuff[7] = 0x00;
	}
	else if(u8id==0x7fff){
		gLedBuff[4] = 0x77;
		gLedBuff[5] = 0x1c;
		gLedBuff[6] = 0xd4;
		gLedBuff[7] = 0x80;
	}
    else if(u8id>=0x0A)
    {
        gLedBuff[4] = strCh[CH_E];
		gLedBuff[5] = strCh[CH_r];
		gLedBuff[6] = strNo[u8id/10];
		gLedBuff[7] = strNo[u8id%10];
    }
	else{
		gLedBuff[4] = strCh[CH_E];
		gLedBuff[5] = strCh[CH_r];
		gLedBuff[6] = strCh[CH__];
		gLedBuff[7] = strNo[u8id%10];
	}
}
/************************************************************
*function		: DispLogoCode
*description	: light jscc logo.
*On Entry	: jscc.
*On Exit		: none
************************************************************/
void DispLogoCode(void)
{
	tShowBit.BitDot1 = 0;
	
	gLedBuff[0] = 0x1e;
	gLedBuff[1] = 0x6d;
	gLedBuff[2] = 0x39;
	gLedBuff[3] = 0x39;
}
/************************************************************
*function		: DispMenuOff1
*description	: light Off.
*On Entry	: none.
*On Exit		: none
************************************************************/
void DispMenuOff1(void)
{
	tShowBit.BitDot1 = 0;
	gLedBuff[0] = 0x00;
	gLedBuff[1] = 0x00;
	gLedBuff[2] = 0x00;
	gLedBuff[3] = 0x00;
}

void DispMenuOff2(void)
{
	tShowBit.BitDot2 = 0;
	gLedBuff[4] = 0x00;
	gLedBuff[5] = 0x00;
	gLedBuff[6] = 0x00;
	gLedBuff[7] = 0x00;
}

