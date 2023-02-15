#ifndef COM_INTERFACE_H_BY_TP_20170313
#define COM_INTERFACE_H_BY_TP_20170313

#include <stdint.h>

#ifdef __cplusplus
#define EXTERNAL_C_API extern "C"
extern "C" {
#else
#define EXTERNAL_C_API
#endif

#ifdef _BUILD_SYMBOLS
#define IMPORTEXPORT __declspec(dllexport)
#else
#define IMPORTEXPORT __declspec(dllimport)
#endif

// x86 stdcall is different from cdecl
#ifdef _M_IX86 
#define CALLBACK_DEF  EXTERNAL_C_API IMPORTEXPORT int __stdcall
#endif

// x64 stdcall is same as cdecl (default)
#ifdef _M_X64 
#define CALLBACK_DEF  EXTERNAL_C_API  IMPORTEXPORT int
#endif

#if defined(_M_IX86) && defined(_BUILD_SYMBOLS) 
#pragma comment(linker, "/export:COM_API_DeleteFile=_COM_API_DeleteFile@8,@1")
#pragma comment(linker, "/export:COM_API_DowloadFile=_COM_API_DowloadFile@12,@2")
#pragma comment(linker, "/export:COM_API_DowloadSystemCfg=_COM_API_DowloadSystemCfg@4,@3")
#pragma comment(linker, "/export:COM_API_FastLine3=_COM_API_FastLine3@20,@4")
#pragma comment(linker, "/export:COM_API_GetFileName=_COM_API_GetFileName@4,@5")
#pragma comment(linker, "/export:COM_API_GetLastError=_COM_API_GetLastError@4,@6")
#pragma comment(linker, "/export:COM_API_GetMachineStatus=_COM_API_GetMachineStatus@4,@7")
#pragma comment(linker, "/export:COM_API_Home=_COM_API_Home@12,@8")
#pragma comment(linker, "/export:COM_API_Jog=_COM_API_Jog@12,@9")
#pragma comment(linker, "/export:COM_API_OpenLink=_COM_API_OpenLink@8,@10")
#pragma comment(linker, "/export:COM_API_PauseAll=_COM_API_PauseAll@0,@11")
#pragma comment(linker, "/export:COM_API_ReadData=_COM_API_ReadData@12,@12")
#pragma comment(linker, "/export:COM_API_ReadFileData=_COM_API_ReadFileData@16,@13")
#pragma comment(linker, "/export:COM_API_ResumeAll=_COM_API_ResumeAll@0,@14")
#pragma comment(linker, "/export:COM_API_SendData=_COM_API_SendData@8,@15")
#pragma comment(linker, "/export:COM_API_SetComType=_COM_API_SetComType@4,@16")
#pragma comment(linker, "/export:COM_API_SetOutputBit=_COM_API_SetOutputBit@8,@17")
#pragma comment(linker, "/export:COM_API_StartAutoRun=_COM_API_StartAutoRun@20,@18")
#pragma comment(linker, "/export:COM_API_StopAll=_COM_API_StopAll@0,@19")
#pragma comment(linker, "/export:COM_API_StopAxis=_COM_API_StopAxis@12,@20")
#pragma comment(linker, "/export:?COM_API_UpdateSystemCfg@@YAHPEAD@Z,@21")
#pragma comment(linker, "/export:COM_API_UploadSystemCfg=_COM_API_UploadSystemCfg@4,@22")
#pragma comment(linker, "/export:COM_API_WriteFileData=_COM_API_WriteFileData@16,@23")
#endif

#if defined(_M_X64) && defined(_BUILD_SYMBOLS) 
//#pragma comment(linker, "/export:COM_API_DeleteFile=_COM_API_DeleteFile@8,@1")
#pragma comment(linker, "/export:COM_API_DowloadFile,@1")
#pragma comment(linker, "/export:COM_API_DowloadSystemCfg,@2")
#pragma comment(linker, "/export:?COM_API_FastLine3@@YAHMMMMM@Z,@3")
//#pragma comment(linker, "/export:COM_API_GetFileName=_COM_API_GetFileName@4,@5")
#pragma comment(linker, "/export:COM_API_GetLastError,@4")
#pragma comment(linker, "/export:COM_API_GetMachineStatus,@5")
#pragma comment(linker, "/export:COM_API_Home,@6")
#pragma comment(linker, "/export:COM_API_Jog,@7")
#pragma comment(linker, "/export:COM_API_OpenLink,@8")
#pragma comment(linker, "/export:COM_API_PauseAll,@9")
#pragma comment(linker, "/export:COM_API_ReadData,@10")
#pragma comment(linker, "/export:COM_API_ReadFileData,@11")
#pragma comment(linker, "/export:COM_API_ResumeAll,@12")
#pragma comment(linker, "/export:COM_API_SendData,@13")
#pragma comment(linker, "/export:COM_API_SetComType,@14")
#pragma comment(linker, "/export:COM_API_SetOutputBit,@15")
#pragma comment(linker, "/export:COM_API_StartAutoRun,@16")
#pragma comment(linker, "/export:COM_API_StopAll,@17")
#pragma comment(linker, "/export:COM_API_StopAxis,@18")
#pragma comment(linker, "/export:?COM_API_UpdateSystemCfg@@YAHPEAD@Z,@19")
#pragma comment(linker, "/export:COM_API_UploadSystemCfg,@20")
#pragma comment(linker, "/export:COM_API_WriteFileData,@21")
#endif

enum AMC4030_ERROR_CODE
{
	S_AMC4030_SUCCESS	= 1,
	E_AMC4030_REFUSED	= -1,
	E_AMC4030_PARAM		= -2,
	E_AMC4030_TIMEOUT	= -4,
	E_AMC4030_LOCKED	= -6,
};

// Sets connection mode
// nType:	2			- usb connection
//			otherwise	- invalid
CALLBACK_DEF	COM_API_SetComType(int nType);

// Establish connection with device, always do this and COM_API_SetComType before anything
// nID:		- ignored
// nBound	baud rate
//			- 115200 fixed
CALLBACK_DEF	COM_API_OpenLink(int nID,int nBound);

typedef struct _MACHINE_STATUS_
{
	// move state (bits?)
	//  0 - paused
	//  1 - processing
	//  2 - moving to point
	//  3 - returning to origin
	//  4 - alarm
	uint32_t	dwWorkStatus;
	// origin state (bits?)
	//	0 - X OK
	//	1 - Y OK
	//	2 - Z OK
	uint8_t		dwHomeDone;
	// machine logical ID
	uint8_t		nID;
	// firmware version
	uint16_t	FirmVer;
	// 100000 times current position
	int32_t		nPos[3];
	// 100000 times current speed
	uint32_t	RealSpeed[3];
	// alarm code
	uint32_t	nAlmCode;
	// Input status (bits?)
	// 0 - IN1
	// 1 - IN2
	// 2 - ???
	// 3 - IN3
	// 4 - IN4
	// 5 - ORG1
	// 6 - ORG2
	// 7 - ORG3
	uint16_t	dwInputStatus;
	// Input status (bits?)
	uint16_t 	dwOutputStatus;
	// reserved
	uint32_t	Rsv[5];
} MACHINE_STATUS, * PMACHINE_STATUS;

CALLBACK_DEF	COM_API_GetMachineStatus(MACHINE_STATUS* unStatus);

// Go to location
// nAxis	: The axis to move
//		0 - X axis
//		1 - Y axis
//		2 - Z axis
// fDis		: distance
// Speed	: speed
CALLBACK_DEF	COM_API_Jog(int nAxis,float fDis,float Speed);

// Go to origin
// nXAxisSet, nXAxisSet, nXAxisSet: Whether the axis goes to origin
//		0 - No
//		1 - Yes
CALLBACK_DEF	COM_API_Home(int nXAxisSet,int nYAxisSet,int nZAxisSet);

// Stops movement
// nXAxisSet, nXAxisSet, nXAxisSet: Whether the axis stops
//		0 - No
//		1 - Yes
CALLBACK_DEF	COM_API_StopAxis(int nXAxisSet,int nYAxisSet,int nZAxisSet);

// Stops all movement, also exits auto mode
CALLBACK_DEF	COM_API_StopAll();
// (guessed)
CALLBACK_DEF	COM_API_PauseAll();
// (guessed)
CALLBACK_DEF	COM_API_ResumeAll();

// Gets last error code in dwErr
CALLBACK_DEF	COM_API_GetLastError(unsigned int* dwErr);

#define MAX_AXIS_NUM		3

// units are in mm, s, unless specified
typedef struct _SYS_MACHINE_PARAM_SET
{
	// controller interpolation period
	float	fTimerPeriod;
	// rotation arc precision
	float	fWorkPrecision;
	// arc curve precision
	float	fArcCheckPrecision;
	// minimum length for line fragment
	float	fMinLen;
	// maximum feed speed (mm/min)
	float	fMaxFeedSpeed;

	// unused, must be 1, S acceleration
	uint16_t   nAccelType;
	// whether returns to start on start
	// 1 if return to origin upon start
	uint16_t 	nHomePowerOn;

	// max acceleration for curves
	float fMaxAccelSpeed;
	// max acceleration for processing
	float fAccelSpeed;
	// max acceleration for no-work
	float fFastAccelSpeed;
	// acceleration change per sec
	float fJAccelSpeed;
	uint32_t  nRsv[20];
} SYS_MACHINE_PARAM, * PSYS_MACHINE_PARAM;


typedef struct _AXIS_PARAM_SET
{
	// number of pulse for one round of rotation
	float   nPulseFactorUp;
	// distance for one round of rotation
	float   nPulseFactorDown;
	// low or high voltage is pulse?
	int	    nPulseLogic;

	// max speed for axis (mm/min)
	float fMaxSpeed;
	// max position
	float fMaxPos;

	// enable back lash compensation
	int   nEnableBackLash;
	// back lash length
	float fBackLashLen;
	// back lash speed
	float fBackLashSpeed;

	// home direction
	int   nHomeDir;
	// return origin speed
	float fHomeSpeed;
	// unused
	float fHomeCheckDis;
	// unused
	float fHomeZeroSpeed;
	// unused
	float fHomeOrgSpeed;
	// offset when return
	float fHomePosOffset;

	uint32_t  nRsv[20];
} AXIS_PARAM, * PAXIS_PARAM;

typedef struct _SYSTEM_PARAM
{
	SYS_MACHINE_PARAM       MachineParam;
	AXIS_PARAM              tAxisParam[MAX_AXIS_NUM];
	uint32_t          dwRsv[100];
} SYS_PARAM, * PSYS_PARAM;

// Reads file from controller, 500 bytes per time max
// nSrc		: The file to read
//		1 - parameters (binary SYS_PARAM struct, corresponds ini file)
//		2 - processing (unimplemented)
//		3 - firmware
// StartAdd	: The offset to start reading
// len		: Bytes to read
// pOutput  : Read buffer
CALLBACK_DEF	COM_API_ReadFileData(int nSrc, int StartAdd, int len, unsigned char* pOutput);

// Writes file to controller, 500 bytes per time max
// nSrc		: The file to write
//		1 - parameters (binary SYS_PARAM struct, corresponds ini file)
//		2 - processing (unimplemented)
//		3 - firmware
// StartAdd	: The offset to start writing
// len		: Bytes to write
// pOutput  : Write buffer
CALLBACK_DEF	COM_API_WriteFileData(int nSrc, int StartAdd, int len, const unsigned char* pInput);

// Sends raw command to controller
//	nLen	- Command bytes
//	pData	- Command
CALLBACK_DEF	COM_API_SendData(int nLen, const unsigned char* pData);

// Sends raw command to controller and gets response
//	nLen	- Command bytes
//	pInput	- Command
//	pOutput	- Result
CALLBACK_DEF	COM_API_ReadData(int nLen, const unsigned char* pInput,unsigned char* pOutput);

// Directly sets output pin state
// OutputID	: The pin (1-4)
// nStatus	: High or low voltage
//		0 - low
//		1 - high
CALLBACK_DEF	COM_API_SetOutputBit(int OutputID, int nStatus);

// Puts ini data to controller (convenience function)
CALLBACK_DEF	COM_API_DowloadSystemCfg(const char* iniPath);

// Gets ini data back from controller (convenience function, guessed)
CALLBACK_DEF	COM_API_UploadSystemCfg(const char* iniPath);

// Puts file to controller
// szPath		: The file
// nType		: File type
//		1 - config file
//		2 - processing file
//		3 - firmware update
//	ShowProcess	: Shows progress in loading bar
CALLBACK_DEF	COM_API_DowloadFile(const char* szPath, int nType, int ShowProcess);

typedef struct _FILE_INFO_
{
	char filename[11][10];
	int file_total;
} FILE_INFO, * PFILE_INFO;

#if defined(_M_IX86) 

// get list of files in controller
CALLBACK_DEF	COM_API_GetFileName(FILE_INFO* fileinfo);

// deletes file from controller
CALLBACK_DEF	COM_API_DeleteFile(const char* filename, int filename_len);
#endif
// Starts script inside controller
//	Type		- file type, 2 for script
//	nId			- have to be 1
//	FileName	- filename in controller, length<9
//	len			- filename length
//	isRunOnTime	- whether the script is run on startup
CALLBACK_DEF	COM_API_StartAutoRun(int Type, int nId, char* FileName, int Len, int isRunOnTime);

#if defined(_M_IX86) 
// Unkown usage
CALLBACK_DEF	COM_API_FastLine3(float a1, float a2, float a3, float a4, float a5);
#endif
#ifdef __cplusplus
}
#if defined(_M_X64) 
// Unkown usage
IMPORTEXPORT int __stdcall	COM_API_FastLine3(float a1, float a2, float a3, float a4, float a5);
#endif
// Unkown usage
IMPORTEXPORT int __stdcall	COM_API_UpdateSystemCfg(char* iniPath);
#endif

#endif
