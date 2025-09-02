/******************************************************************************
* @file  MappComm.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the Code firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 
#include "MappComm.h"
#include "MappCode.h"
#include "MappShow.h"
#include "MappMenu.h"

#include "MCtrlMain.h"


COMM_DATA dtComm;

AscciiWord pAscciiWd;


//added at 2025.09.01 tjh
extern void Sensor_Mode_Select(uint8_t mode);
/******************************************************************************
*Function 		: void MappMainU1COM(void)
*Description	: It takes Communication With HMI:
							: 1.Receive the Command;
							: 2.Answer the Command;
*******************************************************************************/
void DoModbusScan(void)
{
	if(dtComm.txlen>0)//if tx data....
	{
		/*send datas is finish*/
		if(dtComm.txcnt>=dtComm.txlen)
		{
			uint16_t TimeStamp=0;
			/*tx buffer is empty*/
			while(!(UART1->LSR&0x40))
			{
				TimeStamp++;
				if(TimeStamp>40000)
					break;
			}
			dtComm.txcnt=0;
			dtComm.txlen=0;

			SetCommFlag(0);
			
			P_U1COM_CS_L;
		}
	}
	else if(dtComm.rxicnt!=dtComm.rxocnt)
	{
		uint8_t u8tmp;

		u8tmp = dtComm.rxbuf[dtComm.rxocnt];
		
		if(dtComm.rxocnt<120)
			dtComm.rxocnt++;
		else
			dtComm.rxocnt=0;

		//Ascii format
		if(gParcode.Item.uCommMode==1)
		{
			switch(u8tmp){
				case 0x3a: //start bit
					dtComm.u8State = 1;
					dtComm.u8TxLen = 1;
					dtComm.txbuf[0]=u8tmp;
					break;
				case 0x0d: //end bit0 0x0d
					if(dtComm.u8State==1)
						dtComm.u8State = 2;
					break;
				case 0x0a: //end bit1 0x0a
					if(dtComm.u8State==2&&dtComm.u8TxLen>8)
					{
						dtComm.u8State = 0;
						dtComm.u8DataIn=1;
					}
					break;
				default: //data bit
					if(dtComm.u8State>0){
						
						if(dtComm.u8TxLen>100){
							dtComm.u8TxLen = 0;
							dtComm.u8State = 0;
						}
						
						dtComm.txbuf[dtComm.u8TxLen]=u8tmp;
						dtComm.u8TxLen++;
					}
					break;
			}
			//it has received a full string data.
			if(dtComm.u8DataIn>0&&dtComm.u8TxLen>4){
				dtComm.u8DataIn=0;

				SetCommFlag(0);
			
				//get the address
				dtComm.u8Addr = GetASCiiByte(&dtComm.txbuf[1]);
				//check the address...
				if(gParcode.Item.uCommAddr==dtComm.u8Addr||dtComm.u8Addr==0)
				{
					 u8tmp = CalcLRCChksum(&dtComm.txbuf[1],dtComm.u8TxLen-3);
					 dtComm.u8Sum = GetASCiiByte(&dtComm.txbuf[dtComm.u8TxLen-2]);

					 dtComm.uErrCnt = gParcode.Item.uCommTimer*10;
					 
					//check the checksum..
					if(dtComm.u8Sum==u8tmp)
						DoModbusAscciiCmd();
				}
			}
		}
		else{//RTU format...
			//start to receive data.
			if(dtComm.u8TxLen>100)
					dtComm.u8TxLen = 0;
				
			dtComm.txbuf[dtComm.u8TxLen]=u8tmp;
			dtComm.u8TxLen++;

			dtComm.uRTUtim2 = 0;

			SetCommFlag(1);
		}
	}
	else if(gParcode.Item.uCommMode==2){//RTU format...

		if(dtComm.uRTUtim2>dtComm.uRTUtim0)//end receive
		{
			/*it has received a string data*/
			if(dtComm.u8TxLen>4)
			{
				SetCommFlag(0);
				
				dtComm.u8Addr = dtComm.txbuf[0];
				//check the address...
				if(gParcode.Item.uCommAddr==dtComm.u8Addr||dtComm.u8Addr==0){
					uint16_t u16Chksum;
					
					 dtComm.u16Sum = CalcCRCChksum(dtComm.txbuf,dtComm.u8TxLen-2);

					 dtComm.u8ErrCode = 0;
					 u16Chksum = dtComm.txbuf[dtComm.u8TxLen-1];
					 u16Chksum = (u16Chksum<<8)|dtComm.txbuf[dtComm.u8TxLen-2];

					 dtComm.uErrCnt = gParcode.Item.uCommTimer*10;
					 
					//check the checksum..
					if(dtComm.u16Sum==u16Chksum)
						DoModbusRTUCmd();
				}
			}
			
			dtComm.u8TxLen = 0;
		}
	}
}

/************************************************************
*function		: MappUxComTime
************************************************************/
void MappUxComTime(void)
{
	if(dtComm.uRTUtim2<32)
		dtComm.uRTUtim2++;
}

void Do485Time10MS(void)
{
	if(dtComm.uErrCnt>0)
	{
		dtComm.uErrCnt--;
		if(dtComm.uErrCnt==0)
		{
			SetErrorCode(6);
		}
	}
}
/************************************************************
*function		: DoSendData
************************************************************/
void DoSendData(uint16_t len)
{
	dtComm.txcnt = 1;
	dtComm.txlen = len;
	
	//enable output
	P_U1COM_CS_H;

	SetCommFlag(1);
	
	//enable tx interrupt
	UART1->THR = dtComm.txbuf[0];
	UART1->IER |= 0x02;
}
/************************************************************
*function		: InitModbusPar
************************************************************/
void InitModbusPar(void)
{
	dtComm.txlen = 0;
	dtComm.rxicnt= 0;
	dtComm.rxocnt= 0;

	dtComm.u8TxLen = 0;
	
	dtComm.uRTUtim0 = 8;
	dtComm.uRTUtim2 = 0;

	SetCommXXFormat();
	SetCommBaudrate();

	P_U1COM_CS_L;
}

/************************************************************
*function		: SetCommFormat
************************************************************/
void SetCommXXFormat(void)
{
	switch(gParcode.Item.uCommFormat)
	{
		case 1://8,n,1
			UART1->LCR = 0x03;
			break;
		case 2://8,e,1
			UART1->LCR = 0x1b;
			break;
		case 3://8,o,1
			UART1->LCR = 0x0b;
			break;
		case 4://8,n,2
			UART1->LCR = 0x07;
			break;
		case 5://8,e,2
			UART1->LCR = 0x1f;
			break;
		case 6://8,o,2
			UART1->LCR = 0x0f;
			break;
	}
}

/************************************************************
*function		: SetCommBaudrate
************************************************************/
void SetCommBaudrate(void)
{
	uint32_t tmpClk;

	tmpClk = SysClockFreq/16;
	
	switch(gParcode.Item.uCommSpeed)
	{
		case 1:
			UART1->DLR = (tmpClk+2400)/4800;
			dtComm.uRTUtim0 = 16;
			break;
		case 2:
			UART1->DLR = (tmpClk+4800)/9600;
			dtComm.uRTUtim0 = 8;
			break;
		case 3:
			UART1->DLR = (tmpClk+9600)/19200;
			dtComm.uRTUtim0 = 4;
			break;
		case 4:
			UART1->DLR = (tmpClk+19200)/38400;
			dtComm.uRTUtim0 = 4;
			break;
		case 5:
			UART1->DLR = (tmpClk+28800)/57600;
			dtComm.uRTUtim0 = 4;
			break;
		case 6:
			UART1->DLR = (tmpClk+57600)/115200;
			dtComm.uRTUtim0 = 4;
			break;
		default:
			dtComm.uRTUtim0 = 4;
			break;
	}
}

/************************************************************
*function		: DoModbusAscciiCmd
*description	: deal with the command from modbus.
*On Entry	: none
*On Exit		: none
************************************************************/
void DoModbusAscciiCmd(void)
{
	dtComm.u8ErrCode = 0;
	dtComm.u8Cmd = GetASCiiByte(&dtComm.txbuf[3]);
	dtComm.u16DAddr = GetASCiiWord(&dtComm.txbuf[5]);

	switch(dtComm.u8Cmd)
	{
		case 0x03:
			dtComm.u8DLen = GetASCiiWord(&dtComm.txbuf[9]);
			if(dtComm.u8DLen>0&&dtComm.u8Addr>0)
				DoReadnRegister1();
			break;
		case 0x06:
			dtComm.u16DValue = GetASCiiWord(&dtComm.txbuf[9]);
				DoWrite1Register1();
			break;
		case 0x10:
			dtComm.u8DLen = GetASCiiWord(&dtComm.txbuf[9]);
			if(dtComm.u8DLen>0)
				DoWritenRegister1();
			break;
		default:
			dtComm.u8ErrCode=1;
			break;
	}
}
/************************************************************
*function		: DoModbusRTUCmd
*description	: deal with the command from modbus.
*On Entry	: none
*On Exit		: none
************************************************************/
void DoModbusRTUCmd(void)
{
	uint16_t utmp;
	
	dtComm.u8Cmd = dtComm.txbuf[1];
	utmp = dtComm.txbuf[2];
	dtComm.u16DAddr = (utmp<<8)|dtComm.txbuf[3];
	dtComm.u8ErrCode = 0;

	switch(dtComm.u8Cmd){
		case 0x03:
			dtComm.u8DLen = dtComm.txbuf[5];
			if(dtComm.u8DLen>0&&dtComm.u8Addr>0)
				DoReadnRegister2();
			break;
		case 0x06:
			utmp = dtComm.txbuf[4];
			dtComm.u16DValue = (utmp<<8)|dtComm.txbuf[5];
			DoWrite1Register2();
			break;
		case 0x10:
			dtComm.u8DLen = dtComm.txbuf[5];
			if(dtComm.u8DLen>0)
				DoWritenRegister2();
			break;
		default:
			dtComm.u8ErrCode=1;
			break;
	}
}

/************************************************************
*function		: GetASCiiCode
*description	: get the data from asccii.
*On Entry	: ascii data
*On Exit		: RTU data.
************************************************************/
uint8_t GetASCiiCode(uint8_t u8buf)
{
	uint8_t u8data;

	if(u8buf > 9)
		u8data=u8buf+0x37;
	else
		u8data=u8buf+0x30;

	return u8data;
}
/************************************************************
*function		: GetASCiiData
*description	: get the data from asccii.
*On Entry	: ascii data
*On Exit		: RTU data.
************************************************************/
uint8_t GetASCiiData(uint8_t u8buf)
{
	uint8_t u8data;

	if(u8buf>0x39)
		u8data=u8buf-0x37;
	else
		u8data=u8buf&0x0f;

	return u8data;
}
/************************************************************
*function		: GetASCiiByte
*description	: get the BYTE data from asccii.
*On Entry	: ascii data
*On Exit		: RTU data.
************************************************************/
uint8_t GetASCiiByte(uint8_t *u8buf)
{
	uint8_t u8data;

	u8data=(GetASCiiData(u8buf[0])<<4)|GetASCiiData(u8buf[1]);

	return u8data;
}

/************************************************************
*function		: GetASCiiWord
*description	: get the 	word data from asccii.
*On Entry	: ascii data
*On Exit		: RTU data.
************************************************************/
uint16_t GetASCiiWord(uint8_t *u8buf)
{
	uint16_t u16data,udatx;

	udatx = GetASCiiData(u8buf[0]);
	u16data = udatx<<12;
	udatx = GetASCiiData(u8buf[1]);
	u16data |= (udatx<<8);
	u16data |= (GetASCiiData(u8buf[2])<<4);
	u16data |= GetASCiiData(u8buf[3]);

	return u16data;
}
/************************************************************
*function		: CalcLRCChksum
*description	: calcare the data to checksum
*On Entry	: data string
*On Exit		: LRC value
************************************************************/
uint8_t CalcLRCChksum(uint8_t *u8buf,uint8_t u8len)
{
	uint8_t u8i,u8Sum;

	for(u8i=0,u8Sum=0;u8i<u8len;u8i+=2)
		u8Sum +=GetASCiiByte(&u8buf[u8i]);

	return ((0x100-u8Sum)&0xff);
}
/************************************************************
*function		: CalcLRCChksum
*description	: calcare the data to checksum
*On Entry	: data string
*On Exit		: CRC value
************************************************************/
uint16_t CalcCRCChksum(uint8_t *u8buf,uint8_t u8len)
{
	uint8_t u8i,u8j;
	uint16_t u16Sum=0xffff;

	for(u8j=0;u8j<u8len;u8j++){
		u16Sum^=u8buf[u8j];
		for(u8i=0;u8i<8;u8i++){
			if(u16Sum&0x01)
				u16Sum=(u16Sum>>1)^0xa001;
			else
				u16Sum=(u16Sum>>1);
		}
	}

	return u16Sum;
}
/************************************************************
*function		: DoReadnRegister1
*description	: calcare the data to checksum
*On Entry	: data string
*On Exit		: CRC value
************************************************************/
void DoReadnRegister1(void)
{
	if(dtComm.u16DAddr<0x1000)
	{
		dtComm.uIndex=GetMenuIndex(dtComm.u16DAddr);

		if((dtComm.uIndex+dtComm.u8DLen)<=ParmMaxItem)
		{
			dtComm.u8XLen = dtComm.u8DLen<<1;
			dtComm.txbuf[5] = GetASCiiCode(dtComm.u8XLen>>4);
			dtComm.txbuf[6] = GetASCiiCode(dtComm.u8XLen&0xf);
			dtComm.u8XLen = 7;

			do{
				pAscciiWd.uData=gParcode.u16Buff[dtComm.uIndex];

				dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit16);
				dtComm.u8XLen++;
				dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit12);
				dtComm.u8XLen++;
				dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit08);
				dtComm.u8XLen++;
				dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit04);
				
				dtComm.u8XLen++;
				dtComm.uIndex++;
				dtComm.u8DLen --;
			}while(dtComm.u8DLen>0);
		}
		else
		{
			dtComm.u8ErrCode = 2;
		}
	}
	else if(dtComm.u16DAddr>=0x2000&&dtComm.u16DAddr<=0x2004)
	{
		uint16_t tmpBuf[5];

		if(GetErrorCode()>0)
			tmpBuf[0]=8;
		else 
			tmpBuf[0]=GetOutputFlag();

		tmpBuf[1]=tCtrlOutParm.GetTempT1Val;
		tmpBuf[2]=0;
		tmpBuf[3]=0;
		tmpBuf[4]=0;
		
		dtComm.uIndex = dtComm.u16DAddr-0x2000;
		dtComm.u8XLen = dtComm.u8DLen<<1;
		dtComm.txbuf[5] = GetASCiiCode(dtComm.u8XLen>>4);
		dtComm.txbuf[6] = GetASCiiCode(dtComm.u8XLen&0xf);
		dtComm.u8XLen = 7;
		
		do{
			pAscciiWd.uData=tmpBuf[dtComm.uIndex];

			dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit16);
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit12);
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit08);
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit04);
			
			dtComm.u8XLen++;
			dtComm.uIndex++;
			dtComm.u8DLen --;
		}while(dtComm.u8DLen>0);
	}
	else if(dtComm.u16DAddr==0x3000)
	{
		if(dtComm.u8DLen==1)
		{
			dtComm.u8XLen = dtComm.u8DLen<<1;
			dtComm.txbuf[5] = GetASCiiCode(dtComm.u8XLen>>4);
			dtComm.txbuf[6] = GetASCiiCode(dtComm.u8XLen&0xf);
			dtComm.u8XLen = 7;

			pAscciiWd.uData=GetErrorCode();

			dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit16);
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit12);
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit08);
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(pAscciiWd.B.Bit04);
			dtComm.u8XLen++;
		}
		else
		{
			dtComm.u8ErrCode = 2;
		}
	}
	//added at 2025.09.01 tjh
	else if(dtComm.u16DAddr>=0x5000 && dtComm.u16DAddr<=0x5009)
	{
		if(dtComm.u8DLen==1)
		{
			dtComm.u8XLen = dtComm.u8DLen<<1;
			dtComm.txbuf[2] = dtComm.u8XLen;
			dtComm.u8XLen = 3;
			
			switch(dtComm.u16DAddr)
			{
				case 0x5000:pAscciiWd.uData=CALData.cold_temp_offset;		break;
				case 0x5001:pAscciiWd.uData=CALData.LHA7918_ZERO_OFFSET1;	break;
				case 0x5002:pAscciiWd.uData=CALData.LHA7918_ZERO_OFFSET2;	break;
				case 0x5003:pAscciiWd.uData=CALData.LHA7918_ZERO_OFFSET3;	break;
				case 0x5004:pAscciiWd.uData=CALData.LHA7918_CAL1;			break;
				case 0x5005:pAscciiWd.uData=CALData.LHA7918_CAL2;			break;
				case 0x5006:pAscciiWd.uData=CALData.LHA7918_CAL3;			break;
				case 0x5007:pAscciiWd.uData=CALData.V_ref;					break;
				case 0x5008:pAscciiWd.uData=CALData.Res_ref;				break;
				case 0x5009:pAscciiWd.uData=CALData.MCU_Vref;				break;
				default:break;
			}
			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData>>8;
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData&0xff;
			dtComm.u8XLen++;	
		}
		else 
		{
			dtComm.u8ErrCode = 2;
		}	
	}
	
	//added at 2025.08.30 tjh
	else if(dtComm.u16DAddr==0x5200 | dtComm.u16DAddr==0x5203)
	{
		if(dtComm.u8DLen==1)
		{
			dtComm.u8XLen = dtComm.u8DLen<<1;
			dtComm.txbuf[2] = dtComm.u8XLen;
			dtComm.u8XLen = 3;

			pAscciiWd.uData=LHAData.Data1;

			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData>>8;
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData&0xff;
			dtComm.u8XLen++;			
		}
		else 
		{
			dtComm.u8ErrCode = 2;
		}
	}

	
	else 
	{
		dtComm.u8ErrCode = 2;
	}
	
	if(dtComm.u8ErrCode>0){//error...
		dtComm.txbuf[3]='8';
		dtComm.txbuf[5]=GetASCiiCode(dtComm.u8ErrCode>>4);
		dtComm.txbuf[6]=GetASCiiCode(dtComm.u8ErrCode&0xf);
		dtComm.uIndex=CalcLRCChksum(&dtComm.txbuf[1],6);
		dtComm.txbuf[7]=GetASCiiCode(dtComm.uIndex>>4);
		dtComm.txbuf[8]=GetASCiiCode(dtComm.uIndex&0xf);
		dtComm.txbuf[9] = 0x0d;
		dtComm.txbuf[10] = 0x0a;
		DoSendData(11);
	}
	else{
		dtComm.u8Sum = CalcLRCChksum(&dtComm.txbuf[1],dtComm.u8XLen-1);
		dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(dtComm.u8Sum>>4);
		dtComm.u8XLen++;
		dtComm.txbuf[dtComm.u8XLen] = GetASCiiCode(dtComm.u8Sum&0xf);
		dtComm.u8XLen++;
		dtComm.txbuf[dtComm.u8XLen] = 0x0d;
		dtComm.u8XLen++;
		dtComm.txbuf[dtComm.u8XLen] = 0x0a;
		dtComm.u8XLen++;
		DoSendData(dtComm.u8XLen);
	}
}
/************************************************************
*function		: DoWrite1Register1
*description	: calcare the data to checksum
*On Entry	: data string
*On Exit		: CRC value
************************************************************/
void DoWrite1Register1(void)
{
	//run controller style ...
	if(dtComm.u16DAddr<0x1000)
	{
		dtComm.uIndex=GetMenuIndex(dtComm.u16DAddr);

		if(dtComm.uIndex>emqPID12Kd)
			dtComm.u8ErrCode=2;
		else
		{
			if(DoSave1FromComm(dtComm.uIndex,dtComm.u16DValue))
				dtComm.u8ErrCode=3;
		}
	}
	else if(dtComm.u16DAddr==0x1000)
	{
		if(dtComm.u16DValue<2)
		{
			if(gParcode.Item.XRunSwitch==1&&gParcode.Item.CtrlMethod==3)
				SetOutputSwitch(dtComm.u16DValue);
			else
				dtComm.u8ErrCode=3;
		}
		else if(dtComm.u16DValue==8)
		{
			if(gParcode.Item.WarnReset==2)
					SetErrorCode(0);
			else
				dtComm.u8ErrCode=3;
		}
	}
	else if(dtComm.u16DAddr==0x1001)
	{
		if(gParcode.Item.CtrlMethod==3)
		{
			gParcode.Item.TempSetVal = dtComm.u16DValue;
			SetOutpTmpToRef(gParcode.Item.TempSetVal);
			DispShowFrc2(gParcode.Item.TempSetVal,1);
		}
		else
		{
			dtComm.u8ErrCode=3;
		}
	}
	else if(dtComm.u16DAddr==0x1002)
	{
		
	}
	else if(dtComm.u16DAddr==0x1003)
	{
		
	}
	else if(dtComm.u16DAddr==0x1004)
	{
		
	}
	//added at 2025.09.01 tjh
	else if(dtComm.u16DAddr >= 0x5000 && dtComm.u16DAddr <= 0x5009)
	{
		switch(dtComm.u16DAddr)
		{
			case 0x5000:CALData.cold_temp_offset = 	dtComm.u16DValue;		break;
			case 0x5001:CALData.LHA7918_ZERO_OFFSET1 = dtComm.u16DValue;	break;
			case 0x5002:CALData.LHA7918_ZERO_OFFSET2 = dtComm.u16DValue;	break;
			case 0x5003:CALData.LHA7918_ZERO_OFFSET3 = dtComm.u16DValue;	break;
			case 0x5004:CALData.LHA7918_CAL1 = dtComm.u16DValue;			break;
			case 0x5005:CALData.LHA7918_CAL2 = dtComm.u16DValue;			break;
			case 0x5006:CALData.LHA7918_CAL3 = dtComm.u16DValue;			break;
			case 0x5007:CALData.V_ref = dtComm.u16DValue;					break;
			case 0x5008:CALData.Res_ref = dtComm.u16DValue;					break;
			case 0x5009:CALData.MCU_Vref = dtComm.u16DValue;				break;
			default:break;
		}
	}
	
	//added at 2025.8.29 tjh	
	else if(dtComm.u16DAddr >= 0x5100 && dtComm.u16DAddr <= 0x5104)
	{
		if(dtComm.u16DValue)
		{
			LHAData.LHA_Step=2;
			LHAData.LHAOut=0;
			switch(dtComm.u16DAddr)
			{
				case 0x5100:LHAData.WRITE1=0x0C2B;			break;
				case 0x5101:LHAData.WRITE1=0x062B;			break;
				case 0x5102:LHAData.WRITE1=0x022B;			break;
				case 0x5103:LHAData.WRITE1=0x622B;			break;
				case 0x5104:Sensor_Mode_Select(RTD_MODE);	break;
				default:break;	
			}
		}
	}	
	else
	{
		dtComm.u8ErrCode = 2;
	}


	if(dtComm.u8Addr>0){
		if(dtComm.u8ErrCode>0){//error...
			dtComm.txbuf[3]='8';
			dtComm.txbuf[5]=GetASCiiCode(dtComm.u8ErrCode>>4);
			dtComm.txbuf[6]=GetASCiiCode(dtComm.u8ErrCode&0xf);
			dtComm.uIndex=CalcLRCChksum(&dtComm.txbuf[1],6);
			dtComm.txbuf[7]=GetASCiiCode(dtComm.uIndex>>4);
			dtComm.txbuf[8]=GetASCiiCode(dtComm.uIndex&0xf);
			dtComm.txbuf[9] = 0x0d;
			dtComm.txbuf[10] = 0x0a;
			DoSendData(11);
		}
		else{
			dtComm.txbuf[dtComm.u8TxLen] = 0x0d;
			dtComm.u8TxLen++;
			dtComm.txbuf[dtComm.u8TxLen] = 0x0a;
			dtComm.u8TxLen++;
			DoSendData(dtComm.u8TxLen);
		}
	}
}
/************************************************************
*function		: CalcLRCChksum
*description	: calcare the data to checksum
*On Entry	: data string
*On Exit		: CRC value
************************************************************/
void DoWritenRegister1(void)
{
	if(dtComm.u16DAddr<0x1000)
	{
		uint8_t utemp,u8i;
		
		dtComm.uIndex=GetMenuIndex(dtComm.u16DAddr);

		if(dtComm.uIndex==0xffff)
			dtComm.u8ErrCode=2;
		else if(dtComm.u8DLen<26&&dtComm.u8ErrCode==0
			&&(dtComm.uIndex+dtComm.u8DLen)<emuCheckSum0)
		{
			utemp = 15;
			for(u8i=0;u8i<dtComm.u8DLen;u8i++,utemp += 4)
				dtComm.upDat[u8i]=GetASCiiWord(&dtComm.txbuf[utemp]);
				
			if(DoSavenFromComm(dtComm.uIndex,dtComm.u8DLen,dtComm.upDat))
				dtComm.u8ErrCode=7;
		}
	}
	else dtComm.u8ErrCode=2;

	if(dtComm.u8Addr>0){
		if(dtComm.u8ErrCode>0){//error...
			dtComm.txbuf[3]='8';
			dtComm.txbuf[5]=GetASCiiCode(dtComm.u8ErrCode>>4);
			dtComm.txbuf[6]=GetASCiiCode(dtComm.u8ErrCode&0xf);
			dtComm.uIndex=CalcLRCChksum(&dtComm.txbuf[1],6);
			dtComm.txbuf[7]=GetASCiiCode(dtComm.uIndex>>4);
			dtComm.txbuf[8]=GetASCiiCode(dtComm.uIndex&0xf);
			dtComm.txbuf[9] = 0x0d;
			dtComm.txbuf[10] = 0x0a;
			DoSendData(11);
		}
		else{
			dtComm.uIndex=CalcLRCChksum(&dtComm.txbuf[1],12);
			dtComm.txbuf[13]=GetASCiiCode(dtComm.uIndex>>4);
			dtComm.txbuf[14]=GetASCiiCode(dtComm.uIndex&0xf);
			dtComm.txbuf[15] = 0x0d;
			dtComm.txbuf[16] = 0x0a;
			DoSendData(17);
		}
	}
}
/************************************************************
*function		: DoReadnRegister2
*description	: calcare the data to checksum
*On Entry	: data string
*On Exit		: CRC value
************************************************************/
void DoReadnRegister2(void)
{
	uint16_t uchsum;
	
	if(dtComm.u16DAddr<0x1000)
	{
		dtComm.uIndex=GetMenuIndex(dtComm.u16DAddr);

		if((dtComm.uIndex+dtComm.u8DLen)<=ParmMaxItem)
		{
			dtComm.u8XLen = dtComm.u8DLen<<1;
			dtComm.txbuf[2] = dtComm.u8XLen;
			dtComm.u8XLen = 3;
			
			do{
				pAscciiWd.uData=gParcode.u16Buff[dtComm.uIndex];
				dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData>>8;
				dtComm.u8XLen++;
				dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData&0xff;
				dtComm.u8XLen++;
				dtComm.uIndex++;
				dtComm.u8DLen --;
			}while(dtComm.u8DLen>0);
		}
		else
		{
			dtComm.u8ErrCode = 2;
		}
	}
	else if(dtComm.u16DAddr>=0x2000&&dtComm.u16DAddr<=0x2004)
	{
		uint16_t tmpBuf[5];

		if(GetErrorCode()>0)
			tmpBuf[0]=8;
		else 
			tmpBuf[0]=GetOutputFlag();

		tmpBuf[1]=tCtrlOutParm.GetTempT1Val;
		tmpBuf[2]=0;
		tmpBuf[3]=0;
		tmpBuf[4]=0;
		
		dtComm.uIndex = dtComm.u16DAddr-0x2000;
		
		dtComm.u8XLen = dtComm.u8DLen<<1;
		dtComm.txbuf[2] = dtComm.u8XLen;
		dtComm.u8XLen = 3;

		do{
			pAscciiWd.uData=tmpBuf[dtComm.uIndex];
			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData>>8;
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData&0xff;
			dtComm.u8XLen++;
			dtComm.uIndex++;
			dtComm.u8DLen --;
		}while(dtComm.u8DLen>0);
	}
//    else if(dtComm.u16DAddr==0x2006)        //test
//    {
//        
//    }
	else if(dtComm.u16DAddr==0x3000)
	{
		if(dtComm.u8DLen==1)
		{
			dtComm.u8XLen = dtComm.u8DLen<<1;
			dtComm.txbuf[2] = dtComm.u8XLen;
			dtComm.u8XLen = 3;
			
			pAscciiWd.uData=GetErrorCode();
			
			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData>>8;
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData&0xff;
			dtComm.u8XLen++;
		}
		else
		{
			dtComm.u8ErrCode = 2;
		}
	}
	//added at 2025.09.01 tjh
	else if(dtComm.u16DAddr>=0x5000 && dtComm.u16DAddr<=0x5009)
	{
		if(dtComm.u8DLen==1)
		{
			dtComm.u8XLen = dtComm.u8DLen<<1;
			dtComm.txbuf[2] = dtComm.u8XLen;
			dtComm.u8XLen = 3;
			
			switch(dtComm.u16DAddr)
			{
				case 0x5000:pAscciiWd.uData=CALData.cold_temp_offset;		break;
				case 0x5001:pAscciiWd.uData=CALData.LHA7918_ZERO_OFFSET1;	break;
				case 0x5002:pAscciiWd.uData=CALData.LHA7918_ZERO_OFFSET2;	break;
				case 0x5003:pAscciiWd.uData=CALData.LHA7918_ZERO_OFFSET3;	break;
				case 0x5004:pAscciiWd.uData=CALData.LHA7918_CAL1;			break;
				case 0x5005:pAscciiWd.uData=CALData.LHA7918_CAL2;			break;
				case 0x5006:pAscciiWd.uData=CALData.LHA7918_CAL3;			break;
				case 0x5007:pAscciiWd.uData=CALData.V_ref;					break;
				case 0x5008:pAscciiWd.uData=CALData.Res_ref;				break;
				case 0x5009:pAscciiWd.uData=CALData.MCU_Vref;				break;
				default:break;
			}
			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData>>8;
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData&0xff;
			dtComm.u8XLen++;	
		}
		else 
		{
			dtComm.u8ErrCode = 2;
		}	
	}
	
	//added at 2025.08.30 tjh
	else if(dtComm.u16DAddr==0x5200 | dtComm.u16DAddr==0x5203)
	{
		if(dtComm.u8DLen==1)
		{
			dtComm.u8XLen = dtComm.u8DLen<<1;
			dtComm.txbuf[2] = dtComm.u8XLen;
			dtComm.u8XLen = 3;

			pAscciiWd.uData=LHAData.Data1;

			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData>>8;
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = pAscciiWd.uData&0xff;
			dtComm.u8XLen++;			
		}
		else 
		{
			dtComm.u8ErrCode = 2;
		}
	}
	
	else 
	{
		dtComm.u8ErrCode = 2;
	}

	if(dtComm.u8Addr>0){
		if(dtComm.u8ErrCode>0){//error...
			dtComm.txbuf[1]=0x83;
			dtComm.txbuf[2]=dtComm.u8ErrCode;
			uchsum =CalcCRCChksum(dtComm.txbuf,3);
			dtComm.txbuf[3]=uchsum&0xff;
			dtComm.txbuf[4]=uchsum>>8;
			DoSendData(5);
		}
		else{
			uchsum = CalcCRCChksum(dtComm.txbuf,dtComm.u8XLen);
			
			dtComm.txbuf[dtComm.u8XLen] = uchsum&0xff;
			dtComm.u8XLen++;
			dtComm.txbuf[dtComm.u8XLen] = uchsum>>8;
			dtComm.u8XLen++;

			DoSendData(dtComm.u8XLen);
		}
	}
}
/************************************************************
*function		: CalcLRCChksum
*description	: calcare the data to checksum
*On Entry	: data string
*On Exit		: CRC value
************************************************************/
void DoWrite1Register2(void)
{
	uint16_t uchsum;
	//run controller style ...
	//run controller style ...
	if(dtComm.u16DAddr<0x1000)
	{
		dtComm.uIndex=GetMenuIndex(dtComm.u16DAddr);

		if(dtComm.uIndex>emqPID12Kd)
			dtComm.u8ErrCode=2;
		else
		{
			if(DoSave1FromComm(dtComm.uIndex,dtComm.u16DValue))
				dtComm.u8ErrCode=3;
		}
	}
	else if(dtComm.u16DAddr==0x1000)
	{
		if(dtComm.u16DValue<2)
		{
			if(gParcode.Item.XRunSwitch==1&&gParcode.Item.CtrlMethod==3)
				SetOutputSwitch(dtComm.u16DValue);
			else
				dtComm.u8ErrCode=3;
		}
		else if(dtComm.u16DValue==8)
		{
			if(gParcode.Item.WarnReset==2)
					SetErrorCode(0);
			else
				dtComm.u8ErrCode=3;
		}
	}
	else if(dtComm.u16DAddr==0x1001)
	{
		if(gParcode.Item.CtrlMethod==3)
		{
			gParcode.Item.TempSetVal = dtComm.u16DValue;
			SetOutpTmpToRef(gParcode.Item.TempSetVal);
			DispShowFrc2(gParcode.Item.TempSetVal,1);
		}
		else
		{
			dtComm.u8ErrCode=3;
		}
	}
	else if(dtComm.u16DAddr==0x1002)
	{
		
	}
	else if(dtComm.u16DAddr==0x1003)
	{
		
	}
	else if(dtComm.u16DAddr==0x1004)
	{
		
	}
	//added at 2025.09.01 tjh
	else if(dtComm.u16DAddr >= 0x5000 && dtComm.u16DAddr <= 0x5009)
	{
		switch(dtComm.u16DAddr)
		{
			case 0x5000:CALData.cold_temp_offset = 	dtComm.u16DValue;		break;
			case 0x5001:CALData.LHA7918_ZERO_OFFSET1 = dtComm.u16DValue;	break;
			case 0x5002:CALData.LHA7918_ZERO_OFFSET2 = dtComm.u16DValue;	break;
			case 0x5003:CALData.LHA7918_ZERO_OFFSET3 = dtComm.u16DValue;	break;
			case 0x5004:CALData.LHA7918_CAL1 = dtComm.u16DValue;			break;
			case 0x5005:CALData.LHA7918_CAL2 = dtComm.u16DValue;			break;
			case 0x5006:CALData.LHA7918_CAL3 = dtComm.u16DValue;			break;
			case 0x5007:CALData.V_ref = dtComm.u16DValue;					break;
			case 0x5008:CALData.Res_ref = dtComm.u16DValue;					break;
			case 0x5009:CALData.MCU_Vref = dtComm.u16DValue;				break;
			default:break;
		}
	}
	
	//added at 2025.8.29 tjh	
	else if(dtComm.u16DAddr >= 0x5100 && dtComm.u16DAddr <= 0x5104)
	{
		if(dtComm.u16DValue)
		{
			LHAData.LHA_Step=2;
			LHAData.LHAOut=0;
			switch(dtComm.u16DAddr)
			{
				case 0x5100:LHAData.WRITE1=0x0C2B;			break;
				case 0x5101:LHAData.WRITE1=0x062B;			break;
				case 0x5102:LHAData.WRITE1=0x022B;			break;
				case 0x5103:LHAData.WRITE1=0x622B;			break;
				case 0x5104:Sensor_Mode_Select(RTD_MODE);	break;
				default:break;	
			}
		}
	}
	else
	{
		dtComm.u8ErrCode = 2;
	}
	
	if(dtComm.u8Addr>0&&dtComm.u8Flaggg==0)
	{
		if(dtComm.u8ErrCode>0){//error...
			dtComm.txbuf[1]=0x86;
			dtComm.txbuf[2]=dtComm.u8ErrCode;
			uchsum =CalcCRCChksum(dtComm.txbuf,3);
			dtComm.txbuf[3]=uchsum&0xff;
			dtComm.txbuf[4]=uchsum>>8;
			DoSendData(5);
		}
		else{
			DoSendData(dtComm.u8TxLen);
		}
	}

	dtComm.u8Flaggg = 0;
}
/************************************************************
*function		: CalcLRCChksum
*description	: calcare the data to checksum
*On Entry	: data string
*On Exit		: CRC value
************************************************************/
void DoWritenRegister2(void)
{
	uint16_t uchsum;
	
	if(dtComm.u16DAddr<0x1000){
		uint8_t utemp,u8i;
		
		dtComm.uIndex=GetMenuIndex(dtComm.u16DAddr);
		if(dtComm.uIndex==0xffff)
			dtComm.u8ErrCode=2;
		else if(dtComm.u8DLen<26&&(dtComm.uIndex+dtComm.u8DLen)<emuCheckSum0)
		{
			utemp = 7;
			for(u8i=0;u8i<dtComm.u8DLen;u8i++,utemp+=2)
				dtComm.upDat[u8i]=((uint16_t)dtComm.txbuf[utemp]<<8)|dtComm.txbuf[utemp+1];

			if(DoSavenFromComm(dtComm.uIndex,dtComm.u8DLen,dtComm.upDat))
				dtComm.u8ErrCode=7;
		}
		else dtComm.u8ErrCode=2;

	}
	else if(dtComm.u8DLen==1)
	{
		dtComm.u16DValue = (uint16_t)dtComm.txbuf[7]<<8;
		dtComm.u16DValue |= dtComm.txbuf[8];
		dtComm.u8Flaggg = 1;
		DoWrite1Register2();
	}
	else
	{
		dtComm.u8ErrCode = 2;
	}
		
	if(dtComm.u8Addr>0){
		if(dtComm.u8ErrCode>0){//error...
			dtComm.txbuf[1]=0x90;
			dtComm.txbuf[2]=dtComm.u8ErrCode;
			uchsum =CalcCRCChksum(dtComm.txbuf,3);
			dtComm.txbuf[3]=uchsum&0xff;
			dtComm.txbuf[4]=uchsum>>8;
			DoSendData(5);
		}
		else{
			uchsum=CalcCRCChksum(dtComm.txbuf,6);
			dtComm.txbuf[6]=uchsum&0xff;
			dtComm.txbuf[7]=uchsum>>8;
			DoSendData(8);
		}
	}
}

/******************************************************************************
*Function 		: void UART1_IRQHandler(void)
*Description	: It Receives datas from Interrupt Handler:
							: 1.data buffer to receive.
*******************************************************************************/
void UART1_IRQHandler(void)
{
	uint32_t ltempdat;

	ltempdat = UART1->IIR;
	
	if(ltempdat&0x04)
	{
		dtComm.rxbuf[dtComm.rxicnt]=UART1->RBR;

		if((UART1->LSR&0x04)==0)
		{
			ltempdat = UART1->LSR;
			
			dtComm.rxicnt++;
			if(dtComm.rxicnt>120)
				dtComm.rxicnt=0;
		}
	}
	else if(ltempdat&0x02)
	{
		if(dtComm.txcnt<dtComm.txlen)
		{
			UART1->THR = dtComm.txbuf[dtComm.txcnt];
			dtComm.txcnt++;
		}
		else
		{	
		// disable tx interrupt.
			UART1->IER &= 0x0d;
		}
	}
	else
	{
		ltempdat = UART1->IIR;
	}
}

