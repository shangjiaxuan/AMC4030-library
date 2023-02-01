#include <Windows.h>

#define PIPE_NAME "\\\\.\\pipe\\AMC4030-tracepipe"

#include <AMC4030_controller.h>

#include <stdio.h>

typedef void (*record_handler)(int buffer[1024]);

struct record_info
{
	const char* func_name;
	record_handler handler;
};
extern record_info g_handler_table[23];

#include <fstream>

class tracer_client
{
	HANDLE named_pipe;
	int buffer[1024]{};

	bool ignored[23]{};
	//FILE* trace_file;
public:
	tracer_client()
	{
		named_pipe = CreateFileA(PIPE_NAME, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (named_pipe == INVALID_HANDLE_VALUE) {
			MessageBoxA(NULL, "Failed connecting to tracer", NULL, MB_OK);
			exit(1);
		}
		DWORD mode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
		SetNamedPipeHandleState(named_pipe, &mode, NULL, NULL);
		//trace_file = fopen("test.trace", "ab");
		std::ifstream ifs;
		ifs.open("ignored_record.txt");
		if (!ifs) {
			return;
		}
		while (1) {
			int item;
			ifs >> item;
			if (!ifs) break;
			if (item >= 0 && item < 23) {
				ignored[item] = true;
			}
		}
	}
	~tracer_client()
	{
		CloseHandle(named_pipe);
		//fclose(trace_file);
	}

	void next_message()
	{
		DWORD word_count;
		if (!ReadFile(named_pipe, &buffer, 4096, &word_count, NULL)) {
			DWORD err = GetLastError();
			if (err == ERROR_BROKEN_PIPE) {
				MessageBoxA(NULL, "Server closed", "Info", MB_OK);
				exit(0);
			}
			printf("Error reading message: GLE: %X\n", err);
		}
		else if (word_count & 3 || buffer[0] != word_count >> 2) {
			printf("Message size mismatch\n");
		}
		else {
			if (!ignored[buffer[1]]) {
				handle_message();
			}
		}
	}
	void handle_message()
	{
		//printf("%s\n",(const char*)&buffer[2]);
		//fwrite(&buffer, 4, buffer[0], trace_file);
		g_handler_table[buffer[1]].handler(buffer);
	}
};

int main()
{
	tracer_client c;
	while (1) {
		c.next_message();
	}
}

inline int round_up_4(int len)
{
	return (len + 3) / 4;
}


void SetComType_handler(int record[1024])
{
	printf("SetComType(%d)\t= %d\n",record[2],record[3]);
}

void OpenLink_handler(int record[1024])
{
	printf("OpenLink(%d, %d)\t= %d\n", record[2], record[3], record[4]);
}

void ReadFileData_handler(int record[1024])
{
	printf("ReadFileData(%d, %d, %d, output_payload<not printing>)\t= %d\n", record[2], record[3], record[4], record[4 + round_up_4(record[4])]);
}

void WriteFileData_handler(int record[1024])
{
	printf("WriteFileData(%d, %d, %d, input_payload<not printing>)\t= %d\n", record[2], record[3], record[4], record[4 + round_up_4(record[4])]);
}

void Jog_handler(int record[1024])
{
	float fDis;
	memcpy(&fDis, &record[3], 4);
	float Speed;
	memcpy(&Speed, &record[4], 4);
	printf("Jog(%d, %.6f, %.6f)\t= %d\n", record[2], fDis, Speed, record[5]);
}

void Home_handler(int record[1024])
{
	printf("Home(%d, %d, %d)\t= %d\n", record[2], record[3], record[4], record[5]);
}

void StopAxis_handler(int record[1024])
{
	printf("StopAxis(%d, %d, %d)\t= %d\n", record[2], record[3], record[4], record[5]);
}

void StopAll_handler(int record[1024])
{
	printf("StopAll()\t= %d\n", record[2]);
}

void SetOutputBit_handler(int record[1024])
{
	printf("SetOutputBit(%d, %d)\t= %d\n", record[2], record[3], record[4]);
}

void GetLastError_handler(int record[1024])
{
	printf("GetLastError(out %d)\t= %d\n", record[2], record[3]);
}

void DowloadSystemCfg_handler(int record[1024])
{
	const char* str = (const char*)&record[2];
	int skip = round_up_4(strlen(str)+1);
	printf("DowloadSystemCfg(\"%s\")\t= %d\n", str, record[2 + skip]);
}

void DowloadFile_handler(int record[1024])
{
	const char* str = (const char*)&record[2];
	int skip = round_up_4(strlen(str) + 1);
	printf("DowloadFile(\"%s\", %d, %d)\t= %d\n", str, record[2 + skip], record[3 + skip], record[4 + skip]);
}

void SendData_handler(int record[1024])
{
	int skip = round_up_4(record[2]);
	printf("SendData(%d, input_payload<not printing>)\t= %d\n", record[2], record[3 + skip]);
}

void ReadData_handler(int record[1024])
{
	int skip = round_up_4(record[2]);
	printf("ReadData(%d, input_payload<not printing>, output_payload<not recorded>)\t= %d\n", record[2], record[3 + skip]);
}

void GetMachineStatus_handler(int record[1024])
{
	int skip = round_up_4(sizeof(MACHINE_STATUS));
	MACHINE_STATUS sts;
	memcpy(&sts, &record[2], sizeof(MACHINE_STATUS));
	printf("GetMachineStatus(sts)\t= %d\n\tsts:\n"
		"\tdwWorkStatus\t= %X\n"
		"\tdwHomeDone\t= %hhX\n"
		"\tID\t= %hhu\n"
		"\tFirmVer\t= %hu\n"
		"\tPos\t= (%d, %d, %d)\n"
		"\tRealSpeed\t= (%u, %u, %u)\n"
		"\tAlmCode\t= %u\n"
		"\tdwInputStatus\t= %hX\n"
		"\tdwOutputStatus\t= %hX\n",
		record[2 + skip], 
		sts.dwWorkStatus,
		sts.dwHomeDone,
		sts.nID,
		sts.FirmVer,
		sts.nPos[0], sts.nPos[1], sts.nPos[2],
		sts.RealSpeed[0], sts.RealSpeed[1], sts.RealSpeed[2],
		sts.nAlmCode,
		sts.dwInputStatus,
		sts.dwOutputStatus);
}

void PauseAll_handler(int record[1024])
{
	printf("PauseAll()\t= %d\n", record[2]);
}

void ResumeAll_handler(int record[1024])
{
	printf("ResumeAll()\t= %d\n", record[2]);
}

void UploadSystemCfg_handler(int record[1024])
{
	const char* str = (const char*)&record[2];
	int skip = round_up_4(strlen(str) + 1);
	printf("UploadSystemCfg(\"%s\")\t= %d\n", str, record[2 + skip]);
}

void DeleteFile_handler(int record[1024])
{
	const char* str = (const char*)&record[2];
	int skip = round_up_4(strlen(str) + 1);
	printf("DeleteFile(\"%s\", %d)\t= %d\n", str, record[2 + skip], record[3 + skip]);
}

void GetFileName_handler(int record[1024])
{
	int skip = round_up_4(sizeof(FILE_INFO));
	FILE_INFO names;
	memcpy(&names, &record[2], sizeof(FILE_INFO));

	char scratch_buffer1[512];
	if(names.file_total) {
		
		snprintf(scratch_buffer1, 512, "\n\t%s", names.filename[0]);
		char scratch_buffer2[512]{};
		int i = 1;
		for (; i < names.file_total && i < 10; ++i) {
			snprintf(scratch_buffer2, 512, "%s\n\t%s", scratch_buffer1, names.filename[i]);
			strncpy(scratch_buffer1, scratch_buffer2, 512);
		}
		
	}
	printf("GetFileName(names)\t= %d\n\tnames:%s\n\tfile_total\t:%d\n", record[2 + skip], scratch_buffer1, names.file_total);
}

void StartAutoRun_handler(int record[1024])
{
	const char* str = (const char*)&record[4];
	int skip = round_up_4(strlen(str) + 1);
	printf("StartAutoRun(%d, %d, \"%s\", %d, %d)\t= %d\n", record[2], record[3], str, record[3 + skip], record[4 + skip], record[5 + skip]);
}

void FastLine3_handler(int record[1024])
{
	printf("FastLine3(%d, %d, %d, %d, %d)\t= %d\n", record[2], record[3], record[4], record[5], record[6], record[7]);
}

void UpdateSystemCfg_handler(int record[1024])
{
	const char* str = (const char*)&record[2];
	int skip = round_up_4(strlen(str) + 1);
	printf("UpdateSystemCfg(\"%s\")\t= %d\n", str, record[2 + skip]);
}

#define REGISTER_HANDLER(name) {#name, name##_handler}

record_info g_handler_table[23] = {
	REGISTER_HANDLER(SetComType),
	REGISTER_HANDLER(OpenLink),
	REGISTER_HANDLER(ReadFileData),

	REGISTER_HANDLER(WriteFileData),
	REGISTER_HANDLER(Jog),
	REGISTER_HANDLER(Home),

	REGISTER_HANDLER(StopAxis),
	REGISTER_HANDLER(StopAll),
	REGISTER_HANDLER(SetOutputBit),

	REGISTER_HANDLER(GetLastError),
	REGISTER_HANDLER(DowloadSystemCfg),
	REGISTER_HANDLER(DowloadFile),

	REGISTER_HANDLER(SendData),
	REGISTER_HANDLER(ReadData),
	REGISTER_HANDLER(GetMachineStatus),

	REGISTER_HANDLER(PauseAll),
	REGISTER_HANDLER(ResumeAll),
	REGISTER_HANDLER(UploadSystemCfg),

	#undef DeleteFile
	REGISTER_HANDLER(DeleteFile),
	REGISTER_HANDLER(GetFileName),
	REGISTER_HANDLER(StartAutoRun),

	REGISTER_HANDLER(FastLine3),
	REGISTER_HANDLER(UpdateSystemCfg)
};
