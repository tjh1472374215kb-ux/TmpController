/******************************************************************************
* @file  drvForSPI.c
* @author Of JSCC
* @version  V1.0.0
* @date  05/06/2025
* @brief  This file provides all the LED&KEY firmware functions.
******************************************************************************
* <h2><center>&copy; COPYRIGHT 2025 xiaots</center></h2>
*/ 

#include "drvForMain.h"

/******************************************/
//void SPI_WR_Delay(void)
/******************************************/
void LHA_WR_Delay(void)
{
	uint8_t count=3;
	while(count--);
}
/************************************************************
*function		: LHA_WR_Data
*description	: ��LHA7918оƬд�����ݣ�������LHA�Ļض�ֵ
*On Entry	    : W_Data :(1)TC1
                          (2)TC2
                          (3)TC1V2
                          (4)TC2V2
                          (5)RTD1P
                          (6)RTD1N
                          (7)RTD2P
                          (8)RTD2N
*On Exit		: 
************************************************************/
uint16_t LHA_WR_Data(uint16_t W_Data)
{
	uint16_t ui,R_Data=0;

	for(ui=0;ui<16;ui++)
	{
		LHA_WR_Delay();
		LHA_SCLK_H;
		LHA_WR_Delay();
		
		if(W_Data&(0x8000>>ui))
			LHA_MOSI_H;
		else
			LHA_MOSI_L;

		if(LHA_MISO_I)
			R_Data |= (0x8000>>ui);
		
		LHA_WR_Delay();
		LHA_SCLK_L;
		LHA_WR_Delay();
	}
	
	return R_Data;
}
/************************************************************
*function		: Sensor_Mode_Select
*description	: �����ȵ�ż���ȵ����ģʽѡ��STE1 SET2 �Ŀ��ϣ�
*On Entry	    : mode :(1)TC_MODE
                        (2)RTD_MODE
*On Exit		: 
************************************************************/
void Sensor_Mode_Select(uint8_t mode)
{
    
    if(mode == TC_MODE)                     //�ر�SET1 SET2����
    {
        DBG->DBGSTOPCR = 0;                 //����SWD�ӿ�
    }
    else if(mode == RTD_MODE)               //��SET1 SET2����
    {
        DBG->DBGSTOPCR |= 1 << 24;          //����SWD�ӿ�
        PORT->PM0 &= 0xF3;                  //P02 P03��������˿�
        PORT->PCLR0 = 0x0C;                 //P02 P03���õ͵�ƽ
    }
    
}


