#pragma once
class Config
{

public:
	static std::string configPath;

	static void GetConfigPath();
	static void LoadConfig(); // No Saving. Config will be externally edited.
};

