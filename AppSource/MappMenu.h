#include "MainDefine.h"

#define LongKey_05		35      //50
#define LongKey_50		175     //250

typedef struct{
	uint8_t u8MenuID;
	uint8_t u8FuncID;
	uint8_t u8ParaID;
	uint8_t u8Wait1S;
	uint8_t u8Timeout;
	uint8_t u8DatDot;
	uint8_t LockFlag;
	uint8_t TempFlag;
	uint8_t u8DatSave;
	uint8_t uSetError;
	uint16_t u16Pswd;
	uint16_t u16Data;
	uint16_t LockNum;
	uint16_t LockTmr;
	uint16_t TempTmr;
	
	uint16_t LockHTmr;
	uint16_t LockLTmr;
	uint16_t TempHTmr;
	uint16_t TempLTmr;
	uint16_t StopHTmr;
	uint16_t StopLTmr;
	
	int16_t u16Para;
	int16_t u16DatMin;
	int16_t u16DatMax;
}MainMenu;

extern MainMenu stMenu;

void MappMainMenu(void);
void MappMenu_Init(void);
void MainPara_Init(void);
void MappMenu100MS(void);
void MenuTime1000MS(void);
void Menu_MainShow(void);
void Menu_MainPswd(void);
void Menu_MainFunc(void);
void Menu_SubFunc(void);

void SetParaDefault(void);
void SaveParaToRom(void);

uint16_t GetParaChkSum(int16_t *strbuf);

uint8_t DoSave1FromComm(uint8_t uitem,uint16_t uPara);
uint8_t DoSavenFromComm(uint8_t u8Item,uint8_t ulen,uint16_t* buf);

