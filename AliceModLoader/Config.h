#pragma once
class Config
{

public:
	static std::string configPath;

	static void setConfigPath();
	static void loadConfig(); // No Saving. Config will be externally edited.
};

