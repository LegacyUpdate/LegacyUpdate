#pragma once

#include <windows.h>

typedef void (WINAPI *_SetDefaultDllDirectories)(DWORD DirectoryFlags);
typedef BOOL (WINAPI *_SetDllDirectoryW)(LPWSTR);

static inline void HardenDllSearchPaths() {
	// Try our best to secure DLL search paths to just system32
	WCHAR path[MAX_PATH];

	// Reset %windir% and %SystemRoot%
	GetWindowsDirectory(path, ARRAYSIZE(path));
	SetEnvironmentVariable(L"WINDIR", path);
	SetEnvironmentVariable(L"SystemRoot", path);

	// Reset %PATH% to just system32
	ZeroMemory(path, sizeof(path));
	GetSystemDirectory(path, ARRAYSIZE(path));
	SetEnvironmentVariable(L"PATH", path);

	wcscat(path, L"\\kernel32.dll");
	HMODULE kernel32 = GetModuleHandle(path);
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

static inline void Startup() {
	HardenDllSearchPaths();

	// Init COM
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (!SUCCEEDED(hr)) {
		ExitProcess(hr);
		return;
	}

	// Init common controls
	INITCOMMONCONTROLSEX initComctl = {0};
	initComctl.dwSize = sizeof(initComctl);
	initComctl.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&initComctl);
}
