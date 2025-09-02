/******************************************************************************
* @file  drvForMain.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the drivers firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "drvForMain.h"
#include "drvForCgc.h"

#define CGC_PER0Periph_TIM4             ((uint8_t)0x01) /*!< CGC PER0: TIM4EN (Bit 0)       */
#define CGC_PER0Periph_IIC              ((uint8_t)0x10) /*!< CGC PER0: TIM4EN (Bit 0)       */
#define CGC_PER0Periph_LSITIMER         ((uint8_t)0x80) /*!< CGC PER0: LSITIMEREN  (Bit 7)       */

#define CGC_PER10Periph_DMA             ((uint8_t)0x08) /*!< CGC PER0: DMAEN (Bit 3)       */

#define CGC_PER11Periph_TIMER01         ((uint8_t)0x01) /*!< CGC PER1: TIMER01EN   (Bit 0)     */
#define CGC_PER11Periph_EPWM            ((uint8_t)0x08) /*!< CGC PER1: EPWMEN    (Bit 3)     */
#define CGC_PER11Periph_CCP           	((uint8_t)0x10) /*!< CGC PER1: CCPEN (Bit 4)     */

#define CGC_PER12Periph_DIVSQRT         ((uint8_t)0x01) /*!< CGC PER2: DIVEN   (Bit 0)     */
#define CGC_PER12Periph_SPI             ((uint8_t)0x02) /*!< CGC PER2: SPIEN    (Bit 1)     */
#define CGC_PER12Periph_UART0           ((uint8_t)0x04) /*!< CGC PER2: UARTEN (Bit 2)     */
#define CGC_PER12Periph_UART1           ((uint8_t)0x08) /*!< CGC PER2: UART1EN (Bit 3)     */
#define CGC_PER12Periph_DIV             ((uint8_t)0x20) /*!< CGC PER2: UARTEN (Bit 5)     */
#define CGC_PER12Periph_HALL            ((uint8_t)0x40) /*!< CGC PER2: UARTEN (Bit 6)     */
#define CGC_PER12Periph_TEST            ((uint8_t)0x80) /*!< CGC PER2: UARTEN (Bit 7)     */


#define CGC_PER13Periph_ADCEN           ((uint8_t)0x01) /*!< CGC PER3: ADCEN (Bit 0)       */
#define CGC_PER13Periph_ACMPEN          ((uint8_t)0x02) /*!< CGC PER3: ACMPEN (Bit 1)       */
#define CGC_PER13Periph_PGAEN           ((uint8_t)0x08) /*!< CGC PER3: PGA0EN (Bit 3)       */
//#define CGC_PER13Periph_PGA1EN           ((uint8_t)0x10) /*!< CGC PER3: PGA1EN (Bit 4)       */
//#define CGC_PER13Periph_PGA2EN           ((uint8_t)0x10) /*!< CGC PER3: PGA2EN (Bit 4)       */
#define CGC_PER13Periph_DAC           	((uint8_t)0x20) /*!< CGC PER3: DACEN (Bit 5)     */
#define CGC_PER13Periph_ADCLDO          ((uint8_t)0x40) /*!< CGC PER3: ADCLDOEN    (Bit 6)     */


/*********************************/
//void SYSConfiguration(void)
/*********************************/
void SYSConfiguration(void)
{
#if OSC_EXTERNAL_CLOCK
	uint8_t Temp;

	Temp = CGC->CMC;
	Temp &= ~0x3f;
	Temp |= 1UL << CGC_CMC_OSCSEL1_Pos;
	CGC->CMC = Temp;

	//CGC->OSTS = 0x02;
	
	CGC->CSC &= ~CGC_CSC_MSTOP_Msk;

	//while(CGC->OSTC<=0xe0);
	
	CGC->CKC |= CGC_CKC_MCM0_Msk ;

	while((CGC->CKC & CGC_CKC_MCS_Msk) == 1);

	CGC->PLLCR |= PLL_SR_fMX << CGC_PLLCR_PLLCLKSEL_Pos; 	/* PLL ON = 1 */
	CGC->PLLCR |= PLL_MUL_8 << CGC_PLLCR_PLLP_Pos; 
	CGC->MCKC |= CGC_MCLK_PLL << CGC_MCKC_MCKSEL_Pos; 	/* PLL DIV */

	CGC->PLLCR |= CGC_PLLCR_PLLON_Msk; 	/* PLL ON = 1 */
	while((CGC->MCKC & CGC_MCKC_PLLCKSTR_Msk) != _80_CGC_UPLLSR_fMX); 

	CGC->CKC &= ~CGC_CKC_MCM0_Msk ;
#endif

	//RESINB as GPIO
	CGC->RSTM = 1;

	CGC->PER11 |= CGC_PER11Periph_TIMER01|CGC_PER11Periph_CCP;
	CGC->PER12 |= CGC_PER12Periph_UART1|CGC_PER12Periph_DIV;
	CGC->PER13 |= CGC_PER13Periph_ADCEN;
}

/*********************************/
//void TMRConfiguration(void)
/*********************************/
void TMRConfiguration(void)
{
	//2000hz = 0.5ms
	SysTick_Config(SysClockFreq/4000);

	//设置加载周期 100HZ
	TMR0->LOAD = SysClockFreq/100;
	//使能中断
	TMR0->CON = 0x0e2;
}

/*********************************/
//void LVDConfiguration(void)
/*********************************/
void LVDConfiguration(void)
{
	LVD->LVIM = 0x83;
	LVD->LVIS = 0X01;

	NVIC_EnableIRQ(LVI_IRQn);
	NVIC_SetPriority(LVI_IRQn,3);
}

/*********************************/
//void NVICConfiguration(void)
/*********************************/
void NVICConfiguration(void)
{
	NVIC_EnableIRQ(TIMER0_IRQn);
  NVIC_SetPriority(TIMER0_IRQn, 2); 
	
	NVIC_EnableIRQ(UART1_IRQn);
	NVIC_SetPriority(UART1_IRQn,4);
}

