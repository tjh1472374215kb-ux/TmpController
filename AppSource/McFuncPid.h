
#include "MainDefine.h"

#define PidQBit	15

typedef struct
{
	uint16_t iKp;								
	uint16_t iKi;
	
	int16_t qRef;   			 
	int16_t qFdb;
	int16_t qErr;

	int16_t  qSatErr;
	int16_t  qOutput;   			
	int16_t  SatFlag;
	
	int32_t	qUp;
	int32_t	qUi;
	
	int32_t  qOutSat; 		
	int32_t  qOutMax;		      
	int32_t  qOutMin;	    	     
	
} tPICParm;

typedef struct
{
	int16_t qRef;   			 
	int16_t qFdb;
	int16_t qErr;
	
	uint16_t	iKP;
	uint16_t	iKI;
	uint16_t	iKD;
	
	int16_t	qErrP0;
	int16_t	qErrI0;
	int16_t	qErrD0;
	int16_t	qErrP1;
	int16_t	qErrD1;
	int16_t	qErrD2;
	int16_t	qOutput;

	int16_t	qRangeH;
	int16_t	qRangeL;
	int16_t	qErrMax;
	int16_t	qErrMin;
}tPIDParm;

extern tPICParm tHeatPic,tCoolPic;

extern tPICParm tSubT2Pic;

void InitPidParm(void);
void CalcPicFunc(tPICParm *v);
void CalcPidFunc(tPIDParm *v);

