

#include "CMS32M67xx.h"

#define OSC_EXTERNAL_CLOCK	0

#define SysClockFreq	64000000UL

#define PeriodPwm SysClockFreq/1000

#define FMC_Address0	0x00500200UL
#define FMC_Address1	0x00500400UL
#define FMC_Address2	0x0001FC00UL

#define SCAN_SEL1_H	(PORT->PSET4 = 0x80)
#define SCAN_SEL1_L	(PORT->PCLR4 = 0x80)

#define SCAN_SEL2_H	(PORT->PSET4 = 0x40)
#define SCAN_SEL2_L	(PORT->PCLR4 = 0x40)

#define SCAN_SEL3_H	(PORT->PSET4 = 0x08)
#define SCAN_SEL3_L	(PORT->PCLR4 = 0x08)

#define SCAN_SEL4_H	(PORT->PSET4 = 0x04)
#define SCAN_SEL4_L	(PORT->PCLR4 = 0x04)

#define SCAN_SEL5_H	(PORT->PSET4 = 0x02)
#define SCAN_SEL5_L	(PORT->PCLR4 = 0x02)

#define SCAN_SEL6_H	(PORT->PSET4 = 0x01)
#define SCAN_SEL6_L	(PORT->PCLR4 = 0x01)

#define SCAN_SEL7_H	(PORT->PSET3 = 0x80)
#define SCAN_SEL7_L	(PORT->PCLR3 = 0x80)

#define SCAN_SEL8_H	(PORT->PSET3 = 0x40)
#define SCAN_SEL8_L	(PORT->PCLR3 = 0x40)

#define SCAN_SEL9_H	(PORT->PSET3 = 0x08)
#define SCAN_SEL9_L	(PORT->PCLR3 = 0x08)

#define P_KEY_IN		(PORT->P3&0x04)

#define HC_SPICS_H	(PORT->PSET5 = 0x04)
#define HC_SPICS_L	(PORT->PCLR5 = 0x04)

#define HC_SPIDO_H	(PORT->PSET5 = 0x01)
#define HC_SPIDO_L	(PORT->PCLR5 = 0x01)

#define HC_SPICK_H	(PORT->PSET5 = 0x02)
#define HC_SPICK_L	(PORT->PCLR5 = 0x02)

#define LHA_SCLK_H	(PORT->PSET5 = 0x80)
#define LHA_SCLK_L	(PORT->PCLR5 = 0x80)

#define LHA_MOSI_H	(PORT->PSET5 = 0x08)
#define LHA_MOSI_L	(PORT->PCLR5 = 0x08)

#define LHA_MISO_I	(PORT->P5&0x40)

#define P_U1COM_CS_H 	(PORT->PSET2 = 0x04)
#define P_U1COM_CS_L 	(PORT->PCLR2 = 0x04)

#define P_RELAY1_H 	(PORT->PSET0 = 0x01)
#define P_RELAY1_L 	(PORT->PCLR0 = 0x01)

#define P_RELAY2_H 	(PORT->PSET0 = 0x02)
#define P_RELAY2_L 	(PORT->PCLR0 = 0x02)

#define P_X1_IN			(PORT->P3&0x01)
#define P_X2_IN			(PORT->P3&0x02)



//Ä£Ê½×ª»»
#define TC_MODE     0x01
#define RTD_MODE    0x02


void SYSConfiguration(void);
void NVICConfiguration(void);
void GPIOConfiguration(void);
void ADCConfiguration(void);
void TMRConfiguration(void);
void UARTConfiguration(void);
void LVDConfiguration(void);
void CCPConfiguration(void);

void GPIOResetOutput(void);


void FMC_ErasePage(uint32_t PageAddr);
uint32_t FMC_Read(uint32_t DataAddr);
void FMC_Write(uint32_t DataAddr, uint32_t DataVlue);
void FMC_Program(uint32_t adr, int32_t sz, uint32_t *buf);


void DoLedKeyScan(void);
uint8_t GetMsgIfKeyIn(void);
uint8_t GetMsgKeyData(void);
uint16_t GetMsgKeyCnt(void);

