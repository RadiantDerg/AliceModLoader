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
		printf("Could not load the configuration file.\n");
		return;
	}

	AliceLoader::enableConsole = reader.GetBoolean("Config", "EnableConsole", false);
}

void Config::setConfigPath()
{
	std::string mDir = AML_FS::getModuleDir() + "\\AML\\";
	Config::configPath = mDir;
}
