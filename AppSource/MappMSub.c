
/************************************************************
*function		: GetParaChkSum
*description	: Count the checksum...
************************************************************/
uint16_t GetParaChkSum(int16_t *strbuf)
{
	uint16_t u8cnt,uChkSum;
	
	uChkSum = 0x1234;
	
	for(u8cnt=0;u8cnt<emuRstDefault;u8cnt++)
	{
		uChkSum^=strbuf[u8cnt];

		if(uChkSum==0)
			uChkSum=0x1234;
		else
			uChkSum <<= 1;
	}

	return uChkSum;
}
/************************************************************
*function		: SetParameterCode
*description	: Save the parameter to rom...
************************************************************/
void SetParameterCode(uint16_t uitem)
{
	if(GetIfRWFlag(uitem)&0x10) 
	{
		stMenu.uSetError = 1;
		return;
	}

	if(stMenu.u16Para!=gParcode.u16Buff[uitem])
	{
		int16_t tmpParm;
		
		stMenu.u8DatSave = 1;

		tmpParm = gParcode.u16Buff[uitem];

		gParcode.u16Buff[uitem] = stMenu.u16Para;
		
		((void(*)(void))(MenuSettingFunc[uitem]))();

		if(stMenu.uSetError>0)
			gParcode.u16Buff[uitem] = tmpParm;
			
		gParcode.Item.uCheckSum0=GetParaChkSum(gParcode.u16Buff);
	}
}
/************************************************************
*function		: DoSave1Para
*description	: save one parameter from other comm.
************************************************************/
uint8_t DoSave1FromComm(uint8_t uitem,uint16_t uPara)
{
	stMenu.uSetError = 0;
	stMenu.u16Para = uPara;

	if(GetIfRWFlag(uitem)&0x20)
	{
		stMenu.uSetError = 1;
		return 1;
	}

	if(stMenu.u16Para<ParaCodeTAB[uitem][1]||
		stMenu.u16Para>ParaCodeTAB[uitem][2])
	{
		stMenu.uSetError = 3;
		return 3;
	}
		
	if(stMenu.u16Para!=gParcode.u16Buff[uitem])
	{
		int16_t tmpParm;
		
		stMenu.u8DatSave = 1;

		tmpParm = gParcode.u16Buff[uitem];

		gParcode.u16Buff[uitem] = stMenu.u16Para;
		
		((void(*)(void))(MenuSettingFunc[uitem]))();

		if(stMenu.uSetError>0)
			gParcode.u16Buff[uitem] = tmpParm;
			
		gParcode.Item.uCheckSum0=GetParaChkSum(gParcode.u16Buff);
	}
	
	return stMenu.uSetError;
}
/************************************************************
*function		: DoSavenParas
*description	: save n parameters from other comm.
************************************************************/
uint8_t DoSavenFromComm(uint8_t u8Item,uint8_t ulen,uint16_t* buf)
{
	uint8_t u8i,u8j;
	
	stMenu.uSetError = 0;
	
	for(u8i=0,u8j=u8Item;u8i<ulen;u8i++,u8j++){
		stMenu.u16Para = buf[u8i];

		if(GetIfRWFlag(u8j)&0x20)
		{
			stMenu.uSetError = 1;
			return 1;
		} 
			
		if(stMenu.u16Para<ParaCodeTAB[u8j][1]||
			stMenu.u16Para>ParaCodeTAB[u8j][2]){
			stMenu.uSetError = 3;
			return 3;
		}

		if(stMenu.u16Para!=gParcode.u16Buff[u8j])
		{
			int16_t tmpParm;
			
			stMenu.u8DatSave = 1;

			tmpParm = gParcode.u16Buff[u8j];

			gParcode.u16Buff[u8j] = stMenu.u16Para;
			
			((void(*)(void))(MenuSettingFunc[u8j]))();

			if(stMenu.uSetError>0)
			{
				gParcode.u16Buff[u8j] = tmpParm;

				gParcode.Item.uCheckSum0=GetParaChkSum(gParcode.u16Buff);
				
				return 3;
			}
		}
	}

	gParcode.Item.uCheckSum0=GetParaChkSum(gParcode.u16Buff);

	return stMenu.uSetError;
}

/************************************************************
*function:SetParaDefault
*description: 
************************************************************/
void SetParaDefault(void)
{
	SetCodeDefault();
	
	gParcode.Item.uCheckSum0=GetParaChkSum(gParcode.u16Buff);

	gParcode.Item.TempSetVal = 1000;
	
	SetCodeInitial();
}
/************************************************************
*function		: MainPara_Init
*description	: set parameter code to initial value.
*On Entry	: none.
*On Exit		: noe
************************************************************/
void SetCodeInitial(void)
{
	int16_t tmpMin,tmpMax;

	tmpMin = RxTempRange[gParcode.Item.SensorType-1][0];
	tmpMax = RxTempRange[gParcode.Item.SensorType-1][1];

	ParaCodeTAB[emTmpUpLimit][1]=tmpMin;
	ParaCodeTAB[emTmpUpLimit][2]=tmpMax;
	ParaCodeTAB[emTmpDnLimit][1]=tmpMin;
	ParaCodeTAB[emTmpDnLimit][2]=tmpMax;
	
	SetUpDnErrWarnQv();
	SetHeatCoolErrQv();
	
	InitTempRamp();

	SetCommBaudrate();

	SetCommXXFormat();

	if(gParcode.Item.TmpSetLock==1)
		SetLockFlag(1);
	else
		SetLockFlag(0);
	
	if(gParcode.Item.TemperatxUnit==2)
		SetTempFlag(1);
	else
		SetTempFlag(0);

	SetCommFlag(0);

	SetOutpTmpToRef(gParcode.Item.TempSetVal);

	//Auto to run...
	if(gParcode.Item.XRunSwitch==1&&gParcode.Item.CtrlMethod==1)
	{
		SetOutputSwitch(1);
	}
	else
	{
		SetOutputSwitch(0);
	}
}

/************************************************************
*function:SaveParaToRom
*description: 
************************************************************/
void SaveParaToRom(void)
{
	if(stMenu.u8DatSave>0)
	{
		stMenu.u8DatSave=0;

		GPIOResetOutput();
		
		SaveCodeAtRom0(gParcode.u32Buff);
	}
}

