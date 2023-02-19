#include "Config.h"
#include "AliceLoader.h"

#include <vector>
#include <fstream>

std::string Config::configPath;

void Config::LoadConfig()
{
	if (configPath.empty())
		Config::GetAliceFolder();

	printf("Loading AML Configuration\n");
	
	const INIReader reader(configPath + "AliceML.ini");

	if (reader.ParseError() != 0)
	{
		printf("Could not load AML configuration file!\n");
		return;
	}

	// Base Configuration
	AliceLoader::enableConsole   = reader.GetBoolean("Config", "EnableConsole", false);
	AliceLoader::waitForDebugger = reader.GetBoolean("Config", "WaitForDebugger", false);
	AliceLoader::patcherDir      = reader.Get("Config", "PatcherDir", "");
	AliceLoader::skipDLLs        = reader.GetBoolean("Config", "DisableCodeMods", false);

	// Episode 1
	AliceLoader::ep1Width        = reader.GetFloat("Episode1", "Width", 1280.f);
	AliceLoader::ep1Height       = reader.GetFloat("Episode1", "Height", 720.f);

	// Episode 2
	AliceLoader::ep2FPSTarget    = reader.GetFloat("Episode2", "TargetFPS", 60.f);

	// Temporary!
	AliceLoader::useList         = reader.GetBoolean("Config", "UserList", false);
}

void Config::GetAliceFolder()
{
	std::string mDir = FileService::GetModuleDir() + "\\AML\\";
	Config::configPath = mDir;
}
