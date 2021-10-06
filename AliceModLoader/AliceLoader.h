#pragma once

class AliceLoader
{
	static std::string exePath;
public:
	static bool enableConsole;

	static void TestFunc();
	static void initLoader();
};

typedef void (InitFunc_t)();