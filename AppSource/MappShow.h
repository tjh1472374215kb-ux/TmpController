
#include "MainDefine.h"

typedef struct{
	uint8_t BitDot1;
	uint8_t BitDot2;
	uint8_t BitTemp;
	uint8_t BitLock;
	uint8_t BitComm;
}ShowBit;

void DoLedBitScan(void);

void SetTempFlag(uint8_t);
void SetLockFlag(uint8_t);
void SetCommFlag(uint8_t);
void SetOut1Flag(uint8_t);
void SetOut2Flag(uint8_t);
void SetAlamFlag(uint8_t);

uint8_t GetTempFlag(void);

void DispShowLedx(uint8_t);
void DispShowNum1(long);
void DispShowNum2(long);
void DispShowFrc1(long,uint16_t);
void DispShowFrc2(long,uint16_t);

void DispMenuNum1(long num);
void DispMenuNum2(long num);

void DispMenuPswd(uint16_t);
void DispMenuFunc(uint16_t);
void DispMenuEnd(uint16_t);
void DispErrorCode(uint16_t);
void DispLogoCode(void);
void DispMenuOff1(void);
void DispMenuOff2(void);



