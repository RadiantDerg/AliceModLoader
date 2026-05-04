#include "AliceLoader.h"
#include "Config.h"
#include "HookContainer.h"
#include "ModList.h"


bool AliceLoader::enableConsole   = false;
bool AliceLoader::waitForDebugger = false;
bool AliceLoader::skipCodeModules = false;
bool AliceLoader::isDebug         = false;
bool AliceLoader::useModList      = false;

AliceLoader::EGame AliceLoader::detectedGame = EGame::unknown;
std::string AliceLoader::patcherDir;

float AliceLoader::ep1Width     = 1280.f;
float AliceLoader::ep1Height    = 720.f;

float AliceLoader::ep2FPSTarget = 60.f;



// Called in DllMain.cpp
void AliceLoader::InitLoader()
{
#if _DEBUG
	// Debug output for steps taken before the if() statement below
	AliceLoader::isDebug = true;
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif
	// Install hooks
	HookContainer::InstallHooks();

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
bool IsMemAccessValid(void* pAddress, size_t nSize)
{
	MEMORY_BASIC_INFORMATION mbi;

	if (!VirtualQuery(pAddress, &mbi, sizeof(mbi)))
		return false;

    if (mbi.Protect & (PAGE_EXECUTE_READ | PAGE_READWRITE | PAGE_READONLY))
		return true;

	return false;
}


/// <summary>
/// Scan the calling process memory to detect the game, thus set the loader mode
/// </summary>
void AliceLoader::IdentifyApp()
{
	char appName[] = { 0x41, 0x4D, 0x5F, 0x57, 0x49, 0x4E }; // AM_WIN
	int  address[] = { ASLR(0x575C94), 0x6F1464, 0x5F2474 }; // S4E1, S4E2, S4E2_beta8
	int itr = 0;

	// Test if we can access the given location first, then check if the location contains "AM_WIN"
	while (itr < sizeof(address))
	{
		if (IsMemAccessValid((char*)address[itr], sizeof(appName)) && !memcmp((char*)address[itr], appName, sizeof(appName)))
			break;
		itr++;
	}

	detectedGame = static_cast<EGame>(itr);

	switch (detectedGame)
	{
		case EGame::episode1: // Sonic 4 Episode I (Retail)
		{
			printf("Game: Episode I\n\n");

			printf("Setting internal resolution to %.fx%.f\n", AliceLoader::ep1Width, AliceLoader::ep1Height);
			WRITE_MEMORY(ASLR(0x575C9C), float, AliceLoader::ep1Height);
			WRITE_MEMORY(ASLR(0x575CA0), float, AliceLoader::ep1Width);
			break;
		}

		case EGame::episode2: // Sonic 4 Episode II (Retail)
		{
			printf("Game: Episode II\n\n");
			//WRITE_MEMORY(0x6B369C, uint8_t, 0xDC, 0x35, 0x6C, 0x14, 0x6F, 0x00) // Overwrite instruction that compares 0x742A38 and point to the target FPS
			//WRITE_MEMORY(0x6B369C, uint8_t, 0xDC, 0x35, 0xDC, 0x6A, 0x6F, 0x00) // 6F6ADC
			if (AliceLoader::ep2FPSTarget != 60.f)
			{
				printf("Setting target refresh rate to %.f\n", AliceLoader::ep2FPSTarget);
				WRITE_MEMORY(0x6F146C, float, AliceLoader::ep2FPSTarget);
				//WRITE_MEMORY(0x6F6ADC, float, AliceLoader::ep2FPSTarget);	// Another 60.0f, unknown use-case
				//WRITE_MEMORY(0x742A38, double, AliceLoader::ep2FPSTarget); // Sus double, possibly related?
			}
			break;
		}

		case EGame::episode2beta: // Sonic 4 Episode II (Beta8)
		{
			printf("Game: Episode II (Beta8)\n\n");
			AliceLoader::skipCodeModules = true;

			if (AliceLoader::ep2FPSTarget != 60.f)
			{
				printf("Setting target refresh rate to %.f\n", AliceLoader::ep2FPSTarget);
				WRITE_MEMORY(0x5F247C, float, AliceLoader::ep2FPSTarget);
			}
			break;
		}

        case EGame::unknown:
		default:
		{ 
			printf("No known game detected!\n");
			AliceLoader::skipCodeModules = true;
		        break;
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
		PROCESS_INFORMATION pi = {};
		STARTUPINFO si = {};
		if (!CreateProcess(TEXT(patcherDir.c_str()), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
			std::cout << "Error creating patcher process. Error code: " << GetLastError() << '\n' << '\n';

		//BringWindowToTop((HWND)pi.dwProcessId); //not needed

		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else printf("External patcher not specified, skipping...\n\n");
}


void AliceLoader::ReadList()
{
	printf("AliceLoader::ReadList() is not currently implemented!\n");
}


// Called in Hooks_D3D9.cpp @ void HookD3D9()
// We do this because the executable is decompressed at this state, allowing us to freely load
// extra DLL files and/or directly patch the process memory without causing issues with SteamStub.
void AliceLoader::LoadCodeMods()
{
	if (Config::configPath.empty())
		Config::GetAliceFolder();

	printf("\nReading mod list...\n");

	// Read the list ini
	const INIReader reader(Config::configPath + "mods.ini");
	if (reader.ParseError() != 0)
	{
		printf("Could not load mod list!\n");
		return;
	}

	int total = reader.GetInteger("LoadInfo", "Count", 0);

	// Check each mod for code modules
	for (int i = 1; i <= total; i++)
	{
		ModList::ModInfo info = {};

		std::string modFolder = reader.Get("LoadInfo", "Mod" + std::to_string(i), "");

		// Load the listing's mod.ini
		const INIReader mod_reader(Config::configPath + modFolder + "\\mod.ini");
		if (mod_reader.ParseError() != 0)
		{
			printf("Could not load mod.ini!");
			break;
		}

		info.CodeModule = mod_reader.Get("Mod", "Module", "");
		info.ModName = mod_reader.Get("Description", "Title", "");

		// Load the defined module
		if (!info.CodeModule.empty())
		    LoadExternalModule(info.CodeModule, modFolder, info.ModName);
	}

	printf("\n");
}


void AliceLoader::ApplyPatches()
{

}


// TODO: Add a check to skip cases where DLL has already been loaded
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
	}
	else 
		printf(" >> Failed to load the module!\n");
}


void AliceLoader::LoadExternalModule(std::string file, std::string relativePath, std::string modName)
{
	std::string moduleDir = Config::configPath + relativePath + "\\" + file;

	if (!FileService::IsEmptyOrWhiteSpace(file))
	{
		printf("Loading module from mod: \"%s\"...\n", !modName.empty() ? modName.c_str() : file.c_str());
		LoadDll(moduleDir.c_str());
	}
	else
		printf("WARNING! Module file is undefined! Skipped \"%s\"\n", !modName.empty() ? modName.c_str() : "MISSING MOD NAME");
}


void AliceLoader::LoadExternalModule_Direct(const std::string& filePath)
{
	printf("Loading module @ \"%s\"...\n", filePath.c_str());
	LoadDll(filePath.c_str());
}
