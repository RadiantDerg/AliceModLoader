#include "LoaderConfig.h"
#include "AliceLoader.h"

#include <vector>
#include <fstream>

std::string AMLConfig::configPath;

void AMLConfig::loadConfig()
{
	if (configPath.empty())
		AMLConfig::setConfigPath();

	const INIReader reader(configPath + "AliceML.ini");

	if (reader.ParseError() != 0)
	{
		printf("Could not load the configuration file.\n");
		return;
	}

	AliceLoader::enableConsole = reader.GetBoolean("Config", "EnableConsole", false);
}

void AMLConfig::saveConfig()
{
	// ?
}

void AMLConfig::setConfigPath()
{
	std::string mDir = AMLConfig::getModuleDir() + "\\AML\\";
	AMLConfig::configPath = mDir;
}

std::string AMLConfig::getModuleDir()
{
	std::vector<TCHAR> lpFilename(MAX_PATH);
	GetModuleFileName(0, &lpFilename[0], MAX_PATH);
	int check = GetCurrentDirectory(160, &lpFilename[0]);

	if (check == 0 || check == MAX_PATH)
	{
		printf("Error getting current directory\n");
		return NULL;
	}
	else
		return &lpFilename[0];
}
