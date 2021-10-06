#pragma once
class AMLConfig
{

public:
	static std::string configPath;

	static std::string getModuleDir();
	static void setConfigPath();
	static void loadConfig();
	static void saveConfig();
};

