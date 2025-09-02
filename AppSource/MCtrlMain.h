#include "MainDefine.h"


#include "MCtrlTemp.h"
#include "MCtrlOutp.h"
#include "McFuncPid.h"
#include "MCtrlRxIN.h"
#include "MCtrlXnIN.h"
#include "MCtrlWarn.h"

typedef struct
{
	uint8_t	OutpSwitch;
	uint8_t	HeatCoolSw;
	
	uint16_t	HeatPeriod;
	uint16_t	CoolPeriod;
	
	uint16_t HeatPwmSwOn;
	uint16_t HeatPwmHCnt;
	uint16_t HeatPwmLCnt;

	uint16_t CoolPwmSwOn;
	uint16_t CoolPwmHCnt;
	uint16_t CoolPwmLCnt;

	int16_t	 SetTempValue;
	int16_t	 GetTempT1Val;
	int16_t	 GetTempT2Val;
	int16_t	 qTempT2Value;
	
	int16_t	 qUpErrWarn;
	int16_t	 qDnErrWarn;
	int16_t	 qHeatCoolErr;
}CtrlOut_PARM;

extern CtrlOut_PARM tCtrlOutParm;

void DoMainCtrlLoop(void);

void CalcHeatTmpOutput(void);
void CalcCoolTmpOutput(void);

void SetUpDnErrWarnQv(void);
void SetHeatCoolErrQv(void);

void SetOutputSwitch(uint16_t);

uint16_t GetOutputFlag(void);

extern void SetErrorCode(uint16_t);
extern uint16_t GetErrorCode(void);




