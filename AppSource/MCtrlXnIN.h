#include "MainDefine.h"

#define IO_FILFER	40

typedef struct{
	uint8_t	InMX1HCnt;
	uint8_t	InMX1LCnt;
	uint8_t	InMX2HCnt;
	uint8_t	InMX2LCnt;
	uint8_t InnerTemp;
}XnFilterCnt;

void DoXnScanLoop(void);

uint16_t GetInnerTempFlag(void);

