#pragma once
class AliceLoader
{
	static std::string exePath;
public:
	// AML
	static bool enableConsole;
	static bool waitForDebugger;
	static bool isDebug;
	static bool skipDLLs;
	static bool useList;

	static std::string patcherDir;

	// Episode 1 Specific
	static float ep1Width;
	static float ep1Height;

	// Episode 2 Specific
	static float ep2FPSTarget;
	

	static void TestFunc();
	static void InitLoader();
	static void LaunchExternalPatcher();
	static void IdentifyApp();

	static void ReadList();
	static void ApplyPatches();
	static void LoadCodeMods();

	static void LoadExternalModule(std::string file, std::string relativePath = "", std::string name = "");
	static void LoadExternalModule_Direct(std::string filePath);
};

typedef void (InitFunc_t)();