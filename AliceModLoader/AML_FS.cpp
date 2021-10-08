#include "AML_FS.h"

#include <vector>
#include <fstream>

const std::string AML_FS::getModuleDir() // Un-optimal hack solution Xd
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
