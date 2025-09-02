#include "MainDefine.h"

typedef struct{
	uint8_t rxicnt;
	uint8_t rxocnt;
	uint8_t rxbuf[128];
	uint8_t txcnt;
	uint8_t txlen;
	uint8_t txbuf[128];
	uint8_t u8State;
	uint8_t u8Flaggg;
	uint8_t u8DataIn;
	uint8_t u8TxLen;
	
	uint8_t u8Addr;
	uint8_t u8Cmd;
	uint8_t u8Sum;
	uint8_t u8DLen;
	uint8_t u8XLen;
	uint8_t u8ErrCode;

	uint16_t uRTUtim0;
	uint16_t uRTUtim1;
	uint16_t uRTUtim2;
	uint16_t uErrCnt;
	uint16_t uIndex;
	uint16_t u16Sum;
	uint16_t u16DAddr;
	uint16_t u16DValue;
	uint16_t upDat[36];

	//added at 2025.08.28 tjh
	uint16_t u16LHAConMSB;
	uint16_t u16LHAConLSB;
	uint32_t u32LHAConReg;
}COMM_DATA;

typedef union{
	uint16_t uData;
	
	struct{
		unsigned Bit04:4;
		unsigned Bit08:4;
		unsigned Bit12:4;
		unsigned Bit16:4;
	}B;
}AscciiWord;

extern COMM_DATA dtComm;

void InitModbusPar(void);
void DoModbusScan(void);
void Do485Time10MS(void);
void MappUxComTime(void);

uint8_t GetASCiiCode(uint8_t u8buf);
uint8_t GetASCiiData(uint8_t u8buf);
uint8_t GetASCiiByte(uint8_t *u8buf);
uint16_t GetASCiiWord(uint8_t *u8buf);
uint8_t CalcLRCChksum(uint8_t *u8buf,uint8_t u8len);
uint16_t CalcCRCChksum(uint8_t *u8buf,uint8_t u8len);

void DoReadnRegister1(void);
void DoWrite1Register1(void);
void DoWritenRegister1(void);
void DoReadnRegister2(void);
void DoWrite1Register2(void);
void DoWritenRegister2(void);

void DoModbusAscciiCmd(void);
void DoModbusRTUCmd(void);

void SetCommXXFormat(void);
void SetCommBaudrate(void);
void DoSendData(uint16_t len);

extern void SetErrorCode(uint16_t);

