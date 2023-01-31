#define _BUILD_SYMBOLS
#include "AMC4030_controller.h"

CALLBACK_DEF	COM_API_SetComType(int nType){return 0;}
CALLBACK_DEF	COM_API_OpenLink(int nID, int nBound){return 0;}

CALLBACK_DEF	COM_API_ReadFileData(int nSrc, int StartAdd, int len, unsigned char* pOutput){return 0;}
CALLBACK_DEF	COM_API_WriteFileData(int nSrc, int StartAdd, int len, const unsigned char* pInput){return 0;}

CALLBACK_DEF	COM_API_Jog(int nAxis, float fDis, float Speed){return 0;}
CALLBACK_DEF	COM_API_Home(int nXAxisSet, int nYAxisSet, int nZAxisSet){return 0;}
CALLBACK_DEF	COM_API_StopAxis(int nXAxisSet, int nYAxisSet, int nZAxisSet){return 0;}
CALLBACK_DEF	COM_API_StopAll(){return 0;}
CALLBACK_DEF	COM_API_SetOutputBit(int OutputID, int nStatus){return 0;}
CALLBACK_DEF	COM_API_GetLastError(unsigned int* dwErr){return 0;}//
CALLBACK_DEF	COM_API_DowloadSystemCfg(const char* iniPath){return 0;}//
CALLBACK_DEF	COM_API_DowloadFile(const char* szPath, int nType, int ShowProcess){return 0;}
CALLBACK_DEF	COM_API_SendData(int nLen, const unsigned char* pData){return 0;}
CALLBACK_DEF	COM_API_ReadData(int nLen, const unsigned char* pInput, unsigned char* pOutput){return 0;}
CALLBACK_DEF	COM_API_GetMachineStatus(struct _MACHINE_STATUS_* unStatus) { return 0; }
CALLBACK_DEF	COM_API_PauseAll() { return 0; }
CALLBACK_DEF	COM_API_ResumeAll() { return 0; }

CALLBACK_DEF	COM_API_UploadSystemCfg(const char* iniPath) { return 0; }
CALLBACK_DEF	COM_API_DeleteFile(const char* payload, int len) { return 0; }
CALLBACK_DEF	COM_API_GetFileName(unsigned char* result) { return 0; }
CALLBACK_DEF	COM_API_StartAutoRun(char a1, char a2, void* Src, int Size, char a5) { return 0; }

CALLBACK_DEF	COM_API_FastLine3(int a1, int a2, int a3, int a4, int a5) { return 0; }
IMPORTEXPORT int __stdcall	COM_API_UpdateSystemCfg(const char* iniPath) { return 0; }