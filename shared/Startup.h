#pragma once

#include <windows.h>

typedef void (WINAPI *_SetDefaultDllDirectories)(DWORD DirectoryFlags);
typedef BOOL (WINAPI *_SetDllDirectoryW)(LPWSTR);

static inline void HardenDllSearchPaths(void) {
	// Try our best to secure DLL search paths to just system32
	WCHAR windir[MAX_PATH];
	WCHAR sysdir[MAX_PATH];

	GetWindowsDirectory(windir, ARRAYSIZE(windir));
	GetSystemDirectory(sysdir, ARRAYSIZE(sysdir));

	// Reset %windir% and %SystemRoot%
	SetEnvironmentVariable(L"WINDIR", windir);
	SetEnvironmentVariable(L"SystemRoot", windir);

	// Reset %PATH% to just system32
	WCHAR path[MAX_PATH];
	wsprintf(path, L"%ls;%ls", sysdir, windir);
	SetEnvironmentVariable(L"PATH", path);

	wcscat(sysdir, L"\\kernel32.dll");
	HMODULE kernel32 = GetModuleHandle(windir);
	if (!kernel32) {
		return;
	}

	// Windows Vista SP2+/7 SP1+ with KB2533623
	_SetDefaultDllDirectories $SetDefaultDllDirectories = (_SetDefaultDllDirectories)GetProcAddress(kernel32, "SetDefaultDllDirectories");
	if ($SetDefaultDllDirectories) {
		$SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32);
		return;
	}

	// Windows XP SP1+
	_SetDllDirectoryW $SetDllDirectoryW = (_SetDllDirectoryW)GetProcAddress(kernel32, "SetDllDirectoryW");
	if ($SetDllDirectoryW) {
		$SetDllDirectoryW(L"");
		return;
	}
}

static inline void Startup(void) {
	HardenDllSearchPaths();
}
