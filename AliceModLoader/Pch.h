#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

#include <Windows.h>
#include <detours.h>

#include <cstdint>
#include <cstdio>

#include "Helpers.h"
#include "INIReader.h"
#include "D3D9Hook.h"
#include "AML_FS.h"
