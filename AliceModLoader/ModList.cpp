#include "Pch.h"
#include "ModList.h"
#include "Config.h"


void ModList::ReadList()
{
	
}

ModList::ModInfo GetModInfo()
{
	ModList::ModInfo info;
	info.Dll, info.Name = "";
	return info;
}

ModList::ModInfo GetModInfo(std::string modini)
{
	ModList::ModInfo info;

	if (Config::configPath.empty())
		Config::GetAliceFolder();

	printf("Reading mod info...\n");

	const INIReader reader(Config::configPath + modini);

	if (reader.ParseError() != 0)
	{
		printf("Could not load mod.ini!");
		info.Dll, info.Name = "";

		return info;
	}

	info.Dll = reader.Get("Mod", "Module", "");
	info.Name = reader.Get("Description", "Title", "");

	return info;
}