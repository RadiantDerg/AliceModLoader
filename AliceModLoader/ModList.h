#pragma once
class ModList
{

public:
	struct ModInfo
	{
		std::string Dll;
		std::string Name;
	};

	static void ReadList();
	static ModInfo GetModInfo();
	static ModInfo GetModInfo(std::string);
	
};

