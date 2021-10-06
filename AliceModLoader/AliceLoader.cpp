#include "AliceLoader.h"
#include "LoaderConfig.h"

bool AliceLoader::enableConsole = false;

void AliceLoader::TestFunc()
{
	std::string mod = AMLConfig::configPath + "EP2Debug.dll";
	LPCTSTR defMod = mod.c_str();

	std::string mod2 = AMLConfig::configPath + "cpkredir.dll";
	LPCTSTR defMod2 = mod2.c_str();

	printf("Loading module %s..\n", defMod2);
	auto hModule2 = LoadLibraryA(defMod2);
	if (!hModule2)
		printf("Unable to load CPKRedir\n");

	printf("Loading module %s..\n\n", defMod);
	auto hModule = LoadLibraryA(defMod);
	if (hModule)
	{
		auto* pProc = (InitFunc_t*)GetProcAddress(hModule, "Init");
		pProc();
	}
	else
		printf("Unable to load specified DLL: %s\n\n", defMod);
}

void AliceLoader::initLoader()
{
	AMLConfig::loadConfig();
	if (enableConsole)
	{
		AllocConsole();
		SetConsoleTitleA("AliceLoader Console");
		freopen("CONOUT$", "w", stdout);
	}

	printf("<> AliceModLoader <>\n\n");

	//TestFunc();
}