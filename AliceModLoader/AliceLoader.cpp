#include "AliceLoader.h"
#include "Config.h"

bool AliceLoader::enableConsole = false;
bool AliceLoader::waitForDebugger = false;
bool AliceLoader::skipDLLs = false;
bool AliceLoader::isDebug = false;

float AliceLoader::fpsTarget = 60;
std::string AliceLoader::patcherDir;

/// <summary>
/// Checks if process memory access is possible at a given address.
/// </summary>
/// <param name="pAddress">Pointer address to section</param>
/// <param name="nSize">Size of the queried section</param>
/// <returns></returns>
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


void AliceLoader::InitLoader()
{
#if _DEBUG
	// For debugging steps taken before the if() statement below.
	AliceLoader::isDebug = true;
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif

	Config::LoadConfig();
	if (!AliceLoader::isDebug && enableConsole)
	{
		AllocConsole();
		SetConsoleTitleA("AliceLoader Console");
		freopen("CONOUT$", "w", stdout);
	}	
	printf("<> AliceModLoader <>\n\n");

	AliceLoader::IdentifyApp();
}

/// <summary>
/// Scan the calling process memory to detect the game, thus set the loader mode.
/// </summary>
void AliceLoader::IdentifyApp()
{
	char appName[] = { 0x41, 0x4D, 0x5F, 0x57, 0x49, 0x4E }; // AM_WIN
	int  address[] = { 0x6F1464, 0x5F2474 }; // S4E2, S4E2_beta8
	int i = 0;

	// Test if we can access the given location first, then check if the location contains "AM_WIN"
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
		{ 
			printf("No known game detected!\n");
			AliceLoader::skipDLLs = false; break;
		}
	}
}

/// <summary>
/// Launches an external program defined in the config file. Intended for use with OSA413's AMBPatcher.
/// </summary>
void AliceLoader::LaunchExternalPatcher()
{
	if (!patcherDir.empty())
	{
		PROCESS_INFORMATION pi = { 0 };
		STARTUPINFO si = { 0 };

		bool patchPrc = CreateProcess(TEXT(patcherDir.c_str()), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
		if (!patchPrc)
			std::cout << "Error creating patcher process. Error code: " << GetLastError() << std::endl;

		//BringWindowToTop((HWND)pi.dwProcessId);     Uneccessary actually

		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else printf("External patcher not specified, skipping...\n");
}

// This function is called in D3D9Hook.cpp @ void HookD3D9(). We do this because the executable is decompressed at this state,
// allowing us to freely load in external DLL files and/or directly patch the process memory without causing SteamStub issues.
void AliceLoader::TestFunc()
{
	printf("\n");

	/*
		Arg 1: DLL file to be loaded
		Arg 2: Relative path from \AML\ to file
		Arg 3: Friendly name (For console output, defaults to arg 1 if undefined)

		"EP2Debug.dll"           retrieved from:  {ModFolder}/mod.ini, [Mod] DLLFile=""
		"Mods/Episode 2 Debug"   retrieved from:  Mod Database
		"Episode 2 Debug"        retrieved from:  {ModFolder}/mod.ini, [Description] Title=""
	*/


	// Korama's CPKREDIR
	LoadExternalModule("cpkredir.dll", "", "CPKREDIR");

	// Ep2 Debug
	LoadExternalModule("EP2Debug.dll", "", "Episode 2 Debug");


	printf("\n");
}


// TODO: Check if a DLL has already been loaded. 
void LoadDll(const char* dll)
{
	auto hModule = LoadLibraryA(dll);

	if (hModule)
	{
		printf(" >> Load Successful!\n");

		// Call the loaded DLL's Init() function.
		auto* pProc = (InitFunc_t*)GetProcAddress(hModule, "Init");
		if (pProc)
			pProc();

		/*if (hModule != hExists)
		{
		}
		else
		{
			printf(" >> Failed to load module, it already exists!\n");
			FreeLibrary(hModule);
		}*/
	}
	else 
		printf(" >> Failed to load the module!\n");
}

void AliceLoader::LoadExternalModule(std::string file, std::string relativePath, std::string name)
{
	std::string moduleDir = Config::configPath + relativePath + "\\" + file;

	printf("Loading module from mod: \"%s\"...\n", name != "" ? name.c_str() : file.c_str());
	LoadDll(moduleDir.c_str());
}

void AliceLoader::LoadExternalModule_Direct(std::string filePath)
{
	printf("Loading module @ \"%s\"...\n", filePath.c_str());
	LoadDll(filePath.c_str());
}