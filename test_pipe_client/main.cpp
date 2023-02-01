#include <Windows.h>

#define PIPE_NAME "\\\\.\\pipe\\testpipe"

#include <stdio.h>

class tracer_client
{
	HANDLE named_pipe;
	int buffer[1024]{};
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
	}
	~tracer_client()
	{
		CloseHandle(named_pipe);
	}

	void next_message()
	{
		DWORD word_count;
		if (!ReadFile(named_pipe, &buffer, 4096, &word_count, NULL)) {
			printf("Error reading message: GLE: %ux", GetLastError());
		}
		else if (word_count & 3 || buffer[0] != word_count >> 2) {
			printf("Message size mismatch");
		}
		else {
			handle_message();
		}
	}
	void handle_message()
	{
		printf("%s\n",(const char*)&buffer[2]);
	}
};

int main()
{
	tracer_client c;
	while (1) {
		c.next_message();
	}
}

