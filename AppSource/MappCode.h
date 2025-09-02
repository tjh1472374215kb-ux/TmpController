#include "MainDefine.h"

#define ParmMaxItem	54
#define ParmMaxBuff	28

//define parameter code struct
typedef union{
	uint32_t u32Buff[28];
	int16_t u16Buff[56];
	struct{
//-----Basic function Setting---16-----//
		int16_t ControlMode;
		int16_t CtrlMethod;
		int16_t XRunSwitch;
		int16_t TmpSetLock;
		int16_t SensorType;
		int16_t TmpUpLimit;
		int16_t TmpDnLimit;
		int16_t InnerTemp;
		int16_t UpLmtWarn;
		int16_t DnLmtWarn;
		int16_t UpErrWarn;
		int16_t DnErrWarn;
		int16_t WarnReset;
		int16_t WarnSwitch;
		int16_t CtrlStyle;
		int16_t HeatCoolErr;
//-----Heat Control Setting---7-----//
		int16_t HeatOutput;
		int16_t HeatPeriod;
		int16_t HeatAccRate;
		int16_t HeatDecRate;
		int16_t HeatAnlgMax;
		int16_t HeatAnlgMin;
		int16_t HeatAnlgErr;
//-----Cool Control Setting---7-----//
		int16_t CoolOutput;
		int16_t CoolPeriod;
		int16_t CoolDecRate;
		int16_t CoolAccRate;
		int16_t CoolAnlgMax;
		int16_t CoolAnlgMin;
		int16_t CoolAnlgErr;
//--Cascade Control Setting---2-----//
		int16_t CascadeSwon;
		int16_t CscdT2Limit;
//--Special Control Setting---2-----//
		int16_t ErrCorrectedV;
		int16_t TemperatxUnit;
//-----RS485 param Setting---5------//
		int16_t uCommAddr;
		int16_t uCommSpeed;
		int16_t uCommFormat;
		int16_t uCommMode;
		int16_t uCommTimer;
//-----System param Setting---3----//
		uint16_t uRstDefault;
		uint16_t uSwVertion;
		uint16_t uMnVertion;
//-----PID---param Setting---12----//
		uint16_t qPID01Kp;
		uint16_t qPID01Ki;
		uint16_t qPID01Kd;
		uint16_t qPID02Kp;
		uint16_t qPID02Ki;
		uint16_t qPID02Kd;
		uint16_t qPID11Kp;
		uint16_t qPID11Ki;
		uint16_t qPID11Kd;
		uint16_t qPID12Kp;
		uint16_t qPID12Ki;
		uint16_t qPID12Kd;
//-----checksum param-------------//
		int16_t  TempSetVal;
		uint16_t uCheckSum0;
		//uint16_t uCheckSum1;
	}Item;
//-------FE---4-----
}ParCode;

typedef enum nPara{
//-----Basic function Setting---16-----//
	emControlMode=0,
	emCtrlMethod=1,
	emXRunSwitch=2,
	emTmpSetLock,
	emSensorType,
	emTmpUpLimit,
	emTmpDnLimit,
	emInnerTemp,
	emUpLmtWarn,
	emDnLmtWarn,
	emUpErrWarn,
	emDnErrWarn,
	emWarnReset,
	emWarnSwitch,
	emCtrlStyle,
	emHeatCoolErr,
//-----Heat Control Setting---7-----//
	emHeatOutput,
	emHeatPeriod,
	emHeatAccRate,
	emHeatDecRate,
	emHeatAnlgMax,
	emHeatAnlgMin,
	emHeatAnlgErr,
//-----Cool Control Setting---7-----//
	emCoolOutput,
	emCoolPeriod,
	emCoolDecRate,
	emCoolAccRate,
	emCoolAnlgMax,
	emCoolAnlgMin,
	emCoolAnlgErr,
//--Cascade Control Setting---2-----//
	emCascadeSwon,
	emCscdT2Limit,
//--Special Control Setting---2-----//
	emErrCorrectedV,
	emTemperatxUnit,
//-----RS485 param Setting---5------//
	emuCommAddr,
	emuCommSpeed,
	emuCommFormat,
	emuCommMode,
	emuCommTimer,
//-----System param Setting---3----//
	emuRstDefault,
	emuSwVertion,
	emuMnVertion,
//-----PID---param Setting---12----//
	emqPID01Kp,
	emqPID01Ki,
	emqPID01Kd,
	emqPID02Kp,
	emqPID02Ki,
	emqPID02Kd,
	emqPID11Kp,
	emqPID11Ki,
	emqPID11Kd,
	emqPID12Kp,
	emqPID12Ki,
	emqPID12Kd,
	//-----checksum param-------------//
	emTempSetVal,
	emuCheckSum0,
	//emuCheckSum1
}EnumCode;

//define Secret code struct
typedef union{
	uint32_t u32Buff[10];
	struct{
		uint16_t tPassWordr[6];
		uint16_t tPassWordt[8];
		uint16_t tPassWordc[6];
	}Item;
}Encryption;

extern ParCode gParcode;

extern Encryption gEncryption;

extern int16_t ParaCodeTAB[][4];

extern const int16_t WarnOutSwTab[16];

extern int16_t GetDotBitno(uint16_t);
extern int16_t GetIfRWFlag(uint16_t);
extern int16_t GetMaxPara(uint16_t);
extern int16_t GetMinPara(uint16_t);
extern void SetCodeDefault(void);
extern void SetCodeInitial(void);
extern uint16_t GetMenuIndex(uint16_t);

extern void ReadCodeAtRom0(uint32_t *u32Buff);
extern void SaveCodeAtRom0(uint32_t *u32Buff);
extern void ReadCodeAtRom1(uint32_t *u32Buff);
extern void SaveCodeAtRom1(uint32_t *u32Buff);

extern void ReadCHIPID0Code(void);
extern void ReadCHIPID1Code(void);
extern void SaveCHIPID1Code(void);
extern void GetEncryptionCode(void);
extern uint16_t CheckPassWord(void);

