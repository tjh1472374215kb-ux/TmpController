/************************************************************
*function		: Set Parameter value
************************************************************/
void SetControlMode(void)
{
	
}

void SetSensorType(void)
{
	if(gParcode.Item.SensorType==9)
	{
		stMenu.uSetError = 1;
	}
	else
	{
		int16_t tmpMin,tmpMax,tmpData;

		tmpMin = RxTempRange[gParcode.Item.SensorType-1][0]*10;
		tmpMax = RxTempRange[gParcode.Item.SensorType-1][1]*10;

		ParaCodeTAB[emTmpUpLimit][1]=tmpMin;
		ParaCodeTAB[emTmpUpLimit][2]=tmpMax;
		ParaCodeTAB[emTmpDnLimit][1]=tmpMin;
		ParaCodeTAB[emTmpDnLimit][2]=tmpMax;
		
		if(gParcode.Item.TmpUpLimit>tmpMax)
			gParcode.Item.TmpUpLimit=tmpMax;
		else if(gParcode.Item.TmpUpLimit<tmpMin)
			gParcode.Item.TmpUpLimit=tmpMin;

		if(gParcode.Item.TmpDnLimit>tmpMax)
			gParcode.Item.TmpDnLimit=tmpMax;
		else if(gParcode.Item.TmpDnLimit<tmpMin)
			gParcode.Item.TmpDnLimit=-tmpMin;

		if(gParcode.Item.InnerTemp<gParcode.Item.TmpDnLimit)
			gParcode.Item.InnerTemp=gParcode.Item.TmpDnLimit;
		else if(gParcode.Item.InnerTemp>gParcode.Item.TmpUpLimit)
			gParcode.Item.InnerTemp=gParcode.Item.TmpUpLimit;

		if(gParcode.Item.TemperatxUnit==1)
		{
			if(gParcode.Item.TempSetVal<gParcode.Item.TmpDnLimit*10)
				gParcode.Item.TempSetVal=gParcode.Item.TmpDnLimit*10;
			else if(gParcode.Item.TempSetVal>gParcode.Item.TmpUpLimit*10)
				gParcode.Item.TempSetVal=gParcode.Item.TmpUpLimit*10;

			tmpData = gParcode.Item.TempSetVal;
		}
		else
		{

			tmpMax = gParcode.Item.TmpUpLimit*18+320;
			tmpMin = gParcode.Item.TmpDnLimit*18+320;

			if(gParcode.Item.TempSetVal<tmpMin)
				gParcode.Item.TempSetVal=tmpMin;
			else if(gParcode.Item.TempSetVal>tmpMax)
				gParcode.Item.TempSetVal=tmpMax;

			tmpData = (gParcode.Item.TempSetVal-320)*5/9;
		}

		LHA_Config_Set();
		
		if(GetInnerTempFlag()>0)
			SetOutpTmpToRef(gParcode.Item.InnerTemp);
		else
			SetOutpTmpToRef(tmpData);
	}
}

void SetTmpSetLock(void)
{
	if(gParcode.Item.TmpSetLock==1)
		SetLockFlag(1);
	else
		SetLockFlag(0);
}

void SetCtrlMethod(void)
{
	if(gParcode.Item.CtrlMethod==1)
		SetOutputSwitch(1);
	else
		SetOutputSwitch(0);
}

void SetXRunSwitch(void)
{
	#if(TempCtrlType==TempCtrlWF96||TempCtrlType==TempCtrlWF96R||\
			TempCtrlType==TempCtrlWF49||TempCtrlType==TempCtrlWF49R)
	if(gParcode.Item.XRunSwitch==2)
	{
		;
	}
	#else
	if(gParcode.Item.XRunSwitch==2)
		stMenu.uSetError = 1;
	#endif
}

void SetTmpUpLimit(void)
{
	int16_t tmpMin,tmpMax,tmpData;
	
	if(gParcode.Item.InnerTemp<gParcode.Item.TmpDnLimit)
		gParcode.Item.InnerTemp=gParcode.Item.TmpDnLimit;
	else if(gParcode.Item.InnerTemp>gParcode.Item.TmpUpLimit)
		gParcode.Item.InnerTemp=gParcode.Item.TmpUpLimit;

	if(gParcode.Item.TemperatxUnit==1)
	{
		if(gParcode.Item.TempSetVal<gParcode.Item.TmpDnLimit*10)
			gParcode.Item.TempSetVal=gParcode.Item.TmpDnLimit*10;
		else if(gParcode.Item.TempSetVal>gParcode.Item.TmpUpLimit*10)
			gParcode.Item.TempSetVal=gParcode.Item.TmpUpLimit*10;

		tmpData = gParcode.Item.TempSetVal;
	}
	else
	{

		tmpMax = gParcode.Item.TmpUpLimit*18+320;
		tmpMin = gParcode.Item.TmpDnLimit*18+320;

		if(gParcode.Item.TempSetVal<tmpMin)
			gParcode.Item.TempSetVal=tmpMin;
		else if(gParcode.Item.TempSetVal>tmpMax)
			gParcode.Item.TempSetVal=tmpMax;

		tmpData = (gParcode.Item.TempSetVal-320)*5/9;
	}

	if(GetInnerTempFlag()>0)
		SetOutpTmpToRef(gParcode.Item.InnerTemp);
	else
		SetOutpTmpToRef(tmpData);
}

void SetTmpDnLimit(void)
{
	int16_t tmpMin,tmpMax,tmpData;
	
	if(gParcode.Item.InnerTemp<gParcode.Item.TmpDnLimit)
		gParcode.Item.InnerTemp=gParcode.Item.TmpDnLimit;
	else if(gParcode.Item.InnerTemp>gParcode.Item.TmpUpLimit)
		gParcode.Item.InnerTemp=gParcode.Item.TmpUpLimit;

	if(gParcode.Item.TemperatxUnit==1)
	{
		if(gParcode.Item.TempSetVal<gParcode.Item.TmpDnLimit*10)
			gParcode.Item.TempSetVal=gParcode.Item.TmpDnLimit*10;
		else if(gParcode.Item.TempSetVal>gParcode.Item.TmpUpLimit*10)
			gParcode.Item.TempSetVal=gParcode.Item.TmpUpLimit*10;

		tmpData = gParcode.Item.TempSetVal;
	}
	else
	{

		tmpMax = gParcode.Item.TmpUpLimit*18+320;
		tmpMin = gParcode.Item.TmpDnLimit*18+320;

		if(gParcode.Item.TempSetVal<tmpMin)
			gParcode.Item.TempSetVal=tmpMin;
		else if(gParcode.Item.TempSetVal>tmpMax)
			gParcode.Item.TempSetVal=tmpMax;

		tmpData = (gParcode.Item.TempSetVal-320)*5/9;
	}

	if(GetInnerTempFlag()>0)
		SetOutpTmpToRef(gParcode.Item.InnerTemp);
	else
		SetOutpTmpToRef(tmpData);
}

void SetInnerTemp(void)
{
	if(GetInnerTempFlag()>0)
		SetOutpTmpToRef(gParcode.Item.InnerTemp*10);
}

void SetUpLmtWarn(void)
{
	
}

void SetDnLmtWarn(void)
{
	
}

void SetUpErrWarn(void)
{
	SetUpDnErrWarnQv();
}

void SetDnErrWarn(void)
{
	SetUpDnErrWarnQv();
}

void SetWarnReset(void)
{
	
}

void SetWarnSwitch(void)
{
	
}

void SetCtrlStyle(void)
{
	#if(TempCtrlType==TempCtrlWF48||TempCtrlType==TempCtrlWF48C)
	if(gParcode.Item.CtrlStyle==3)
		stMenu.uSetError = 1;
	#endif
}

void SetHeatCoolErr(void)
{
	SetHeatCoolErrQv();
}

//-------------------------------------------
void SetHeatOutput(void)
{
	#if(TempCtrlType==TempCtrlWF48)
	if(gParcode.Item.HeatOutput==3)
	{
		stMenu.uSetError = 1;
		return;
	}
	#elif (TempCtrlType==TempCtrlWF48C)
	if(gParcode.Item.HeatOutput<3)
		stMenu.uSetError = 1;
	#endif

	if(gParcode.Item.HeatOutput==3)
	{
		if(gParcode.Item.HeatPeriod<20)
			gParcode.Item.HeatPeriod=20;
	}
}

void SetHeatPeriod(void)
{
	InitTempRamp();
}

void SetHeatAccRate(void)
{
	InitTempRamp();
}

void SetHeatDecRate(void)
{
	InitTempRamp();
}

void SetHeatAnlgMax(void)
{
	
}

void SetHeatAnlgMin(void)
{
	
}

void SetHeatAnlgErr(void)
{
	
}

//-------------------------------------------
void SetCoolOutput(void)
{
	#if(TempCtrlType==TempCtrlWF48)
	if(gParcode.Item.CoolOutput==3)
	{
		stMenu.uSetError = 1;
		return;
	}
	#elif (TempCtrlType==TempCtrlWF48C)
	if(gParcode.Item.CoolOutput<3)
		stMenu.uSetError = 1;
	#endif

	if(gParcode.Item.CoolOutput==3)
	{
		if(gParcode.Item.CoolPeriod<20)
			gParcode.Item.CoolPeriod=20;
	}
}

void SetCoolPeriod(void)
{
	InitTempRamp();
}

void SetCoolAccRate(void)
{
	InitTempRamp();
}

void SetCoolDecRate(void)
{
	InitTempRamp();
}

void SetCoolAnlgMax(void)
{
	
}

void SetCoolAnlgMin(void)
{
	
}

void SetCoolAnlgErr(void)
{
	
}

//-----------------------------------------
void SetCascadeSwon(void)
{
	#if(TempCtrlType==TempCtrlWF96||TempCtrlType==TempCtrlWF96R||\
			TempCtrlType==TempCtrlWF49||TempCtrlType==TempCtrlWF49R)
	if(gParcode.Item.CascadeSwon==2)
	{
		;
	}
	#else
	if(gParcode.Item.CascadeSwon==2)
		stMenu.uSetError = 1;
	#endif

	LHA_Config_Set();
}

void SetCscdT2Limit(void)
{
	
}

void SetErrCorrectedV(void)
{
	
}

void SetTemperatxUnit(void)
{
	if(gParcode.Item.TemperatxUnit==2)
	{
		if(GetTempFlag()==0)
		{
			int16_t tmpdata;
			
			tmpdata = (long)gParcode.Item.TempSetVal*9/5 + 320;

			gParcode.Item.TempSetVal = (tmpdata/5)*5;
		}
		SetTempFlag(1);
	}
	else
	{
		if(GetTempFlag()==1)
		{
			int16_t tmpdata;
			
			tmpdata = (gParcode.Item.TempSetVal-320)*5/9;

			gParcode.Item.TempSetVal = (tmpdata/5)*5;
		}
		
		SetTempFlag(0);
	}
}
//-----------------------------------------
void SetCommAddr(void)
{
	
}

void SetCommSpeed(void)
{
	SetCommBaudrate();
}

void SetCommFormat(void)
{
	SetCommXXFormat();
}

void SetCommMode(void)
{
	
}

void SetCommTimer(void)
{
	
}
//-----------------------------------------
void SetRstDefault(void)
{
	if(gParcode.Item.uRstDefault==2)
		SetParaDefault();
}

void SetSwVertion(void)
{
	
}

void SetMnVertion(void)
{
	
}
//-----------------------------------------
void SetqPID01Kp(void)
{
	InitPidParm();
}

void SetqPID01Ki(void)
{
	InitPidParm();
}

void SetqPID01Kd(void)
{
	InitPidParm();
}

void SetqPID02Kp(void)
{
	InitPidParm();
}

void SetqPID02Ki(void)
{
	InitPidParm();
}

void SetqPID02Kd(void)
{
	InitPidParm();
}

void SetqPID11Kp(void)
{
	InitPidParm();
}

void SetqPID11Ki(void)
{
	InitPidParm();
}

void SetqPID11Kd(void)
{
	InitPidParm();
}

void SetqPID12Kp(void)
{
	InitPidParm();
}

void SetqPID12Ki(void)
{
	InitPidParm();
}

void SetqPID12Kd(void)
{
	InitPidParm();
}


/************************************************************
description	: Menu Setting functions
Parameters	: none.
default		: none
************************************************************/
void *MenuSettingFunc[]=
{
	(void *)SetControlMode,
	(void *)SetCtrlMethod,
	(void *)SetXRunSwitch,
	(void *)SetTmpSetLock,
	(void *)SetSensorType,
	(void *)SetTmpUpLimit,
	(void *)SetTmpDnLimit,
	(void *)SetInnerTemp,
	(void *)SetUpLmtWarn,
	(void *)SetDnLmtWarn,
	(void *)SetUpErrWarn,
	(void *)SetDnErrWarn,
	(void *)SetWarnReset,
	(void *)SetWarnSwitch,
	(void *)SetCtrlStyle,
	(void *)SetHeatCoolErr,
	
	(void *)SetHeatOutput,
	(void *)SetHeatPeriod,
	(void *)SetHeatAccRate,
	(void *)SetHeatDecRate,
	(void *)SetHeatAnlgMax,
	(void *)SetHeatAnlgMin,
	(void *)SetHeatAnlgErr,
	
	(void *)SetCoolOutput,
	(void *)SetCoolPeriod,
	(void *)SetCoolAccRate,
	(void *)SetCoolDecRate,
	(void *)SetCoolAnlgMax,
	(void *)SetCoolAnlgMin,
	(void *)SetCoolAnlgErr,
	
	(void *)SetCascadeSwon,
	(void *)SetCscdT2Limit,
	(void *)SetErrCorrectedV,
	(void *)SetTemperatxUnit,
	
	(void *)SetCommAddr,
	(void *)SetCommSpeed,
	(void *)SetCommFormat,
	(void *)SetCommMode,
	(void *)SetCommTimer,
	
	(void *)SetRstDefault,
	(void *)SetSwVertion,
	(void *)SetMnVertion,
	
	(void *)SetqPID01Kp,
	(void *)SetqPID01Ki,
	(void *)SetqPID01Kd,
	(void *)SetqPID02Kp,
	(void *)SetqPID02Ki,
	(void *)SetqPID02Kd,
	(void *)SetqPID11Kp,
	(void *)SetqPID11Ki,
	(void *)SetqPID11Kd,
	(void *)SetqPID11Kp,
	(void *)SetqPID11Ki,
	(void *)SetqPID11Kd
};

