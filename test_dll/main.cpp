#include <AMC4030_controller.h>

#include <iostream>

#include <vector>
#include <sstream>

typedef void (*test_func)(std::vector<std::string>& args);

class UsageError: public std::runtime_error{
public:
	UsageError():std::runtime_error("usage error") {}
};

int extract_int_arg(std::string& s)
{
	std::istringstream strm(s);
	int res;
	strm >> res;
	strm.peek();
	if (!strm.eof()) throw UsageError();
	return res;
}

void test_SetComType(std::vector<std::string>& args)
{
	try {
		if (args.size() != 2) throw UsageError();
		int nType = extract_int_arg(args[1]);
		int res = COM_API_SetComType(nType);
		std::cout << "SetComType( " << nType << " ) = " << res << "\n";
	}
	catch (UsageError& e) {
		// prompt usage
		std::cout << "Usage:\tint SetComType(int nType)";
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}
	catch (...) {
		std::cout << "???";
	}
}

void test_OpenLink(std::vector<std::string>& args)
{
	try {
		if (args.size() != 3) throw UsageError();
		int nID = extract_int_arg(args[1]);
		int nBound = extract_int_arg(args[2]);
		int res = COM_API_OpenLink(nID, nBound);
		std::cout << "OpenLink( " << nID << ", " << nBound << " ) = " << res << "\n";
	}
	catch (UsageError& e) {
		// prompt usage
		std::cout << "Usage:\tint OpenLink(int nID,int nBound)";
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}
	catch (...) {
		std::cout << "???";
	}
}

MACHINE_STATUS st;

void test_GetMachineStatus(std::vector<std::string>& args)
{
	try {
		if (args.size() != 1) throw UsageError();
		int res = COM_API_GetMachineStatus(&st);
		std::cout << "GetMachineStatus( " << (char*)&st << " ) = " << res << "\n";
	}
	catch (UsageError& e) {
		// prompt usage
		std::cout << "Usage:\tint GetMachineStatus(unsigned char* unStatus)";
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}
	catch (...) {
		std::cout << "???";
	}
}

void interactive()
{
	while (std::cin) {
		std::string line;
		std::getline(std::cin, line);
		std::vector<std::string> args;
		std::istringstream s(line);
		while (true) {
			std::string arg;
			s >> arg;
			if (!s) break;
			args.push_back(arg);
		}
		if (args[0] == "SetComType") {
			test_SetComType(args);
		}
		else if (args[0] == "OpenLink") {
			test_OpenLink(args);
		}
		else if (args[0] == "GetMachineStatus") {
			test_GetMachineStatus(args);
		}
	}
}

// writes 74 bytes max
char name_buffer[74];

int main()
{
	COM_API_UploadSystemCfg((char*)"test.ini");

	Com("test.bin");


	interactive();

	return 0;
}