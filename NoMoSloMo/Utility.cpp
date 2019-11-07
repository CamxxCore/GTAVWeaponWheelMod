#include "stdafx.h"
#include <time.h>

static HMODULE ourModule;

HMODULE Utility::GetActiveModule()
{
	HMODULE hModule = NULL;

	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCSTR>(&GetActiveModule),
		&hModule);

	return hModule;
}

std::string Utility::GetModuleName(HMODULE hModule)
{
	TCHAR inBuf[MAX_PATH];

	GetModuleFileNameA(hModule, inBuf, sizeof(inBuf));

	auto str = std::string(inBuf);

	auto seperator = str.find_last_of("\\");

	if (seperator != std::string::npos)
		seperator += 1;

	return str.substr(seperator, str.find_last_of(".") - seperator);
}

std::string Utility::GetWorkingDirectory()
{
	TCHAR inBuf[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, inBuf);

	return std::string(inBuf);
}

std::string Utility::GetShortTimeString()
{
	time_t dtNow = time(NULL);

	tm t;

	localtime_s(&t, &dtNow);

	char inBuf[200];

	sprintf_s(inBuf, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);

	return std::string(inBuf);
}

bool Utility::FileExists(std::string fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

void Utility::SetOurModuleHandle(HMODULE module)
{
	ourModule = module;
}

HMODULE Utility::GetOurModuleHandle() {

	return ourModule;
}
