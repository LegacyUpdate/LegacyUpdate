#pragma once

#include <windows.h>

typedef void (WINAPI *_SetDefaultDllDirectories)(DWORD DirectoryFlags);
typedef BOOL (WINAPI *_SetDllDirectoryW)(LPWSTR);

static inline void HardenDllSearchPaths() {
	// Try our best to secure DLL search paths to just system32
	WCHAR path[MAX_PATH];
	ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\kernel32.dll", path, ARRAYSIZE(path));
	HMODULE kernel32 = GetModuleHandle(path);

	// Reset %PATH% to just system32
	ExpandEnvironmentStrings(L"%SystemRoot%\\System32", path, ARRAYSIZE(path));
	SetEnvironmentVariable(L"PATH", path);

	// Windows Vista SP2+/7 SP1+ with KB2533623
	if (!kernel32) {
		return;
	}

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

static inline void Startup() {
	HardenDllSearchPaths();
}
