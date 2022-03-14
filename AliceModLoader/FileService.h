#pragma once
class FileService
{

public:
	const static std::string GetModuleDir();

	static bool ContainsWhiteSpace(std::string s)
	{
		return std::all_of(s.begin(), s.end(), isspace);
	}
};

