#include "MainDefine.h"


#define  TC1     0x0C2B		//AINP=AIN0,AINN=AIN1,64mV
#define  TC2     0x3C2B		//AINP=AIN2,AINN=AIN3,64mV
#define  TC1V2   0x5C2B		//AINP=AIN1,AINN=GND,64mV	
#define  TC2V2   0x7C2B		//AINP=AIN3,AINN=GND,64mV
#define  RTD1P   0x4C2B     //AINP=AIN0,AINN=GND,64mV 		//0 100 1100 0010 1011
#define  RTD1N   0x5C2B     //AINP=AIN1,AINN=GND,64mV		//0 101 1100 0010 1011
#define  RTD2P   0x6C2B     //AINP=AIN2,AINN=GND,64mV	 	//0 110 1100 0010 1011
#define  RTD2N   0x7C2B     //AINP=AIN3,AINN=GND,64mV		//0 111 1100 0010 1011

//WF72\WF48ÏµÁĞÀä¶ËÎÂ¶È
#define  LHACOLD 0x362B     //AINP=AIN2,AINN=AIN3,1024mV	//0 011 0110 0010 1011


#define  TempCtrlGroup1   TempCtrlType==TempCtrlWF48||TempCtrlType==TempCtrlWF48C||\
                          TempCtrlType==TempCtrlWF72||TempCtrlType==TempCtrlWF72R
                          
#define  TempCtrlGroup2   TempCtrlType==TempCtrlWF96||TempCtrlType==TempCtrlWF96R||\
                          TempCtrlType==TempCtrlWF49||TempCtrlType==TempCtrlWF49R||\
                          TempCtrlType==TempCtrlWK02||TempCtrlType==TempCtrlWK04
                          
#define  TempCtrlGroup3   TempCtrlType==TempCtrlWT03||TempCtrlType==TempCtrlWT07


//LHA×îµÍÓĞĞ§Î»´óĞ¡
#define  LHA_FSR_64     0.0019531
#define  LHA_FSR_1024   0.03125



//Àä¶ËÁéÃô¶È
#if(TempCtrlGroup1)
    #define  T_V_SENS  -1.96
#elif(TempCtrlGroup2)
    #define  T_V_SENS  -14.4
#else //if(TempCtrlGroup3)
    #define  T_V_SENS  -8.64
#endif


#define	  ADC_IS_BUSY()		((ADC->CON2&0x0080)? 1:0)
#define   WINDOW_SIZE        7

typedef struct{
    uint8_t   LHA_Step;
	uint8_t   LHAOut;
	uint8_t   LHALBCnt;
	uint8_t   TempPoint;
	uint8_t   thermotype;
    uint8_t   CascadeSwon;
    uint8_t   RTDInit;
    uint8_t   TCInit;
    
    
    int16_t   Data1;
    int16_t   Data2;
    int16_t   Data3;
    int16_t   Data4;
    int16_t   RegTemp;
    int32_t   RegSum;
    uint16_t  RegData[16];
	
    uint32_t  WRITE0;		//ä¸Šç”µç¬¬ä¸€æ¬¡æµ‹è¯•ï¼Œç”¨æ¥åˆ¤æ–­æ˜¯å¦æŠ¥é”™
    uint32_t  WRITE1;		//çƒ­ç”µå¶--åŒç«¯--T1				//çƒ­ç”µé˜»çš„T1+
    uint32_t  WRITE2;		//çƒ­ç”µå¶--åŒç«¯--T2ï¼ˆä¸²çº§ï¼‰			//çƒ­ç”µé˜»çš„T1-
    uint32_t  WRITE3;		//çƒ­ç”µå¶--åŒç«¯--T2+ï¼ˆä¸²çº§ï¼‰	//çƒ­ç”µé˜»--å•ç«¯--T2+ï¼ˆä¸²çº§ï¼‰
    uint32_t  WRITE4;		//çƒ­ç”µå¶--åŒç«¯--T2-ï¼ˆä¸²çº§ï¼‰	//çƒ­ç”µé˜»--å•ç«¯--T2-ï¼ˆä¸²çº§ï¼‰
    
    int32_t   Vreg1;        //ÈÈµç×è1µÄV1       ÈÈµçÅ¼1µÄ£¨V1-V2£©´æÔÚÕı¸ººÅ
    int32_t   Vreg2;        //ÈÈµç×è1µÄV2       ÈÈµçÅ¼2µÄ£¨V1-V2£©´æÔÚÕı¸ººÅ
    int32_t   Vreg3;        //ÈÈµç×è2µÄV1
    int32_t   Vreg4;        //ÈÈµç×è2µÄV2
    
    uint8_t   GetVreg2Flag;
    uint16_t  GetTC1Vreg2;
    uint16_t  GetTC2Vreg2;
    
    int16_t   TC1MesTemp;   //ÈÈµçÅ¼1²âÁ¿ÎÂ¶È£¨·Å´ó10±¶£©
    int16_t   TC2MesTemp;   //ÈÈµçÅ¼2²âÁ¿ÎÂ¶È£¨·Å´ó10±¶£©
    int16_t   RTD1MesTemp;  //ÈÈµç×è1²âÁ¿ÎÂ¶È£¨·Å´ó10±¶£©
    int16_t   RTD2MesTemp;  //ÈÈµç×è2²âÁ¿ÎÂ¶È£¨·Å´ó10±¶£©
    
    int16_t   T1ShowTemp;   //T1µÄÏÔÊ¾ÎÂ¶È£¨½öÏÔÊ¾0.0ºÍ0.5Á½ÖÖ×´Ì¬£©
    uint8_t   T1ShowTempBit;//T1µÄÏÔÊ¾ÎÂ¶ÈĞ¡Êıµã±êÖ¾Î»
    
	float     RTD1Value;    //ÈÈµç×è1×èÖµ
    float     RTD2Value;    //ÈÈµç×è2×èÖµ
}tLHAData;

extern tLHAData LHAData;

typedef struct{
    uint8_t   ADC_Step;
    //uint8_t   ADC_Finish;
	uint8_t   Cnt;
    uint8_t   LB_Count;
    uint8_t   LB_Time_Tick;
    uint16_t  CurrentColdVol;   //µ±Ç°µÄÀä¶ËµçÑ¹Öµ
    uint16_t  StandColdData;    //25¡æÊ±±ê¶¨µÄÀä¶ËµçÑ¹²É¼¯Öµ
    uint16_t  StandColdVol;     //¸ù¾İ±ê¶¨µÄ²É¼¯Öµ¼ÆËã³öµÄµçÑ¹Öµ
    uint16_t  FLASHColdData;    //FLASHÖĞ¶ÁÈ¡µÄ±ê¶¨²É¼¯Öµ
	
//	uint16_t  RefData [8];
//  uint16_t  TData   [8];
//	uint16_t  ColdData[8];
	
	uint16_t  RefDataLB;
	uint16_t  ColdDataLB;
	uint16_t  TDataLB;
	
	uint32_t  RefDataSum;
    uint32_t  TDataSum;
	uint32_t  ColdDataSum;
	
    uint8_t   ntcPN;
	uint16_t  Rntc;
	float     ColdMesTemp;     //Î´·Å´ó
	float     NTCMesTemp;      //Î´·Å´ó

}tADCData;

extern tADCData ADCData;

typedef struct{
    float Ygain;
    float Kgain;
    float Bgain;
    
}tGainErrorData;

extern tGainErrorData GainErrorData;

typedef struct{
    int8_t    cold_temp_offset;             //-50~50
    int8_t    LHA7918_ZERO_OFFSET1;         //-20~20
    int8_t    LHA7918_ZERO_OFFSET2;         //-20~20
    int8_t    LHA7918_ZERO_OFFSET3;         //-20~20
    uint16_t  LHA7918_CAL1;                 //32440~33096
    uint16_t  LHA7918_CAL2;                 //32440~33096
    uint16_t  LHA7918_CAL3;                 //32440~33096
    uint16_t  V_ref;                        //3252~3384
    uint16_t  Res_ref;                      //23640~24360
    uint16_t  MCU_Vref;                     //2470~2520
    
}tCALData;

extern tCALData CALData;

typedef struct{
    int16_t   LHA_AIN0_AIN1;                //LHA_AIN0_AIN1è½¬æ¢å€¼
    int32_t   LHA_AIN0_AIN1_TCR;            //LHA_AIN0_AIN1è½¬æ¢å€¼ç»è¿‡æ¸©æ¼‚è¡¥å¿
    int32_t   LHA_AIN0_AIN1_ACC;            //LHA_AIN0_AIN1è½¬æ¢å€¼ç»è¿‡ç²¾åº¦ä¿®æ­£
    int16_t   LHA_AIN0_GND;                 //LHA_AIN0_GNDè½¬æ¢å€¼
    int32_t   LHA_AIN0_GND_TCR;             //LHA_AIN0_GNDè½¬æ¢å€¼ç»è¿‡æ¸©æ¼‚è¡¥å¿
    int32_t   LHA_AIN0_GND_ACC;             //LHA_AIN0_GNDè½¬æ¢å€¼ç»è¿‡ç²¾åº¦ä¿®æ­£
    uint16_t  Res_value;                    //çƒ­ç”µé˜»æµ‹é‡å€¼
    
}tCONVData;

extern tCONVData CONVData;



extern int16_t RxTempRange[][2];

void LHA7918_Init(void);
void LHA7918_Read(void);
void LHA7918_LB(void);
void LHA_Config_Set(void);

uint32_t PostArray(uint16_t );
uint32_t NegatArray(uint16_t );
uint32_t NtcArray(uint16_t index);
uint16_t CheckTableP(uint16_t);
uint16_t CheckTableN(uint16_t);
int16_t  CheckTableNTC(uint32_t CheckRes);

void CalcTempValue(void);
int16_t  T1TempValue(void);
int16_t  T2TempValue(void);

void TC_Panel_Calc(void);
void RTD_Panel_Calc(void);

void ADCPanelUpdate(void);
void MCUADC_Read(void);
void MCUADC_LB(void);
void ADCValueProcess(void);
void TempErrorWarn(void);
void GainErrorCorrect(float temperture);
void CalcT1ShowTemp(void);

uint16_t MAF_AddData(uint16_t new_value);

