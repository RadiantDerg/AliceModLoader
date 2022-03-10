#pragma once
class AliceLoader
{
	static std::string exePath;
public:
	static bool enableConsole;
	static bool waitForDebugger;
	static bool isDebug;
	static bool skipDLLs;
	static float fpsTarget;

	static std::string patcherDir;

	static void TestFunc();
	static void InitLoader();
	static void LaunchExternalPatcher();
	static void IdentifyApp();

	static void LoadExternalModule(std::string file, std::string relativePath = "", std::string name = "");
	static void LoadExternalModule_Direct(std::string filePath);
};

typedef void (InitFunc_t)();