#include "FileService.h"

#include <vector>
#include <fstream>
#include "Shlwapi.h"

const std::string FileService::GetModuleDir() // Un-optimal hack solution Xd
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

bool FileService::FileExists(const char* file)
{
	if (PathFileExists(file))
		return true;

	return false;
}