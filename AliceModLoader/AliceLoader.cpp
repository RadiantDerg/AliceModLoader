#include "AliceLoader.h"
#include "Config.h"


bool AliceLoader::enableConsole   = false;
bool AliceLoader::waitForDebugger = false;
bool AliceLoader::skipDLLs        = false;
bool AliceLoader::isDebug         = false;

float AliceLoader::ep1Width     = 1280.f;
float AliceLoader::ep1Height    = 720.f;

float AliceLoader::ep2FPSTarget = 60.f;

std::string AliceLoader::patcherDir;


HOOK(HANDLE, __stdcall, _CreateFileA, PROC_ADDRESS("Kernel32.dll", "CreateFileA"), LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	std::string path_ = lpFileName;
	std::string workPath_ = "\\#Work\\" + path_;

	std::string fileRedir = FileService::GetModuleDir().c_str() + workPath_;

	if (AliceLoader::isDebug)
	{
		printf("File Read Detected: \"%s\"\n", path_.c_str());
		//printf("REDIRECT PATH:      \"%s\"\n", fileRedir.c_str());
	}

	// Redirect  if the same file exists in the 'Work' folder
	if (FileService::FileExists(fileRedir.c_str()))
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 11);
		printf("Redirecting \"\\%s\" to \"%s\"\n", path_.c_str(), workPath_.c_str());
		SetConsoleTextAttribute(hConsole, 15);

		return CreateFileW(std::wstring(fileRedir.begin(), fileRedir.end()).c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else
		return CreateFileW(std::wstring(path_.begin(), path_.end()).c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


void AliceLoader::InitLoader()
{
#if _DEBUG
	// Debug output for steps taken before the if() statement below
	AliceLoader::isDebug = true;
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif
	INSTALL_HOOK(_CreateFileA);

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
/// Checks if process memory access is possible at a given address
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

/// <summary>
/// Scan the calling process memory to detect the game, thus set the loader mode
/// </summary>
void AliceLoader::IdentifyApp()
{
	char appName[] = { 0x41, 0x4D, 0x5F, 0x57, 0x49, 0x4E }; // AM_WIN
	int  address[] = { ASLR(0x575C94), 0x6F1464, 0x5F2474 }; // S4E1, S4E2, S4E2_beta8
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
			printf("Game: Episode I\n\n");
			AliceLoader::skipDLLs = true;

			printf("Setting internal resolution to %.fx%.f\n", AliceLoader::ep1Width, AliceLoader::ep1Height);
			WRITE_MEMORY(ASLR(0x575C9C), float, AliceLoader::ep1Height);
			WRITE_MEMORY(ASLR(0x575CA0), float, AliceLoader::ep1Width);
			break;
		}

		case 1:
		{
			printf("Game: Episode II\n\n");
			//WRITE_MEMORY(0x6B369C, uint8_t, 0xDC, 0x35, 0x6C, 0x14, 0x6F, 0x00) // Overwrite instruction that compares 0x742A38 and point to the target FPS
			//WRITE_MEMORY(0x6B369C, uint8_t, 0xDC, 0x35, 0xDC, 0x6A, 0x6F, 0x00) // 6F6ADC
			if (AliceLoader::ep2FPSTarget != 60.f)
			{
				printf("Setting target refresh rate to %.f\n", AliceLoader::ep2FPSTarget);
				WRITE_MEMORY(0x6F146C, float, AliceLoader::ep2FPSTarget);

				//WRITE_MEMORY(0x6F6ADC, float, AliceLoader::ep2FPSTarget);	// Another 60.f I think
				//WRITE_MEMORY(0x742A38, double, AliceLoader::ep2FPSTarget); // Sus Double
			}
			break;
		}
		
		case 2:
		{
			printf("Game: Episode II (Beta8)\n\n");
			AliceLoader::skipDLLs = true;

			if (AliceLoader::ep2FPSTarget != 60.f)
			{
				printf("Setting target refresh rate to %.f\n", AliceLoader::ep2FPSTarget);
				WRITE_MEMORY(0x5F247C, float, AliceLoader::ep2FPSTarget);
			}
			break;
		}

		default:
		{ 
			printf("No known game detected!\n");
			AliceLoader::skipDLLs = true; break;
		}
	}
}


/// <summary>
/// Launches an external program defined in the config file. Intended for use with OSA413's AMBPatcher
/// </summary>
void AliceLoader::LaunchExternalPatcher()
{
	if (!patcherDir.empty())
	{
		PROCESS_INFORMATION pi = { 0 };
		STARTUPINFO si = { 0 };

		bool patchPrc = CreateProcess(TEXT(patcherDir.c_str()), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
		if (!patchPrc)
			std::cout << "Error creating patcher process. Error code: " << GetLastError() << std::endl << std::endl;

		//BringWindowToTop((HWND)pi.dwProcessId);     Uneccessary actually

		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else printf("External patcher not specified, skipping...\n\n");
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


// TODO: Check if a specific DLL has already been loaded
void LoadDll(const char* dll)
{
	auto hModule = LoadLibraryA(dll);

	if (hModule)
	{
		printf(" >> Load Successful!\n");

		// Call the loaded DLL's Init() and PostInit() functions
		auto* pProc_Init = (InitFunc_t*)GetProcAddress(hModule, "Init");
		if (pProc_Init)
			pProc_Init();

		auto* pProc_PostInit = (InitFunc_t*)GetProcAddress(hModule, "PostInit");
		if (pProc_PostInit)
			pProc_PostInit();

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

	if (!FileService::IsEmptyOrWhiteSpace(file))
	{
		printf("Loading module from mod: \"%s\"...\n", name != "" ? name.c_str() : file.c_str());
		LoadDll(moduleDir.c_str());
	}
	else
		printf("WARNING! Module file is undefined! Skipped \"%s\"\n", name != "" ? name.c_str() : "NO MOD NAME");
}

void AliceLoader::LoadExternalModule_Direct(std::string filePath)
{
	printf("Loading module @ \"%s\"...\n", filePath.c_str());
	LoadDll(filePath.c_str());
}

