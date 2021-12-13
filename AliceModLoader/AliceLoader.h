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
	static void initLoader();
	static void launchExternalPatcher();
	static void identifyApp();
};

typedef void (InitFunc_t)();