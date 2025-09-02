#include "MainDefine.h"

typedef struct {
    uint8_t u250usTick;
    uint8_t u250usCount;
	uint8_t uTimeTick;
	uint8_t	uTimeStep;
	uint8_t u802MS;
	uint8_t u810MS;
	uint8_t u8100MS;
	uint8_t u81000MS;
	uint8_t	u8Tm1Cnt;
	uint8_t	u8Tm2Cnt;
	uint8_t	u8Tm3Cnt;
}tSysTime;

typedef struct{
	uint8_t U8PwrOn;
	uint8_t U8ErrSlip;
	uint8_t U8InitStp;
	uint8_t u8CheckOK;
	uint8_t u8CheckER;
	uint16_t uErrorCnt;
	uint16_t uErrorSta;
	uint16_t uErrorRnd;
	uint16_t uErrorRct;
	uint16_t uErrorNum;

	uint16_t LHAInitTmr;
	
	uint16_t U8InitDly;
	uint16_t uError;
	uint16_t uWarning;
	uint16_t uReset;
	uint16_t uIndex;
	uint16_t uCheckR1;
	uint16_t uCheckR2;
	uint16_t strBuf[64];
}tMainPara;

extern tMainPara dtMain;

void MappTimeCnt(void);
void MappPowerOn(void);

void ResetErrCode(void);
void SetErrorCode(uint16_t);
uint16_t GetErrorCode(void);

