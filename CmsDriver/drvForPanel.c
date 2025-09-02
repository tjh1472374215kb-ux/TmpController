/******************************************************************************
* @file  drvForLED&KEY.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the LED&KEY firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "MainDefine.h"

typedef struct{
	uint8_t 	u8Step;
	uint8_t 	uKeyin;
	uint8_t 	uKeybk;
	uint8_t 	uDatKey;
	uint8_t 	uKeyDly;
	uint8_t 	uKeyBox;
	uint8_t 	uKeyTmr;
	uint16_t 	uKeyCnt;
}SCAN_PARA;

SCAN_PARA dtScan;

uint8_t gLedBuff[9];

void drvLightLed(uint8_t);
/************************************************************
*function		: DoLedKeyScan
*description	: Scan for key in?or led light?
*On Entry	: none
*On Exit		: none
************************************************************/
#define RED_LIGHT     6
#define RED_Bit_LIGHT 7
#define RED_POINT     2
#define GRE_LIGHT     1
#define GRE_Bit_LIGHT 1
#define RED_BELT      4

void DoLedKeyScan(void)
{
    static uint8_t count = 0;
    
    dtScan.uKeyin = 0;
    switch(dtScan.u8Step)
    {
        case 0:
            SCAN_SEL9_L;
            if(count==0)drvLightLed(gLedBuff[0]);
            SCAN_SEL1_H;
            if(count==RED_LIGHT)
            {
                if(gLedBuff[0] & 0x01)drvLightLed(0x01);
                else drvLightLed(0x00);
            }
            count++;
            if(count>=RED_LIGHT+RED_Bit_LIGHT)
            {
                if(P_KEY_IN>0)dtScan.uKeyin= KEY_MODE;
                count=0;
                dtScan.u8Step=1;
                break;
            }
            return;
        case 1:
            SCAN_SEL1_L;
            if(count==0)drvLightLed(gLedBuff[1]);
            SCAN_SEL2_H;
            if(count==RED_LIGHT)
            {
                if(gLedBuff[1] & 0x01)drvLightLed(0x01);
                else drvLightLed(0x00);
            }
            count++;
            if(count>=RED_LIGHT+RED_Bit_LIGHT)
            {
                if(P_KEY_IN>0)dtScan.uKeyin= KEY_ENTER;
                count=0;
                dtScan.u8Step=2;
                break;
            }
            return;
        case 2:
            SCAN_SEL2_L;
            if(count==0)drvLightLed(gLedBuff[2]);
            SCAN_SEL3_H;
            if(count==RED_POINT)
            {
                drvLightLed(gLedBuff[2] & 0xFE);          //清除小数点
            }
            count++;
            if(count>=RED_LIGHT)
            {
                if(P_KEY_IN>0)dtScan.uKeyin= KEY_UP;
                count=0;
                dtScan.u8Step=3;
                break;
            }
            return;
        case 3:
            SCAN_SEL3_L;
            if(count==0)drvLightLed(gLedBuff[3]);
            SCAN_SEL4_H;
            if(count==RED_LIGHT)
            {
                if(gLedBuff[3] & 0x01)drvLightLed(0x01);
                else drvLightLed(0x00);
            }
            count++;
            if(count>=RED_LIGHT+RED_Bit_LIGHT)
            {
                if(P_KEY_IN>0)dtScan.uKeyin= KEY_DOWN;
                count=0;
                dtScan.u8Step=4;
                break;
            }
            return;
        case 4:
            SCAN_SEL4_L;
            if(count==0)drvLightLed(gLedBuff[4]);
            SCAN_SEL5_H;
            if(count==GRE_LIGHT)
            {
                if(gLedBuff[4] & 0x01)drvLightLed(0x01);
                else drvLightLed(0x00);
            }
            count++;
            if(count>=RED_BELT)
            {
                count=0;
                dtScan.u8Step=5;
            }
            return;
        case 5:
            SCAN_SEL5_L;
            if(count==0)drvLightLed(gLedBuff[5]);
            SCAN_SEL6_H;
            if(count==GRE_LIGHT)
            {
                if(gLedBuff[5] & 0x01)drvLightLed(0x01);
                else drvLightLed(0x00);
            }
            count++;
            if(count>=GRE_LIGHT+GRE_Bit_LIGHT)
            {
                count=0;
                dtScan.u8Step=6;
            }
            return;
        case 6:
            SCAN_SEL6_L;
            if(count==0)drvLightLed(gLedBuff[6]);
            SCAN_SEL7_H;
            count++;
            if(count>=GRE_LIGHT)
            {
                count=0;
                dtScan.u8Step=7;
            }
            return;
        case 7:
            SCAN_SEL7_L;
            if(count==0)drvLightLed(gLedBuff[7]);
            SCAN_SEL8_H;
            if(count==GRE_LIGHT)
            {
                if(gLedBuff[7] & 0x01)drvLightLed(0x01);
                else drvLightLed(0x00);
            }
            count++;
            if(count>=GRE_LIGHT+GRE_Bit_LIGHT)
            {
                count=0;
                dtScan.u8Step=8;
            }
            return;
        default:
			SCAN_SEL8_L;
			if(count==0)drvLightLed(gLedBuff[8]);
			SCAN_SEL9_H;
            count++;
			if(count>=RED_BELT)
            {
                count=0;
                dtScan.u8Step=0;
            }
            return;
    }
    
    
    if(dtScan.uKeyin>0){ //if key in 
		dtScan.uKeyTmr = 0;
		//check key in.
		if(dtScan.uKeybk==dtScan.uKeyin){ 
			dtScan.uKeyCnt++;
			//make sure the press key.
			if(dtScan.uKeyCnt>2){
				if(dtScan.uKeyDly==0){
					dtScan.uDatKey=dtScan.uKeyin;
					dtScan.uKeyDly = 40;
					dtScan.uKeyBox = 1;
				}
			}
		}
		else{//new key in.
			dtScan.uKeybk = dtScan.uKeyin; 
			dtScan.uKeyCnt = 0;
		}
	}

	 // key release ,clear all status.
	if(dtScan.uKeyTmr<15){
		dtScan.uKeyTmr++;
		if(dtScan.uKeyTmr==14){
			dtScan.uKeybk = 0;
			dtScan.uKeyCnt=0;
		}
	}

	//key delay during the key issue.
	if(dtScan.uKeyDly>0)
		dtScan.uKeyDly--;
    
}
//void DoLedKeyScan(void)
//{
//	dtScan.uKeyin = 0;
//	switch(dtScan.u8Step){
//		case 0:
//			SCAN_SEL9_L;
//			drvLightLed(gLedBuff[1]);
//			SCAN_SEL1_H;
//			dtScan.u8Step=1;
//			return;
//		case 1:
//			if(P_KEY_IN>0)
//				dtScan.uKeyin= KEY_MODE;

//			SCAN_SEL1_L;
//			drvLightLed(gLedBuff[2]);
//			SCAN_SEL2_H;
//			dtScan.u8Step=2;
//			break;
//		case 2:
//			if(P_KEY_IN>0)
//				dtScan.uKeyin= KEY_ENTER;

//			SCAN_SEL2_L;
//			drvLightLed(gLedBuff[3]);
//			SCAN_SEL3_H;
//			dtScan.u8Step=3;
//			break;
//		case 3:
//			if(P_KEY_IN>0)
//				dtScan.uKeyin= KEY_UP;

//			SCAN_SEL3_L;
//			drvLightLed(gLedBuff[4]);
//			SCAN_SEL4_H;
//			dtScan.u8Step=4;
//			break;
//		case 4:
//			if(P_KEY_IN>0)
//				dtScan.uKeyin= KEY_DOWN;
//			
//			SCAN_SEL4_L;
//			drvLightLed(gLedBuff[5]);
//			SCAN_SEL5_H;
//			dtScan.u8Step=5;
//			break;
//		case 5:
//			SCAN_SEL5_L;
//			drvLightLed(gLedBuff[6]);
//			SCAN_SEL6_H;
//			dtScan.u8Step=6;
//			return;
//		case 6:
//			SCAN_SEL6_L;
//			drvLightLed(gLedBuff[7]);
//			SCAN_SEL7_H;
//			dtScan.u8Step=7;
//			return;
//		case 7:
//			SCAN_SEL7_L;
//			drvLightLed(gLedBuff[8]);
//			SCAN_SEL8_H;
//			dtScan.u8Step=8;
//			return;
//		default:
//			SCAN_SEL8_L;
//			drvLightLed(gLedBuff[0]);
//			SCAN_SEL9_H;
//			dtScan.u8Step=0;
//			return;
//	}

//	if(dtScan.uKeyin>0){ //if key in 
//		dtScan.uKeyTmr = 0;
//		//check key in.
//		if(dtScan.uKeybk==dtScan.uKeyin){ 
//			dtScan.uKeyCnt++;
//			//make sure the press key.
//			if(dtScan.uKeyCnt>3){
//				if(dtScan.uKeyDly==0){
//					dtScan.uDatKey=dtScan.uKeyin;
//					dtScan.uKeyDly = 60;
//					dtScan.uKeyBox = 1;
//				}
//			}
//		}
//		else{//new key in.
//			dtScan.uKeybk = dtScan.uKeyin; 
//			dtScan.uKeyCnt = 0;
//		}
//	}

//	 // key release ,clear all status.
//	if(dtScan.uKeyTmr<15){
//		dtScan.uKeyTmr++;
//		if(dtScan.uKeyTmr==14){
//			dtScan.uKeybk = 0;
//			dtScan.uKeyCnt=0;
//		}
//	}

//	//key delay during the key issue.
//	if(dtScan.uKeyDly>0)
//		dtScan.uKeyDly--;
//}
/************************************************************
*function		: InitialKeyData
*description	: 0
*On Entry	: none
*On Exit		: none
************************************************************/
void InitialKeyData(void)
{
	dtScan.uKeyBox = 0;
	dtScan.uDatKey = 0;
}
/************************************************************
*function		: drvLightLed
*description	: light one led
*On Entry	: none
*On Exit		: none
************************************************************/
void SPI_Delay(void)
{
	uint8_t count=3;
	while(count--);
}

void drvLightLed(uint8_t u8outs)
{
	uint8_t ui;//Bit; 

	for(ui=0; ui<8; ui++)
	{
		if((u8outs>>(7-ui))&0x01)
			HC_SPIDO_H;
		else
			HC_SPIDO_L;

		HC_SPICK_L;
        SPI_Delay();
        HC_SPICK_H;
        SPI_Delay();
	}
	HC_SPICS_H;
    SPI_Delay();
    HC_SPICS_L;
    SPI_Delay();
}
/************************************************************
*function		: MsgIfKeyIn
*description	: judge if key in?
*On Entry	: none
*On Exit		: none
************************************************************/
uint8_t GetMsgIfKeyIn(void)
{
	if(dtScan.uKeyBox>0){
		dtScan.uKeyBox=0;
		return 1;
	}

	return 0;
}
/************************************************************
*function		: MsgKeyData
*description	: Get the key value
*On Entry	: none
*On Exit		: key value
************************************************************/
uint8_t GetMsgKeyData(void)
{
	return dtScan.uDatKey;
}
/************************************************************
*function		: MsgKeyCnt
*description	: Get the key in time for long key?
*On Entry	: none
*On Exit		: none
************************************************************/
uint16_t GetMsgKeyCnt(void)
{
	return dtScan.uKeyCnt;
}

