#include "AliceLoader.h"
#include "Config.h"

bool AliceLoader::enableConsole = false;
bool AliceLoader::waitForDebugger = false; // Change this to true if you need to attach
std::string AliceLoader::patcherDir;

void AliceLoader::TestFunc() // Yeah, I'm aware this is stupid.
{
	// CPKRedir
	std::string mod = Config::configPath + "cpkredir.dll";
	LPCTSTR defMod = mod.c_str();

	printf("Loading module %s..\n", defMod);
	auto hModule = LoadLibraryA(defMod);
	if (!hModule)
		printf("Unable to load CPKRedir\n\n");
	else printf("Successfully loaded!\n\n");

	// Ep2Debug
	std::string mod2 = Config::configPath + "EP2Debug.dll";
	LPCTSTR defMod2 = mod2.c_str();

	printf("Loading module %s..\n", defMod2);
	auto hModule2 = LoadLibraryA(defMod2);
	if (hModule2)
	{
		printf("Successfully loaded!\n\n");
		auto* pProc = (InitFunc_t*)GetProcAddress(hModule2, "Init");
		pProc();
	}
	else
		printf("Unable to load EP2Debug\n\n");
	
}

void AliceLoader::launchExternalPatcher()
{
	if (!patcherDir.empty())
	{
		PROCESS_INFORMATION pi = {0};
		STARTUPINFO si = { 0 };

		bool patchPrc = CreateProcess(TEXT(patcherDir.c_str()), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
		if (!patchPrc)
			std::cout << "Error creating patcher process. Error code: " << GetLastError() << std::endl;
		
		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else printf("External patcher not listed in config file, skipping\n\n");
}

void AliceLoader::initLoader()
{
#if _DEBUG
	// For debugging steps taken before the if() statement below.
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif

	Config::loadConfig();
	if (enableConsole)
	{
		AllocConsole();
		SetConsoleTitleA("AliceLoader Console");
		freopen("CONOUT$", "w", stdout);
	}	

	printf("<> AliceModLoader <>\n\n");
}