/******************************************************************************
* @file  drvForFMC.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the FMC firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/
#include "drvForMain.h"

/*****************************************************************************
 ** \brief	FMC_ErasePage
 **			FMC ҳ����
 ** \param [in] PageAddr: 
 ** \return  none
 ** \note	
*****************************************************************************/
void FMC_ErasePage(uint32_t PageAddr)
{
	__DI;
  FMC->FLERMD = 0x10;
  FMC->FLPROT = 0xF1;
  FMC->FLOPMD1 = 0x55;
  FMC->FLOPMD2 = 0xAA;  
  // Write data to start address of sector to trigger Erase Operation
  *(uint32_t *) PageAddr = 0xFFFFFFFF;
  
  // polling Erase Over Flag
  while((FMC->FLSTS & FMC_FLSTS_OVF_Msk) == 0);
  FMC->FLSTS = FMC_FLSTS_OVF_Msk;
  FMC->FLERMD = 0x00;
  FMC->FLPROT = 0xF0;

	__EI;
}

/*****************************************************************************
 ** \brief	FMC_Read
 **			FMC ��
 ** \param [in] DataAddr����ַ��λ����Ϊ0
 ** \return  32λ����
 ** \note	
*****************************************************************************/
uint32_t FMC_Read(uint32_t DataAddr)
{
#if 1
	uint32_t FlashData;
	
	FlashData = *(__IO uint32_t *)DataAddr;
#endif
	return FlashData;
}
/*****************************************************************************
 ** \brief	FMC_Write
 **			FMC д
 ** \param [in] DataAddr����ַ��λ����Ϊ0
 **				DataVlue: 32λ����
 ** \return  none
 ** \note	
*****************************************************************************/
void FMC_Write(uint32_t DataAddr, uint32_t DataVlue)
{
#if 1
	FMC->FLPROT = 0xF1;

	__DI;
  FMC->FLOPMD1 = 0xAA;
  FMC->FLOPMD2 = 0x55;  
	
  *(volatile uint32_t *)DataAddr = DataVlue; 
	
  __EI;			
  // polling OVER Flag
  while((FMC->FLSTS & FMC_FLSTS_OVF_Msk) == 0);
  FMC->FLSTS = FMC_FLSTS_OVF_Msk;

	FMC->FLPROT = 0xF0;
#endif
}
/*****************************************************************************
 ** \brief	FMC_Program
 **			FMC д
 ** \param [in] DataAddr����ַ��λ����Ϊ0
 **				DataVlue: 32λ����
 ** \return  none
 ** \note	
*****************************************************************************/
void FMC_Program(uint32_t adr, int32_t sz, uint32_t *buf) 
{
#if 1
	uint32_t i;

	FMC->FLPROT = 0xF1;

	for(i=0; i<sz; i++,adr+=4) 
  {
		__DI;
    FMC->FLOPMD1 = 0xAA;
    FMC->FLOPMD2 = 0x55;  
		
    *(uint32_t *)adr = buf[i]; 
    __EI;			
    // polling OVER Flag
    while((FMC->FLSTS & FMC_FLSTS_OVF_Msk) == 0);
    FMC->FLSTS = FMC_FLSTS_OVF_Msk;
	}

	FMC->FLPROT = 0xF0;
#endif
}

