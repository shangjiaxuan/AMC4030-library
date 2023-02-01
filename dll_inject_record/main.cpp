#define _BUILD_SYMBOLS
#include "AMC4030_controller.h"

#include <Windows.h>

#include <mutex>

struct AMC4030DLL
{
	std::once_flag init_flag;

	HMODULE hmodule;

	int(__stdcall* pCOM_API_SetComType)(int nType);
	int(__stdcall* pCOM_API_OpenLink)(int nID, int nBound);
	int(__stdcall* pCOM_API_ReadFileData)(int nSrc, int StartAdd, int len, unsigned char* pOutput);

	int(__stdcall* pCOM_API_WriteFileData)(int nSrc, int StartAdd, int len, const unsigned char* pInput);
	int(__stdcall* pCOM_API_Jog)(int nAxis, float fDis, float Speed);
	int(__stdcall* pCOM_API_Home)(int nXAxisSet, int nYAxisSet, int nZAxisSet);

	int(__stdcall* pCOM_API_StopAxis)(int nXAxisSet, int nYAxisSet, int nZAxisSet);
	int(__stdcall* pCOM_API_StopAll)();
	int(__stdcall* pCOM_API_SetOutputBit)(int OutputID, int nStatus);

	int(__stdcall* pCOM_API_GetLastError)(unsigned int* dwErr);
	int(__stdcall* pCOM_API_DowloadSystemCfg)(const char* iniPath);
	int(__stdcall* pCOM_API_DowloadFile)(const char* szPath, int nType, int ShowProcess);

	int(__stdcall* pCOM_API_SendData)(int nLen, const unsigned char* pData);
	int(__stdcall* pCOM_API_ReadData)(int nLen, const unsigned char* pInput, unsigned char* pOutput);
	int(__stdcall* pCOM_API_GetMachineStatus)(struct _MACHINE_STATUS_* unStatus);

	int(__stdcall* pCOM_API_PauseAll)();
	int(__stdcall* pCOM_API_ResumeAll)();
	int(__stdcall* pCOM_API_UploadSystemCfg)(const char* iniPath);

	int(__stdcall* pCOM_API_DeleteFile)(const char* payload, int len);
	int(__stdcall* pCOM_API_GetFileName)(unsigned char* result);
	int(__stdcall* pCOM_API_StartAutoRun)(char a1, char a2, void* Src, int Size, char a5);

	int(__stdcall* pCOM_API_FastLine3)(int a1, int a2, int a3, int a4, int a5);
	int(__stdcall* pCOM_API_UpdateSystemCfg)(const char* iniPath);
} g_dll;

struct tracer
{
	std::once_flag init_flag;
	std::mutex trace_mtx;
	FILE* tracefile;
	bool in_record = false;

	class record_guard:public std::lock_guard<std::mutex>
	{
		tracer& parent;
	public:
		record_guard(tracer& target);;
		~record_guard();
	};

	// always below lock_guard
	inline void begin_record()
	{
		if(in_record) throw;
		in_record = true;
	}
	inline void WriteWord(int word)
	{
		if (!in_record) throw;
		fwrite(&word, sizeof(word), 1, tracefile);
	}
	inline void WriteBuffer(const void* buffer, int size)
	{
		if (!in_record) throw;
		fwrite(buffer, 1, size, tracefile);
		if (size % 4) {
			int zero = 0;
			fwrite(&zero, 1, 4-(size % 4), tracefile);
		}
	}
	inline void end_record()
	{
		if (!in_record) throw;
		fflush(tracefile);
		in_record = false;
	}
} g_tracer;



void initialize_hook()
{
	HMODULE dll = LoadLibraryA("AMC4030-orig.dll");
	if (dll == NULL) {
		throw;
	}
	g_dll.hmodule = dll;
	#define GET_DLL_FUNC(ftable, hmodule, member, name) ftable.member = (decltype(ftable.member))GetProcAddress(hmodule, name);
	#define GET_DLL_FUNC_DEFAULT(ftable, hmodule, name) GET_DLL_FUNC(ftable, hmodule, p##name, #name);

	#define GET_FUNC(name) GET_DLL_FUNC_DEFAULT(g_dll, dll, name)
	GET_FUNC(COM_API_SetComType);
	GET_FUNC(COM_API_OpenLink);
	GET_FUNC(COM_API_ReadFileData);

	GET_FUNC(COM_API_WriteFileData);
	GET_FUNC(COM_API_Jog);
	GET_FUNC(COM_API_Home);

	GET_FUNC(COM_API_StopAxis);
	GET_FUNC(COM_API_StopAll);
	GET_FUNC(COM_API_SetOutputBit);

	GET_FUNC(COM_API_GetLastError);
	GET_FUNC(COM_API_DowloadSystemCfg);
	GET_FUNC(COM_API_DowloadFile);

	GET_FUNC(COM_API_SendData);
	GET_FUNC(COM_API_ReadData);
	GET_FUNC(COM_API_GetMachineStatus);

	GET_FUNC(COM_API_PauseAll);
	GET_FUNC(COM_API_ResumeAll);
	GET_FUNC(COM_API_UploadSystemCfg);

	GET_FUNC(COM_API_DeleteFile);
	GET_FUNC(COM_API_GetFileName);
	GET_FUNC(COM_API_StartAutoRun);

	GET_FUNC(COM_API_FastLine3);
	#undef GET_FUNC
	GET_DLL_FUNC(g_dll, dll, pCOM_API_UpdateSystemCfg, "?COM_API_UpdateSystemCfg@@YGHPAD@Z")
}

void initialize_trace()
{
	g_tracer.tracefile = fopen("AMC4030.trace", "ab");
}

void initialize_once()
{
	std::call_once(g_dll.init_flag, initialize_hook);
	std::call_once(g_tracer.init_flag, initialize_trace);
}

inline tracer::record_guard::record_guard(tracer& target):std::lock_guard<std::mutex>(target.trace_mtx), parent(target)
{
	initialize_once();
	parent.begin_record();
}

inline tracer::record_guard::~record_guard()
{
	parent.end_record();
}

inline int round_up_4(int len)
{
	return (len+3)/4;
}

// saves 2 word, input nType and output res
CALLBACK_DEF	COM_API_SetComType(int nType) {
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(4);
	// function name
	g_tracer.WriteWord(0);
	g_tracer.WriteWord(nType);
	int res = g_dll.pCOM_API_SetComType(nType);
	g_tracer.WriteWord(res);
	return res;
}
CALLBACK_DEF	COM_API_OpenLink(int nID, int nBound) { 
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(5);
	// function name
	g_tracer.WriteWord(1);
	g_tracer.WriteWord(nID);
	g_tracer.WriteWord(nBound);
	int res = g_dll.pCOM_API_OpenLink(nID, nBound);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_ReadFileData(int nSrc, int StartAdd, int len, unsigned char* pOutput) {
	tracer::record_guard lck(g_tracer);
	int res = g_dll.pCOM_API_ReadFileData(nSrc, StartAdd, len, pOutput);
	// record size
	g_tracer.WriteWord(6 + round_up_4(len));
	// function name
	g_tracer.WriteWord(2);
	g_tracer.WriteWord(nSrc);
	g_tracer.WriteWord(StartAdd);
	g_tracer.WriteWord(len);
	g_tracer.WriteBuffer(pOutput, len);
	g_tracer.WriteWord(res);
	return res;
}
CALLBACK_DEF	COM_API_WriteFileData(int nSrc, int StartAdd, int len, const unsigned char* pInput) { 
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(6 + round_up_4(len));
	// function name
	g_tracer.WriteWord(3);
	g_tracer.WriteWord(nSrc);
	g_tracer.WriteWord(StartAdd);
	g_tracer.WriteWord(len);
	g_tracer.WriteBuffer(pInput, len);
	int res = g_dll.pCOM_API_WriteFileData(nSrc, StartAdd, len, pInput);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_Jog(int nAxis, float fDis, float Speed) { 
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(6);
	// function name
	g_tracer.WriteWord(4);
	g_tracer.WriteWord(nAxis);
	int temp;
	memcpy(&temp, &fDis, 4);
	g_tracer.WriteWord(temp);
	memcpy(&temp, &Speed, 4);
	g_tracer.WriteWord(temp);
	int res = g_dll.pCOM_API_Jog(nAxis, fDis, Speed);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_Home(int nXAxisSet, int nYAxisSet, int nZAxisSet) {
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(6);
	// function name
	g_tracer.WriteWord(5);
	g_tracer.WriteWord(nXAxisSet);
	g_tracer.WriteWord(nYAxisSet);
	g_tracer.WriteWord(nZAxisSet);
	int res = g_dll.pCOM_API_Home(nXAxisSet, nYAxisSet, nZAxisSet);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_StopAxis(int nXAxisSet, int nYAxisSet, int nZAxisSet) {
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(6);
	// function name
	g_tracer.WriteWord(6);
	g_tracer.WriteWord(nXAxisSet);
	g_tracer.WriteWord(nYAxisSet);
	g_tracer.WriteWord(nZAxisSet);
	int res = g_dll.pCOM_API_StopAxis(nXAxisSet, nYAxisSet, nZAxisSet);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_StopAll() {
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(3);
	// function name
	g_tracer.WriteWord(7);
	int res = g_dll.pCOM_API_StopAll();
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_SetOutputBit(int OutputID, int nStatus) { 
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(5);
	// function name
	g_tracer.WriteWord(8);
	g_tracer.WriteWord(OutputID);
	g_tracer.WriteWord(nStatus);
	int res = g_dll.pCOM_API_SetOutputBit(OutputID, nStatus);
	g_tracer.WriteWord(res);
	return res;
}


CALLBACK_DEF	COM_API_GetLastError(unsigned int* dwErr) {
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(4);
	// function name
	g_tracer.WriteWord(9);
	int res = g_dll.pCOM_API_GetLastError(dwErr);
	g_tracer.WriteWord(*dwErr);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_DowloadSystemCfg(const char* iniPath) {
	tracer::record_guard lck(g_tracer);
	size_t path_len = strlen(iniPath) + 1;
	// record size
	g_tracer.WriteWord(3 + round_up_4(path_len));
	// function name
	g_tracer.WriteWord(10);
	g_tracer.WriteBuffer(iniPath, path_len);
	int res = g_dll.pCOM_API_DowloadSystemCfg(iniPath);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_DowloadFile(const char* szPath, int nType, int ShowProcess) { 
	tracer::record_guard lck(g_tracer);
	size_t path_len = strlen(szPath) + 1;
	// record size
	g_tracer.WriteWord(5 + round_up_4(path_len));
	// function name
	g_tracer.WriteWord(11);
	g_tracer.WriteBuffer(szPath, path_len);
	g_tracer.WriteWord(nType);
	g_tracer.WriteWord(ShowProcess);
	int res = g_dll.pCOM_API_DowloadFile(szPath, nType, ShowProcess);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_SendData(int nLen, const unsigned char* pData) { 
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(5 + round_up_4(nLen));
	// function name
	g_tracer.WriteWord(12);
	g_tracer.WriteWord(nLen);
	g_tracer.WriteBuffer(pData, nLen);
	int res = g_dll.pCOM_API_SendData(nLen, pData);
	g_tracer.WriteWord(res);
	return res;
}

// unknown output size, ignore in trace
CALLBACK_DEF	COM_API_ReadData(int nLen, const unsigned char* pInput, unsigned char* pOutput) { 
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(5 + round_up_4(nLen));
	// function name
	g_tracer.WriteWord(13);
	g_tracer.WriteWord(nLen);
	g_tracer.WriteBuffer(pInput, nLen);
	int res = g_dll.pCOM_API_ReadData(nLen, pInput, pOutput);
	g_tracer.WriteWord(res);
	return res;
}


CALLBACK_DEF	COM_API_GetMachineStatus(struct _MACHINE_STATUS_* unStatus) { 
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(3 + round_up_4(sizeof(_MACHINE_STATUS_)));
	// function name
	g_tracer.WriteWord(14);
	int res = g_dll.pCOM_API_GetMachineStatus(unStatus);
	g_tracer.WriteBuffer(unStatus, sizeof(*unStatus));
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_PauseAll() {
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(3);
	// function name
	g_tracer.WriteWord(15);
	int res = g_dll.pCOM_API_PauseAll();
	g_tracer.WriteWord(res);
	return res;
}
CALLBACK_DEF	COM_API_ResumeAll() {
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(3);
	// function name
	g_tracer.WriteWord(16);
	int res = g_dll.pCOM_API_ResumeAll();
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_UploadSystemCfg(const char* iniPath) {
	tracer::record_guard lck(g_tracer);
	size_t path_len = strlen(iniPath) + 1;
	// record size
	g_tracer.WriteWord(3 + round_up_4(path_len));
	// function name
	g_tracer.WriteWord(17);
	g_tracer.WriteBuffer(iniPath, path_len);
	int res = g_dll.pCOM_API_UploadSystemCfg(iniPath);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_DeleteFile(const char* payload, int len) {
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(5 + round_up_4(len));
	// function name
	g_tracer.WriteWord(18);
	g_tracer.WriteBuffer(payload, len);
	g_tracer.WriteWord(len);
	int res = g_dll.pCOM_API_DeleteFile(payload, len);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_GetFileName(unsigned char* result) { 
	tracer::record_guard lck(g_tracer);
	int res = g_dll.pCOM_API_GetFileName(result);
	int result_len = strlen((const char*)result)+1;
	// record size
	g_tracer.WriteWord(3 + round_up_4(result_len));
	// function name
	g_tracer.WriteWord(19);
	g_tracer.WriteBuffer(result, result_len);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_StartAutoRun(char a1, char a2, void* Src, int Size, char a5) {
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(7 + round_up_4(Size));
	// function name
	g_tracer.WriteWord(20);
	g_tracer.WriteWord(a1);
	g_tracer.WriteWord(a2);
	g_tracer.WriteBuffer(Src, Size);
	g_tracer.WriteWord(Size);
	g_tracer.WriteWord(a5);
	int res = g_dll.pCOM_API_StartAutoRun(a1, a2, Src, Size, a5);
	g_tracer.WriteWord(res);
	return res;
}

CALLBACK_DEF	COM_API_FastLine3(int a1, int a2, int a3, int a4, int a5) {
	tracer::record_guard lck(g_tracer);
	// record size
	g_tracer.WriteWord(8);
	// function name
	g_tracer.WriteWord(21);
	g_tracer.WriteWord(a1);
	g_tracer.WriteWord(a2);
	g_tracer.WriteWord(a3);
	g_tracer.WriteWord(a4);
	g_tracer.WriteWord(a5);
	int res = g_dll.pCOM_API_FastLine3(a1, a2, a3, a4, a5);
	g_tracer.WriteWord(res);
	return res;
}

IMPORTEXPORT int __stdcall	COM_API_UpdateSystemCfg(const char* iniPath) { 
	tracer::record_guard lck(g_tracer);
	size_t path_len = strlen(iniPath) + 1;
	// record size
	g_tracer.WriteWord(3 + round_up_4(path_len));
	// function name
	g_tracer.WriteWord(22);
	g_tracer.WriteBuffer(iniPath, path_len);
	int res = g_dll.pCOM_API_UpdateSystemCfg(iniPath);
	g_tracer.WriteWord(res);
	return res;
}
