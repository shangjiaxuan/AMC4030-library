#include <Windows.h>

#define PIPE_NAME "\\\\.\\pipe\\testpipe"

inline int round_up_4(int len)
{
	return (len + 3) / 4;
}

class tracer
{
	HANDLE named_pipe = INVALID_HANDLE_VALUE;
	bool send_record = false;
	bool in_record = false;
	int buffer[1024]{};
	int word_count = 0;
public:
	inline int init_tracer()
	{
		named_pipe = CreateNamedPipeA(PIPE_NAME, PIPE_ACCESS_OUTBOUND,
			PIPE_TYPE_MESSAGE | PIPE_NOWAIT, 2, 4096, 4096, 0, NULL);
		if (named_pipe == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			return err;
		}
		return 0;
	}
	~tracer()
	{
		DWORD written;
		WriteFile(named_pipe, NULL, 0,&written, NULL);
		if (named_pipe != INVALID_HANDLE_VALUE) {
			CloseHandle(named_pipe);
		}
	}
	inline void begin_record()
	{
		if (in_record) throw;
		in_record = true;
		
		if (ConnectNamedPipe(named_pipe, NULL)) {
			// client disconnected
			DisconnectNamedPipe(named_pipe);
			ConnectNamedPipe(named_pipe, NULL);
		}
		else {
			DWORD state = GetLastError();
			send_record = false;
			switch (state) {
				case ERROR_PIPE_LISTENING:
					// no client, do not send message
					send_record = false;
					break;
				case ERROR_PIPE_CONNECTED:
					// working pipe
					send_record = true;
					break;
				case ERROR_NO_DATA:
					// client disconnected
					send_record = false;
					DisconnectNamedPipe(named_pipe);
					break;
				default:
					send_record = false;
			}
		}

		if (!send_record) return;
		word_count = 1;
	}
	inline void WriteWord(int word)
	{
		if (!in_record) throw;
		if (!send_record) return;
		if (word_count >= 1024) throw;
		buffer[word_count] = word;
		++word_count;
	}
	inline void WriteBuffer(const void* src, int size)
	{
		if (!in_record) throw;
		if (!send_record) return;
		int count = round_up_4(size);
		if (word_count + count > 1024) throw;
		memcpy(&buffer[word_count], src, size);
		word_count += count;
	}
	inline void end_record()
	{
		if (!in_record) throw;
		in_record = false;
		if (!send_record) return;
		buffer[0] = word_count;
		DWORD written;
		BOOL success = WriteFile(named_pipe, buffer, word_count*4,
		&written, NULL);
		if (!success || written != word_count * 4) {
			// failed write
		}
		FlushFileBuffers(named_pipe);
	}
};

#include <iostream>
#include <string>
int main()
{
	tracer t;
	if (t.init_tracer()) {
		return -1;
	}
	while (1) {
		std::string line;
		std::getline(std::cin, line);
		t.begin_record();
		t.WriteWord(23);
		t.WriteBuffer(line.data(), line.size());
		t.end_record();
	}
	return 0;
}
