#include "D3D9Hook.h"
#include "AliceLoader.h"

void* (WINAPI* Direct3DCreate9Ptr)(UINT SDKVersion);
HRESULT* (WINAPI* Direct3DCreate9ExPtr)(UINT SDKVersion, void** d3d);

void HookD3D9()
{
#if _DEBUG
	// Wait for a debugger to attach. Thanks, Stack Overflow! https://stackoverflow.com/a/35018030
	while (AliceLoader::waitForDebugger && !::IsDebuggerPresent())
		::Sleep(100);
#endif 

	AliceLoader::TestFunc();
	AliceLoader::launchExternalPatcher();

	wchar_t windir[MAX_PATH];
	GetSystemDirectoryW(windir, MAX_PATH);
	wchar_t d3dpath[MAX_PATH];
	_snwprintf(d3dpath, MAX_PATH, L"%s\\d3d9.dll", windir);
	const HMODULE hmod = LoadLibraryW(d3dpath);
	Direct3DCreate9Ptr = (decltype(Direct3DCreate9Ptr))GetProcAddress(hmod, "Direct3DCreate9");
	Direct3DCreate9ExPtr = (decltype(Direct3DCreate9ExPtr))GetProcAddress(hmod, "Direct3DCreate9Ex");
}

void* WINAPI Direct3DCreate9(UINT SDKVersion)
{
#pragma EXPORT
	if (!Direct3DCreate9Ptr) HookD3D9();
	return Direct3DCreate9Ptr(SDKVersion);
}

HRESULT* WINAPI Direct3DCreate9Ex(UINT SDKVersion, void** d3d)
{
#pragma EXPORT
	if (!Direct3DCreate9ExPtr) HookD3D9();
	return Direct3DCreate9ExPtr(SDKVersion, d3d);
}