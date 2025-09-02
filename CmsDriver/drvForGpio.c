/******************************************************************************
* @file  drvForGPIO.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the GPIO firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 


#include "drvForMain.h"

/*******************************************************************************
* Function	: GPIO_Configuration

* Description 	

			
* Enter para: 	
		P00:RELAY1 	P20:ADCT1	P30:IN_X1		P40:SEL6	P50:595_DO
		P01:RELAY2	P21:ADCT2	P31:IN_X2		P41:SEL5	P51:595_CK
		P02:SWDDA		P22:U1CS	P32:KEY_IN	P42:SEL4	P52:595_CS
		P03:SWDCK		P23:U1RX	P33:SEL9		P43:SEL3	P53:SPI_DO
		P04:ADCRef	P24:U1TX	P34:XXX			P44:XXX		P54:XXX
		P05:PWM_DA	P25:XXX		P35:XXX			P45:XXX		P55:XXX
		P06:TESTPN	P26:XXX		P36:SEL8		P46:SEL2	P56:SPI_DI
		P07:XXX			P27:XXX		P37:SEL7		P47:SEL1	P57:SPI_CK
			
* Exit para	: None
*******************************************************************************/
void GPIOConfiguration(void)
{
	//PMX 0:OUT 1:IN
	//PMC 0:DP  1:AP

	//0bx0xxxx00
	PORT->PM0 &= 0x3C;
	PORT->PMC0 = 0x10;
	PORT->PU0 &= 0xfc;
	PORT->PD0 &= 0xfc;

	//0bxxx01011
	PORT->PM2 &= 0xe0;
	PORT->PM2 |= 0x0B;
	PORT->PMC2 = 0x03;

	//0b00xx0111
	PORT->PM3 &= 0x30;
	PORT->PM3 |= 0x07;
	PORT->PMC3 = 0x00;
	//pull down
	PORT->PD3 = 0x04;
	
	//0b00xx0000
	PORT->PM4 &= 0x30;
	PORT->PMC4 = 0x00;

	//0b01xx0000
	PORT->PM5 &= 0x30;
	PORT->PM5 |= 0x40;
	PORT->PMC5 = 0x00;
}

/*********************************/
//void GPIOResetOutput(void)
/*********************************/
void GPIOResetOutput(void)
{
	PORT->PCLR0 = 0x43;
	PORT->PCLR2 = 0x14;
	PORT->PCLR3 = 0xc8;
	PORT->PCLR4 = 0xff;
	PORT->PCLR5 = 0xff;
}

