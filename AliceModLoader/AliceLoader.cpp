#include "AliceLoader.h"
#include "Config.h"

bool AliceLoader::enableConsole = false;
bool AliceLoader::waitForDebugger = false;
bool AliceLoader::skipDLLs = false;
bool AliceLoader::isDebug = false;

float AliceLoader::fpsTarget = 60;
std::string AliceLoader::patcherDir;

bool CheckValidity(void* pAddress, size_t nSize)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery(pAddress, &mbi, sizeof(mbi)))
	{
		if (mbi.Protect & (PAGE_EXECUTE_READ | PAGE_READWRITE | PAGE_READONLY))
			return true;
	}
	return false;
}


void AliceLoader::initLoader()
{
#if _DEBUG
	// For debugging steps taken before the if() statement below.
	AliceLoader::isDebug = true;
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif

	Config::loadConfig();
	if (!AliceLoader::isDebug && enableConsole)
	{
		AllocConsole();
		SetConsoleTitleA("AliceLoader Console");
		freopen("CONOUT$", "w", stdout);
	}	
	printf("<> AliceModLoader <>\n\n");

	AliceLoader::identifyApp();
}


void AliceLoader::identifyApp()
{
	char appName[] = { 0x41, 0x4D, 0x5F, 0x57, 0x49, 0x4E }; // AM_WIN
	int  address[] = { 0x6F1464, 0x5F2474 }; // S4E2, S4E2_beta8
	int i = 0;

	while (i < (sizeof(address)))
	{
		if (CheckValidity((char*)address[i], sizeof(appName)) && !memcmp((char*)address[i], appName, sizeof(appName)))
			break;
		i++;
	}

	switch (i)
	{
	case 0:
	{
		printf("Game: Episode II\n\n");
		if (AliceLoader::fpsTarget != 60.f)
		{
			printf("Setting target refresh rate to %.f\n", AliceLoader::fpsTarget);
			WRITE_MEMORY(0x6F146C, float, AliceLoader::fpsTarget);
		}
		break;
	}
	case 1:
	{
		printf("Game: Episode II (Beta8)\n\n"); AliceLoader::skipDLLs = true;
		if (AliceLoader::fpsTarget != 60.f)
		{
			printf("Setting target refresh rate to %.f\n", AliceLoader::fpsTarget);
			WRITE_MEMORY(0x5F247C, float, AliceLoader::fpsTarget);
		}
		break;
	}
	default:
	{ printf("No known game detected!\n"); AliceLoader::skipDLLs = false; break; }
	}
}

void AliceLoader::launchExternalPatcher()
{
	if (!patcherDir.empty())
	{
		PROCESS_INFORMATION pi = { 0 };
		STARTUPINFO si = { 0 };

		bool patchPrc = CreateProcess(TEXT(patcherDir.c_str()), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
		if (!patchPrc)
			std::cout << "Error creating patcher process. Error code: " << GetLastError() << std::endl;

		//BringWindowToTop((HWND)pi.dwProcessId); Uneccessary actually

		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else printf("External patcher not specified, skipping...\n");
}

void AliceLoader::TestFunc() // Yeah, I'm aware this is stupid.
{
	printf("\n");
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