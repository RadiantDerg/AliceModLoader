#include "Config.h"
#include "AliceLoader.h"

#include <vector>
#include <fstream>

std::string Config::configPath;

void Config::loadConfig()
{
	if (configPath.empty())
		Config::setConfigPath();

	printf("Loading AML Configuration\n");

	const INIReader reader(configPath + "AliceML.ini");

	if (reader.ParseError() != 0)
	{
		printf("Could not load AML configuration file!\n");
		return;
	}

	AliceLoader::enableConsole   = reader.GetBoolean("Config", "EnableConsole", false);
	AliceLoader::waitForDebugger = reader.GetBoolean("Config", "WaitForDebugger", false);
	AliceLoader::patcherDir      = reader.Get("Config", "PatcherDir", "");
	AliceLoader::skipDLLs        = reader.GetBoolean("Config", "DisableCodeMods", false);
	AliceLoader::fpsTarget       = reader.GetFloat("Config", "TargetFPS", 60.f);
}

void Config::setConfigPath()
{
	std::string mDir = AML_FS::getModuleDir() + "\\AML\\";
	Config::configPath = mDir;
}
