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
CALLBACK_DEF	COM_API_StartAutoRun(int Type, int nId, char* FileName, int Len, int isRunOnTime) { return 0; }

#if defined(_M_IX86) 
CALLBACK_DEF	COM_API_DeleteFile(const char* payload, int len) { return 0; }
CALLBACK_DEF	COM_API_GetFileName(FILE_INFO* result) { return 0; }
// Unkown usage
CALLBACK_DEF	COM_API_FastLine3(float a1, float a2, float a3, float a4, float a5) { return 0; }
#endif

#if defined(_M_X64) 
// Unkown usage
IMPORTEXPORT int __stdcall	COM_API_FastLine3(float a1, float a2, float a3, float a4, float a5) { return 0; }
#endif

IMPORTEXPORT int __stdcall	COM_API_UpdateSystemCfg(char* iniPath) { return 0; }