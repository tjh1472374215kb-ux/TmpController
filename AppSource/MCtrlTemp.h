#include "MainDefine.h"

#define TempMaxPU		24000 //2400.0
#define TempMinPU		3000 	//-300.0

typedef struct
{
	int16_t	qOutTmp; 	 							   
	int16_t	qSetTmp;
	
	int16_t	qRefTmp;
	int16_t	qFedTmp;
	
	int16_t	qAccTmp;
	int16_t	qDecTmp;
	
	int16_t	dMaxLmt;
	int16_t	dMinLmt;
	
	int16_t	Equation;
	int16_t MaxTmpPU;
	int16_t MaxOutPU;
}Temp_Ramp;


extern Temp_Ramp tHeatRamp,tCoolRamp;

void InitTempRamp(void);
void CalcTempRamp(Temp_Ramp* v);

void SetOutpTmpToRef(int16_t tmp);
void SetRxINTmpToFed(int16_t tmp);
void SetRxINTmpToSub(int16_t tmp);

void SetHeatTmpToRef(int16_t tmp);
void SetHeatTmpToFed(int16_t tmp);
void SetCoolTmpToRef(int16_t tmp);
void SetCoolTmpToFed(int16_t tmp);

