#pragma once
class FileService
{

public:
	const static std::string GetModuleDir();

	static bool IsEmptyOrWhiteSpace(std::string s)
	{
		if (s.empty() || std::all_of(s.begin(), s.end(), [](char c) {return std::isspace(c); }))
			return true;
		else
			return false;
	}

	static bool IsEmptyOrWhiteSpace(const char* s)
	{
		if (s != NULL && s[0] == '\0')
			return true;
		else
			return false;
	}
};

