#include "stdafx.h"

bytepatch_t g_patches[3];

CConfig g_config("NoMoSloMo.ini");

float g_timescaleModifier = 1.0f;

bool setupPatches()
{
	auto result = BytePattern((BYTE*)"\x38\x51\x64\x74\x19", "xxxxx");

	if ( !result.success ) {

		LOG("Unable to find offsets. Cannot continue.");

		return false;
	}

	auto address = result.get(26);

	address = address + *(int32_t*)address + 4u;

	g_patches[0] = bytepatch_t( address, std::vector<BYTE> { RET, 0x90, 0x90, 0x90, 0x90 } ); // remove vignetting

	g_patches[1] = bytepatch_t( result.get(8), std::vector<BYTE>(5, 0x90) ); // vignetting call patch (NOP)

	g_patches[2] = bytepatch_t( result.get(34), std::vector<BYTE> { XOR_32_64, 0xD2 } ); // timescale override patch

	return true;
}

std::string getResourceConfigData(HMODULE hModule)
{
	std::string result;

	HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(IDR_RCDATA1), RT_RCDATA);

	if (hRes != NULL)
	{
		HGLOBAL hData = LoadResource(hModule, hRes);

		if (hData != NULL)
		{
			DWORD dataSize = SizeofResource(hModule, hRes);
			char* data = (char*)LockResource(hData);
			result.assign(data, dataSize);
		}
	}

	else LOG("Error finding config resource.");

	return result;
}

void makeConfigFile() {

	if (Utility::FileExists(g_config.filename)) return;

	auto module = Utility::GetOurModuleHandle();

	std::string resText = getResourceConfigData(module);

	if (resText.empty())
	{
		LOG("Retrieved resource text was empty.");
		return;
	}

	std::ofstream ofs(g_config.filename);

	if (!ofs.good())
	{
		LOG("Could not write to created log file.");
		return;
	}

	ofs << resText;

	ofs.flush();
	ofs.close();
}

void initialize() {

	LOG("Creating config file...");

	makeConfigFile();

	LOG("Setup patches...");

	if ( !setupPatches() ) {

		LOG("Initialization failed. Cannot continue.");
		return;
	}

	if (g_config.get<bool>("General", "DisableVignetting", true)) {

		g_patches[0].install();

		g_patches[1].install();
	}

	if (g_config.get<bool>("General", "DisableSlowdown", true)) {

		g_patches[2].install();
	}

	LOG("Completed.");
}

void unload() {

	for (auto p = 0; p < ARRAYSIZE(g_patches); ++p) {

		if (g_patches[p].active)
			g_patches[p].remove();
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Utility::SetOurModuleHandle(hModule);
		initialize();
		break;
	case DLL_PROCESS_DETACH:
		unload();
		break;
	}
	return TRUE;
}

