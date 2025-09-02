#include "MCtrlMain.h"
#include "MappMenu.h"
#include "MappCode.h"

int16_t RxTempRange[][2]={
	{-200,1300},//K
	{-200, 800},//E
	{-200,1100},//J
	{-200, 400}, //T
	{-200,1300},//N
	{0,	  1700},//S
	{0,   1700},//R
	{100, 1800},//B
	{0, 	 0},//nc
	{-200, 600},//PT100
	{-80,  300} //Ni120
};

//arrlenth分别保存正负表的长度
//                          K\  E\  J\   T\   N\  S\   R\   B\    Pt100\ Ni120	
uint16_t arrPlenth[] = {0,1373,801,1101,401,1301,1701,1701,1771, 0, 601,  301};
uint16_t arrNlenth[] = {0, 201,  0, 101,201, 201,   0,   0,   0, 0, 201,   80};



#include "MCtrlRxTab.c"

tLHAData LHAData;
tADCData ADCData;
tGainErrorData GainErrorData;
tCALData CALData;
tCONVData CONVData;



extern void ADC_Start(void);
extern void Sensor_Mode_Select(uint8_t mode);
extern uint16_t LHA_WR_Data(uint16_t W_Data);
extern uint32_t ADC_GetResult(uint32_t Channel);

float Cold_Vol_Calc(void);



void LHA7918_Init(void)
{
    LHAData.thermotype  = gParcode.Item.SensorType;
    LHAData.CascadeSwon = gParcode.Item.CascadeSwon;
    
    
    if(LHAData.thermotype < 9)
    {
        Sensor_Mode_Select(TC_MODE);    //选择热电偶模式
        
#if(TempCtrlGroup1)
        LHAData.WRITE0 = TC1V2;
        LHAData.WRITE1 = TC1;
        LHAData.WRITE2 = LHACOLD;
#else //if(TempCtrlGroup2||TempCtrlGroup3)
        LHAData.WRITE0 = TC1V2;
        LHAData.WRITE1 = TC1;
        if(LHAData.CascadeSwon == 2) LHAData.WRITE2 = TC2;
#endif
        
        LHAData.TC1MesTemp = 0;
        LHAData.TC2MesTemp = 0;
        LHAData.TCInit = 0;
        LHAData.GetVreg2Flag = 0;
        LHAData.LHA_Step = 0;
		
	
    }
    else
    {
        Sensor_Mode_Select(RTD_MODE);   //选择热电阻模式
        
#if(TempCtrlGroup1)
        LHAData.WRITE1 = RTD1P;
        LHAData.WRITE2 = RTD1N;
        LHAData.WRITE3 = LHACOLD;
#else //if(TempCtrlGroup2||TempCtrlGroup3)
        LHAData.WRITE1 = RTD1P;
        LHAData.WRITE2 = RTD1N;
        if(LHAData.CascadeSwon == 2)
        {
            LHAData.WRITE3 = RTD2P;
            LHAData.WRITE4 = RTD2N;
        }
#endif
        
        LHAData.RTD1MesTemp = 0;
        LHAData.RTD2MesTemp = 0;
        LHAData.RTDInit = 0;
        LHAData.LHA_Step = 2;
    }

	//added at 2025.09.01 tjh
	CALData.cold_temp_offset = 0;
	CALData.LHA7918_ZERO_OFFSET1 = 0;
	CALData.LHA7918_ZERO_OFFSET2 = 0;
	CALData.LHA7918_ZERO_OFFSET3 = 0;
	CALData.LHA7918_CAL1 = 32768;
	CALData.LHA7918_CAL2 = 32768;
	CALData.LHA7918_CAL3 = 32768;
	CALData.V_ref = 3318;
	CALData.Res_ref = 24000;
	CALData.MCU_Vref = 2495;	

	
    LHA_WR_Data(TC1);       //初始化默认值，防止刚上电时 LHA_MISO_I 不置低电平
    LHA_WR_Data(0);
    
    ADC_Start();            //使能ADC开始转换
}

void LHA7918_Read(void)
{
#if(TempCtrlGroup1)
    
    if(!LHA_MISO_I)
    {
        switch(LHAData.LHA_Step)
        {
            case 0:
			{
                if(LHAData.thermotype > 9)
                {
                    LHAData.LHA_Step = 2;
                    break;
                }
				LHA_WR_Data(LHAData.WRITE0);        //需要测量热电偶的V2数据
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 1:     //读（舍）
			{
				LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 2:     //读（写WR1）
			{
                if(LHAData.thermotype > 9)
                {
                    LHA_WR_Data(LHAData.WRITE1);
                }
                else
                {
                    LHAData.GetTC1Vreg2 = LHA_WR_Data(LHAData.WRITE1);
                    LHAData.GetVreg2Flag = 1;
                }
                
                LHA_WR_Data(0);
                LHAData.LHA_Step++;
                break;
			}
            case 3:     //读（舍）
			{
				LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            //--------------------开始采集（1）---------------------//
            case 4:     //读
			{
				LHAData.RegData[0] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 5:     //读
			{
				LHAData.RegData[1] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 6:     //写
			{
                LHAData.RegData[2] = LHA_WR_Data(LHAData.WRITE2);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 7:     //读
			{
				LHAData.RegData[3] = LHA_WR_Data(0);
				LHA_WR_Data(0);
                LHAData.LHA_Step++;
                LHAData.LHAOut = 1;
				break;
			}
            //--------------------开始采集（2）---------------------//
            case 8:     //读
			{
				LHAData.RegData[4] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 9:     //读
			{
				LHAData.RegData[5] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 10:     //写
			{
                if(LHAData.thermotype < 9)
                    LHAData.RegData[6] = LHA_WR_Data(LHAData.WRITE1);
                else
                    LHAData.RegData[6] = LHA_WR_Data(LHAData.WRITE3);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 11:     //读
			{
				LHAData.RegData[7] = LHA_WR_Data(0);
				LHA_WR_Data(0);
                if(LHAData.thermotype < 9)
                    LHAData.LHA_Step = 4;           //传感器为热电偶
                else
                    LHAData.LHA_Step++;             //传感器为热电阻
                LHAData.TCInit = 1;                //热电偶初始化完成标志
                LHAData.LHAOut = 2;
				break;
			}
            //--------------------开始采集（3）---------------------//
            case 12:     //读
			{
				LHAData.RegData[8] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 13:     //读
			{
				LHAData.RegData[9] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 14:     //写
			{
                LHAData.RegData[10] = LHA_WR_Data(LHAData.WRITE1);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            default:     //读
			{
				LHAData.RegData[11] = LHA_WR_Data(0);
				LHA_WR_Data(0);
                LHAData.LHA_Step = 4;
                LHAData.LHAOut = 3;
                LHAData.RTDInit = 1;                //热电阻初始化完成标志
				break;
			}
        }
        
        LHA7918_LB();
    }
#else //if(TempCtrlGroup2||TempCtrlGroup3)
    if(!LHA_MISO_I)
    {
        switch(LHAData.LHA_Step)
        {
            case 0:
			{
                if(LHAData.thermotype > 9)
                {
                    LHAData.LHA_Step = 2;
                    break;
                }
				LHA_WR_Data(LHAData.WRITE0);        //需要测量热电偶的V2数据
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 1:     //读（舍）
			{
				LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
			
            case 2:     //读（写WR1）
			{
                if(LHAData.thermotype > 9)
                {
                    LHA_WR_Data(LHAData.WRITE1);
                }
                else if(LHAData.CascadeSwon == 2)
                {
                    if(LHAData.GetVreg2Flag == 0)
                    {
                        LHAData.GetTC1Vreg2 = LHA_WR_Data(0);        //读取热电偶1 V2数据
                        LHA_WR_Data(0);
                        LHAData.WRITE0 = TC2V2;
                        LHAData.GetVreg2Flag = 1;
                        LHAData.LHA_Step = 0;
                        break;
                    }
                    else
                    {
                        LHAData.GetTC2Vreg2 = LHA_WR_Data(0);        //读取热电偶2 V2数据
                        LHAData.GetVreg2Flag = 2;
                    }
                }
                else
                {
                    LHAData.GetTC1Vreg2 = LHA_WR_Data(LHAData.WRITE1);
                    LHAData.GetVreg2Flag = 1;
                }
                
                LHA_WR_Data(0);
                LHAData.LHA_Step++;
                break;
			}
            case 3:     //读（舍）
			{
				LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            //--------------------开始采集（1）---------------------//
            case 4:     //读
			{
				LHAData.RegData[0] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 5:     //读
			{
				LHAData.RegData[1] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 6:     //写
			{
				if(LHAData.thermotype < 9 && LHAData.CascadeSwon == 1)        //传感器为热电偶且未开启串级
                    LHAData.RegData[2] = LHA_WR_Data(LHAData.WRITE1);
                else
                    LHAData.RegData[2] = LHA_WR_Data(LHAData.WRITE2);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 7:     //读
			{
				LHAData.RegData[3] = LHA_WR_Data(0);
				LHA_WR_Data(0);
                if(LHAData.thermotype < 9 && LHAData.CascadeSwon == 1)        //传感器为热电偶且未开启串级
                    LHAData.LHA_Step = 4;
                else
                    LHAData.LHA_Step++;
                LHAData.LHAOut = 1;
                if(LHAData.CascadeSwon == 1)LHAData.TCInit = 1;                //热电偶初始化完成标志
				break;
			}
            //--------------------开始采集（2）---------------------//
            case 8:     //读
			{
				LHAData.RegData[4] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 9:     //读
			{
				LHAData.RegData[5] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 10:     //写
			{
                if((LHAData.thermotype < 9 && LHAData.CascadeSwon == 2) \
                || (LHAData.thermotype > 9 && LHAData.CascadeSwon == 1))        //热电偶且开启串级
                    LHAData.RegData[6] = LHA_WR_Data(LHAData.WRITE1);           //热电阻且未开启串级
                else
                    LHAData.RegData[6] = LHA_WR_Data(LHAData.WRITE3);           //热电阻且开启串级
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 11:     //读
			{
				LHAData.RegData[7] = LHA_WR_Data(0);
				LHA_WR_Data(0);
                if((LHAData.thermotype < 9 && LHAData.CascadeSwon == 2) \
                || (LHAData.thermotype > 9 && LHAData.CascadeSwon == 1))        //热电偶且开启串级
                    LHAData.LHA_Step = 4;                                       //热电阻且未开启串级
                else
                    LHAData.LHA_Step++;
                LHAData.LHAOut = 2;
                if(LHAData.CascadeSwon == 1)LHAData.RTDInit = 1;                //热电阻初始化完成标志
                if(LHAData.CascadeSwon == 2)LHAData.TCInit = 1;                 //热电偶初始化完成标志
				break;
			}
            //--------------------开始采集（3）---------------------//
            case 12:     //读
			{
				LHAData.RegData[8] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 13:     //读
			{
				LHAData.RegData[9] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 14:     //写
			{
				LHAData.RegData[10] = LHA_WR_Data(LHAData.WRITE4);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 15:     //读
			{
				LHAData.RegData[11] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
                LHAData.LHAOut = 3;
				break;
			}
            //--------------------开始采集（4）---------------------//
            case 16:     //读
			{
				LHAData.RegData[12] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 17:     //读
			{
				LHAData.RegData[13] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            case 18:     //写
			{
				LHAData.RegData[14] = LHA_WR_Data(LHAData.WRITE1);
				LHA_WR_Data(0);
				LHAData.LHA_Step++;
				break;
			}
            default:     //读
			{
				LHAData.RegData[15] = LHA_WR_Data(0);
				LHA_WR_Data(0);
				LHAData.LHA_Step = 4;
                LHAData.LHAOut = 4;
                if(LHAData.CascadeSwon == 2)LHAData.RTDInit = 1;                //热电阻初始化完成标志
				break;
			}
        }
        LHA7918_LB();
    }
#endif
}
/************************************************************
*function			: LHA原始数据滤波
*description	: 
*On Entry			: 
*On Exit			: 
************************************************************/
void LHA7918_LB(void)
{
	uint8_t i;
    LHAData.LHALBCnt = 4;
    LHAData.RegTemp = 0;
    LHAData.RegSum = 0;
    
	if(LHAData.LHAOut == 1)
	{
        for(i = 0;i < LHAData.LHALBCnt;i++)
		{
            LHAData.RegTemp = (int16_t)LHAData.RegData[i];
			LHAData.RegSum += LHAData.RegTemp;
		}
		LHAData.Data1 = (LHAData.RegSum + LHAData.LHALBCnt / 2)/ LHAData.LHALBCnt;
        LHAData.RegTemp -= GainErrorData.Ygain * LHAData.RegTemp;      //增益误差矫正
        
		LHAData.LHAOut = 0;
	}
	else if(LHAData.LHAOut == 2)
	{
		for(i = 4;i < LHAData.LHALBCnt+4;i++)
		{
            LHAData.RegTemp = (int16_t)LHAData.RegData[i];
			LHAData.RegSum += LHAData.RegTemp;
		}
		LHAData.Data2 = (LHAData.RegSum + LHAData.LHALBCnt / 2)/ LHAData.LHALBCnt;
        LHAData.RegTemp -= GainErrorData.Ygain * LHAData.RegTemp;      //增益误差矫正
        
		LHAData.LHAOut = 0;
	}
    else if(LHAData.LHAOut == 3)
	{
		for(i = 8;i < LHAData.LHALBCnt+8;i++)
		{
            LHAData.RegTemp = (int16_t)LHAData.RegData[i];
			LHAData.RegSum += LHAData.RegTemp;
		}
		LHAData.Data3 = (LHAData.RegSum + LHAData.LHALBCnt / 2)/ LHAData.LHALBCnt;
        LHAData.RegTemp -= GainErrorData.Ygain * LHAData.RegTemp;      //增益误差矫正
        
		LHAData.LHAOut = 0;
	}
    else if(LHAData.LHAOut == 4)
	{
		for(i = 12;i < LHAData.LHALBCnt+12;i++)
		{
            LHAData.RegTemp = (int16_t)LHAData.RegData[i];
			LHAData.RegSum += LHAData.RegTemp;
		}
		LHAData.Data4 = (LHAData.RegSum + LHAData.LHALBCnt / 2)/ LHAData.LHALBCnt;
        LHAData.RegTemp -= GainErrorData.Ygain * LHAData.RegTemp;      //增益误差矫正
        
		LHAData.LHAOut = 0;
	}
}
/************************************************************
*function			: 查表平滑处理
*description	: 
*On Entry			: 
*On Exit			:  
************************************************************/
uint8_t CalcPoint(uint16_t left,uint16_t right,uint16_t num)
{
    return 10*(left - num)/(left - right);
}
/************************************************************
*function			: 根据索引返回对应数组值
*description		: 根据Thermotype变更查表返回的数据
*On Entry			: u8 index：索引位 
*On Exit			: u32 查表值
************************************************************/
uint32_t PostArray(uint16_t index)
{
	switch(LHAData.thermotype)
	{
		case 1:return k_type_p[index];
		case 2:return e_type_p[index];		
		case 3:return j_type_p[index];
		case 4:return t_type_p[index];	
		case 5:return n_type_p[index];
		case 6:return s_type_p[index];	
		case 7:return r_type_p[index];
		case 8:return b_type_p[index];
//      case 9:
		case 10:return pt100_p[index];
		case 11:return ni120_p[index];
		default:return 0;
	}
}

uint32_t NegatArray(uint16_t index)
{
	switch(LHAData.thermotype)
	{
		case 1:return k_type_n[index];
		case 2:return e_type_n[index];
		case 3:return j_type_n[index];
		case 4:return t_type_n[index];
		case 5:return n_type_n[index];
		case 6:return s_type_n[index];
		case 7:return r_type_n[index];
//      case 8:
//      case 9:
		case 10:return pt100_n[index];
		case 11:return ni120_n[index];
		default:return 0;
	}
}

uint32_t NtcArray(uint16_t index)
{
	switch(ADCData.ntcPN)
	{
		case 0:return ntc_type_p[index];
		case 1:return ntc_type_n[index];
		default:return 0;
	}
}
/************************************************************
*function		: CheckTableP  CheckTableN
*description	: 对传感器测量到的电压进行查表得到温度
*On Entry	: none
*On Exit		: 温度
************************************************************/
uint16_t CheckTableP(uint16_t CheckNum)
{
    uint8_t order = 0;
    uint16_t mid, low = 0, high = arrPlenth[LHAData.thermotype] - 1;
    
    if(PostArray(low) > PostArray(high)) order = 1;     //order = 0 递增      order = 1 递减
    if(!order)                                          //出界判断
    {
        if(CheckNum > PostArray(high)) return 9999;
    }
    else
    {
        if(CheckNum < PostArray(high)) return 9999;
    }
    
    while(low<=high)
    {
        mid = (low + high)/2;
        
        if(PostArray(mid) == CheckNum) break;
        if(!order)              //递增
        {
            if(PostArray(mid) > CheckNum) high = mid - 1;
            else low = mid + 1;
        }
        else                    //递减
        {
            if(PostArray(mid) < CheckNum) high = mid - 1;
            else low = mid + 1;
        }
    }
    
    if(!order)
    {
        if(CheckNum < PostArray(mid)) mid--;
    }
    else
    {
        if(CheckNum > PostArray(mid)) mid--;
    }
    //平滑处理
    LHAData.TempPoint = CalcPoint(PostArray(mid),PostArray(mid+1),CheckNum);
    
    if(LHAData.thermotype == 8) mid += 50;              //B型热电偶特殊处理
    
    return 10*mid + LHAData.TempPoint;
}

uint16_t CheckTableN(uint16_t CheckNum)
{
    uint8_t order = 0;
    uint16_t mid, low = 0, high = arrNlenth[LHAData.thermotype] - 1;
    
    if(LHAData.thermotype == 8) return 9999;            //B型热电偶特殊处理
    
    if(NegatArray(low) > NegatArray(high)) order = 1;     //order = 0 递增      order = 1 递减
    if(!order)                                          //出界判断
    {
        if(CheckNum > NegatArray(high)) return 9999;
    }
    else
    {
        if(CheckNum < NegatArray(high)) return 9999;
    }
    
    while(low<=high)
    {
        mid = (low + high)/2;
        
        if(NegatArray(mid) == CheckNum) break;
        if(!order)              //递增
        {
            if(NegatArray(mid) > CheckNum) high = mid - 1;
            else low = mid + 1;
        }
        else                    //递减
        {
            if(NegatArray(mid) < CheckNum) high = mid - 1;
            else low = mid + 1;
        }
    }
    
    if(!order)
    {
        if(CheckNum < NegatArray(mid)) mid--;
    }
    else
    {
        if(CheckNum > NegatArray(mid)) mid--;
    }
    //平滑处理
    LHAData.TempPoint = CalcPoint(NegatArray(mid),NegatArray(mid+1),CheckNum);
    
    return 10*mid + LHAData.TempPoint;
}

int16_t CheckTableNTC(uint32_t CheckRes)
{
    uint16_t low = 0,high = 0,mid = 0;
    
    if(CheckRes > 13723)ADCData.ntcPN = 1;
    else ADCData.ntcPN = 0;
    
    if(!ADCData.ntcPN) high = 125;
    else high = 40;
    
    if(!ADCData.ntcPN)                  //出界判断
    {
        if(CheckRes < NtcArray(high)) return 9999;
    }
    else
    {
        if(CheckRes > NtcArray(high)) return -9999;
    }
    
    while(low<=high)
    {
        mid = (low + high)/2;
        if(NtcArray(mid) == CheckRes)break;
        if(!ADCData.ntcPN)      //正表
        {
            if(NtcArray(mid) > CheckRes) low = mid + 1;
            else high = mid - 1;
        }
        else                    //负表
        {
            if(NtcArray(mid) < CheckRes) low = mid + 1;
            else high = mid - 1;
        }
    }
    
    if(ADCData.ntcPN == 0 && CheckRes > NtcArray(mid)) mid--;            //正表
    else if(ADCData.ntcPN == 1 && CheckRes < NtcArray(mid)) mid--;       //负表
    
    if(!ADCData.ntcPN) return mid;
    else return -1*mid;
}
/************************************************************
*function		: LHA_Config_Set
*description	: 切换热电偶、热电阻 进行不同的配置 （放在菜单中更改参数之后）
*On Entry	: none
*On Exit		: none
************************************************************/
void LHA_Config_Set(void)
{
    LHAData.thermotype  = gParcode.Item.SensorType;
    LHAData.CascadeSwon = gParcode.Item.CascadeSwon;
    
	if(LHAData.thermotype < 9)
    {
        Sensor_Mode_Select(TC_MODE);    //选择热电偶模式
        
#if(TempCtrlGroup1)
        LHAData.WRITE0 = TC1V2;
        LHAData.WRITE1 = TC1;
        LHAData.WRITE2 = LHACOLD;
#else //if(TempCtrlGroup2||TempCtrlGroup3)
        LHAData.WRITE0 = TC1V2;
        LHAData.WRITE1 = TC1;
        if(LHAData.CascadeSwon == 2) LHAData.WRITE2 = TC2;
#endif

        LHAData.TC1MesTemp = 0;
        LHAData.TC2MesTemp = 0;
        LHAData.TCInit = 0;
        LHAData.GetVreg2Flag = 0;
        LHAData.LHA_Step = 0;
	}
    else
    {
        Sensor_Mode_Select(RTD_MODE);    //选择热电阻模式
        
#if(TempCtrlGroup1)
        LHAData.WRITE1 = RTD1P;
        LHAData.WRITE2 = RTD1N;
        LHAData.WRITE3 = LHACOLD;
#else //if(TempCtrlGroup2||TempCtrlGroup3)
        LHAData.WRITE1 = RTD1P;
        LHAData.WRITE2 = RTD1N;
        if(LHAData.CascadeSwon == 2)
        {
            LHAData.WRITE3 = RTD2P;
            LHAData.WRITE4 = RTD2N;
        }
#endif
        
        LHAData.RTD1MesTemp = 0;
        LHAData.RTD2MesTemp = 0;
        LHAData.RTDInit = 0;
        LHAData.LHA_Step = 2;
    }
    
    LHA_WR_Data(LHAData.WRITE0);       //初始化默认值，防止刚上电时 LHA_MISO_I 不置低电平
    LHA_WR_Data(0);
}
/************************************************************
*function		: CalcTempValue
*description	: 使用查表法查询传感器温度（放大十倍）
*On Entry	: none
*On Exit		: none
************************************************************/
void CalcTempValue(void)
{
    if(LHAData.thermotype > 9)        //热电阻
    {
        if(LHAData.RTDInit)
        {
            RTD_Panel_Calc();
            
            LHAData.RTD1MesTemp = (LHAData.RTD1Value<100) ? -1*CheckTableN(100*LHAData.RTD1Value) : \
                                                               CheckTableP(100*LHAData.RTD1Value);
            
            if(LHAData.CascadeSwon == 2)
            LHAData.RTD2MesTemp = (LHAData.RTD2Value<100) ? -1*CheckTableN(100*LHAData.RTD2Value) : \
                                                               CheckTableP(100*LHAData.RTD2Value);
        }
    }
    else
    {
        if(LHAData.TCInit)
        {
            TC_Panel_Calc();
            
            LHAData.TC1MesTemp = (LHAData.Vreg1<0) ? -1*CheckTableN(-LHAData.Vreg1)+10*ADCData.ColdMesTemp : \
                                                        CheckTableP( LHAData.Vreg1)+10*ADCData.ColdMesTemp;
            
            if(LHAData.CascadeSwon == 2)
            LHAData.TC2MesTemp = (LHAData.Vreg2<0) ? -1*CheckTableN(-LHAData.Vreg2)+10*ADCData.ColdMesTemp : \
                                                        CheckTableP( LHAData.Vreg2)+10*ADCData.ColdMesTemp;
        }
    }
}
/************************************************************
*function		: T1TempValue   T2TempValue
*description	: 返回T1 T2的温度值（放大十倍）
*On Entry	: none
*On Exit		: none
************************************************************/
int16_t T1TempValue(void)
{
    if(LHAData.thermotype > 9)
        return LHAData.RTD1MesTemp;
    else
        return LHAData.TC1MesTemp;
}
int16_t T2TempValue(void)
{
    if(LHAData.thermotype > 9)
        return LHAData.RTD2MesTemp;
    else
        return LHAData.TC2MesTemp;
}
/************************************************************
*function		: TC_Panel_Calc  RTD_Panel_Calc
*description	: 获取热电偶测量电压    计算热电阻阻值
*On Entry	: none
*On Exit		: none
************************************************************/
void TC_Panel_Calc(void)
{
    LHAData.Vreg1 = LHAData.Data1 * 1000 * LHA_FSR_64;             //Vreg放大1000倍
    if(LHAData.CascadeSwon == 2) LHAData.Vreg2 = LHAData.Data2 * 1000 * LHA_FSR_64;
}
void RTD_Panel_Calc(void)
{
    LHAData.Vreg1 = LHAData.Data1 * 1000 * LHA_FSR_64;             //Vreg放大1000倍
    LHAData.Vreg2 = LHAData.Data2 * 1000 * LHA_FSR_64;
    
    if(LHAData.CascadeSwon == 2)
    {
        LHAData.Vreg3 = LHAData.Data3 * 1000 * LHA_FSR_64;
        LHAData.Vreg4 = LHAData.Data4 * 1000 * LHA_FSR_64;
    }
    
    if(LHAData.Vreg1 < 60000)
        LHAData.RTD1Value = (LHAData.Vreg1 - 2*LHAData.Vreg2)*24 / (3318 - LHAData.Data1*LHA_FSR_64);
    
    if(LHAData.CascadeSwon == 2)
    {
        if(LHAData.Vreg3 < 60000)
            LHAData.RTD2Value = (LHAData.Vreg3 - 2*LHAData.Vreg4)*24 / (3318 - LHAData.Data3*LHA_FSR_64);
    }
}
/************************************************************
*function		: ADCPanelUpdate
*description	: LHA数据读取  MCU数据读取
*On Entry	: none
*On Exit		: none
************************************************************/
void ADCPanelUpdate()
{
    LHA7918_Read();
    
#if(TempCtrlGroup1)
    
    if((LHAData.thermotype<9 && LHAData.TCInit) ||(LHAData.thermotype>9 && LHAData.RTDInit))
    
        MCUADC_Read();
    
#endif
}
/************************************************************
*function		: MCUADC_Read
*description	: MCU ADC读取
*On Entry	: none
*On Exit		: none
************************************************************/
void MCUADC_Read(void)
{
#if(TempCtrlGroup1)
    
    ADCData.TDataSum    += ADC_GetResult(16);
    
#else //if(TempCtrlGroup2||TempCtrlGroup3)
    
    ADCData.RefDataSum  += ADC_GetResult(8);
    ADCData.TDataSum    += ADC_GetResult(16);
    ADCData.ColdDataSum += ADC_GetResult(17);
    
#endif
    
    ADCData.LB_Count ++;
    
    if(ADCData.LB_Time_Tick >= 2)
    {
        MCUADC_LB();                    //更新ADC的滤波值
        ADCValueProcess();              //计算冷端温度和NTC温度
        ADCData.LB_Count = 0;
        ADCData.LB_Time_Tick = 0;
    }
}
/************************************************************
*function		: MCUADC_LB
*description	: MCU ADC原始数据滤波 
*On Entry	: none
*On Exit		: none
************************************************************/
void MCUADC_LB(void)
{
    ADCData.RefDataLB  = ADCData.RefDataSum /ADCData.LB_Count;
    ADCData.TDataLB    = ADCData.TDataSum   /ADCData.LB_Count;
    ADCData.ColdDataLB = ADCData.ColdDataSum/ADCData.LB_Count;
    
    ADCData.RefDataSum  = 0;
    ADCData.TDataSum    = 0;
    ADCData.ColdDataSum = 0;
}
/************************************************************
*function		: ADCValueProcess
*description	: ADC数据处理 计算冷端温度和NTC温度
*On Entry	: none
*On Exit		: none
************************************************************/
void ADCValueProcess(void)
{
    ADCData.ColdMesTemp = Cold_Vol_Calc();                         //四舍五入到小数点后一位
    
    GainErrorCorrect(ADCData.ColdMesTemp);                         //增益误差矫正
	
    //公式 R_ntc = (5000*ADC_max)/ADC_out - 5000
    ADCData.Rntc = 5000 * 4095 / ADCData.TDataLB - 5000;
    
	//查表得到NTC温度
	ADCData.NTCMesTemp = CheckTableNTC(ADCData.Rntc);
}
/************************************************************
*function		: Cold_Vol_Calc
*description	: 计算冷端电压 四舍五入到小数点后一位
*On Entry	: none
*On Exit		: none
************************************************************/
float Cold_Vol_Calc(void)
{
    float DataTemp;
    
#if(TempCtrlGroup1)
    
    //if(!(ADCData.StandColdData&0x8000))                     //最高1位代表冷端是否标定过
    ADCData.StandColdVol = 563;                         //否则使用默认的输出电压值
    
#elif(TempCtrlGroup2)
    
    //if(!(ADCData.StandColdData&0x8000))                     //最高1位代表冷端是否标定过
    ADCData.StandColdVol = 4136;                        //否则使用默认的输出电压值
    
#else //if(TempCtrlGroup3)
    
    //if(!(ADCData.StandColdData&0x8000))                     //最高1位代表冷端是否标定过
    ADCData.StandColdVol = 2482;                        //否则使用默认的输出电压值
    
#endif
    
    //冷端电压 = 2500*ColdData/RefData
    //if(!(ADCData.StandColdData&0x8000))                     //最高1位代表冷端是否标定过
    //ADCData.StandColdVol = 2500 * (ADCData.StandColdData&0x0FFF) / ADCData.RefDataLB;          //有标定
    
#if(TempCtrlGroup1)

    if(LHAData.thermotype<9) ADCData.CurrentColdVol = LHAData.Data2 * LHA_FSR_1024;
    else                     ADCData.CurrentColdVol = LHAData.Data3 * LHA_FSR_1024;
    
#else //if(TempCtrlGroup2||TempCtrlGroup3)

    ADCData.CurrentColdVol = 2500 * ADCData.ColdDataLB / ADCData.RefDataLB;
    
#endif
    
    DataTemp = (ADCData.CurrentColdVol - ADCData.StandColdVol) / T_V_SENS + 25;
    
    return (int16_t)(10*DataTemp + 0.5f)/10.0f;
}
/************************************************************
*function		: GainErrorCorrect
*description	: 增益误差矫正
*On Entry	: none
*On Exit		: none
************************************************************/
void GainErrorCorrect(float temperture)
{
    if(temperture < -40)temperture = -40;
    else if(temperture >= 60)temperture = 60;
    
    if(temperture >= -40 && temperture <= 20)
    {
        GainErrorData.Kgain = -0.00427;
        GainErrorData.Bgain = -0.13335;
    }
    else if(temperture >= 20 && temperture <= 60)
    {
        GainErrorData.Kgain = -0.00242;
        GainErrorData.Bgain = -0.17043;
    }
    
    GainErrorData.Ygain = 0.01*(GainErrorData.Kgain * temperture + GainErrorData.Bgain);
    
}
/************************************************************
*function		: ZeroOffsetCorrect
*description	: 零点漂移校准
*On Entry	: none
*On Exit		: none
************************************************************/
void ZeroOffsetCorrect(void)
{
    
    
}
/************************************************************
*function		: TempErrorWarn
*description	: 温度异常报警
*On Entry	: none
*On Exit		: none
************************************************************/
void TempErrorWarn(void)
{
    if((LHAData.thermotype < 9 && LHAData.Vreg1 > 63970)|| \
       (LHAData.thermotype > 9 && LHAData.Vreg1 > 63970))
        SetErrorCode(6);    //ERROR 5
    
    if((LHAData.thermotype < 9 && LHAData.GetTC1Vreg2 < 0x7FEF && LHAData.GetVreg2Flag >= 1)|| \
       (LHAData.thermotype > 9 && LHAData.Vreg1 <= 2*LHAData.Vreg2))
        SetErrorCode(12);   //ERROR 11
    
    if(ADCData.NTCMesTemp >= 90)
        SetErrorCode(15);   //ERROR 14
    
    
    
    ///---------------T2_ERROR----------------///
#if (TempCtrlType!=TempCtrlWF48&&TempCtrlType!=TempCtrlWF48C&&\
     TempCtrlType!=TempCtrlWF72&&TempCtrlType!=TempCtrlWF72R)
    
    if(LHAData.CascadeSwon == 2)
    {
//        if((LHAData.thermotype < 9 && LHAData.Data2 > 0x7FF0 && LHAData.Data2PN == 0)|| \
//           (LHAData.thermotype > 9 && LHAData.Data3 > 0x7FF0 && LHAData.Data3PN == 0))
//            SetErrorCode(6);    //ERROR 5
        
        if((LHAData.thermotype < 9 && LHAData.GetTC2Vreg2 < 0x7FEF && LHAData.GetVreg2Flag == 2)|| \
           (LHAData.thermotype > 9 && LHAData.Data3 <= 2*LHAData.Data4))
            SetErrorCode(12);   //ERROR 11
        
    }
#endif
    
}
/************************************************************
*function		: CalcT1ShowTemp
*description	: 显示0.0和0.5两种状态的温度
*On Entry	: none
*On Exit		: none
************************************************************/
void CalcT1ShowTemp(void)
{
//         if(tCtrlOutParm.GetTempT1Val%10 < 3) LHAData.T1ShowTemp = 10*(tCtrlOutParm.GetTempT1Val/10);
//    else if(tCtrlOutParm.GetTempT1Val%10 > 7) LHAData.T1ShowTemp = 10*(tCtrlOutParm.GetTempT1Val/10) + 10;
//    else                                      LHAData.T1ShowTemp = 10*(tCtrlOutParm.GetTempT1Val/10) +  5;
//    
//    if(LHAData.T1ShowTemp > 9995 || LHAData.T1ShowTemp <= -1000)
//    {
//        LHAData.T1ShowTemp /= 10;
//        LHAData.T1ShowTempBit = 0;
//    }
//    else
//    {
//        LHAData.T1ShowTempBit = 1;
//    }
    
    static uint8_t Tmr = 1;
    
    if((LHAData.thermotype > 9 && LHAData.RTDInit == 1) || (LHAData.thermotype < 9 && LHAData.TCInit == 1))
    {
        if(Tmr == 1 || Tmr == 6 || Tmr ==8)
        {
            LHAData.T1ShowTemp = MAF_AddData(tCtrlOutParm.GetTempT1Val);
            
            if(LHAData.T1ShowTemp > 9995 || LHAData.T1ShowTemp <= -1000)
            {
                LHAData.T1ShowTemp /= 10;
                LHAData.T1ShowTempBit = 0;
            }
            else
            {
                LHAData.T1ShowTempBit = 1;
            }
        }
        
        if(Tmr >= 16)Tmr = 1;
        else Tmr++;
    }
}
uint16_t MAF_AddData(uint16_t new_value)
{
    static uint8_t count = 0;
    static uint8_t index = 0;
    static uint32_t sum = 0;
    static uint16_t out_value = 0;
    static uint16_t buffer[WINDOW_SIZE] = {};
    
    //初始阶段，未填满 WINDOW_SIZE
    if(count < WINDOW_SIZE)
    {
        sum += new_value;
        buffer[index] = new_value;
        index = (index + 1)%(WINDOW_SIZE);
        count++;
        out_value = (sum + count/2)/count;
        
        return out_value;
    }
    //稳定阶段
    sum -= buffer[index];               //移除旧数据
    sum += new_value;                   //添加新数据
    buffer[index] = new_value;          //更新缓冲区
    index = (index + 1)%WINDOW_SIZE;
    out_value = (sum + WINDOW_SIZE/2)/WINDOW_SIZE;
    
    return out_value;
}





