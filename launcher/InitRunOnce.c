#include <windows.h>
#include "GetPidForName.h"
#include "HResult.h"
#include "VersionInfo.h"

typedef int (__fastcall *_ThemesOnCreateSessionWithPID)(unsigned int pid);

static void StartThemes() {
	// Only relevant to Vista or later
	if (!IsOSVersionOrLater(6, 0)) {
		return;
	}

	// Only relevant if we're SYSTEM
	DWORD usernameLen = 256;
	LPWSTR username = (LPWSTR)LocalAlloc(LPTR, usernameLen * sizeof(WCHAR));
	GetUserName(username, &usernameLen);

	if (lstrcmpi(username, L"SYSTEM") != 0) {
		GlobalFree(username);
		return;
	}

	GlobalFree(username);

	// Ask UxInit.dll to ask the Themes service to start a session for this desktop. Themes doesn't
	// automatically start a session for the SYSTEM desktop, so we need to ask it to. This matches
	// what msoobe.exe does, e.g., on first boot.
	int winlogonPid = GetPidForName(L"winlogon.exe");
	if (!winlogonPid) {
		return;
	}

	HMODULE uxInit = LoadLibrary(IsOSVersionEqual(6, 0) ? L"shsvcs.dll" : L"UxInit.dll");
	if (!uxInit) {
		return;
	}

	// Call function with ordinal 13
	_ThemesOnCreateSessionWithPID $ThemesOnCreateSessionWithPID = (_ThemesOnCreateSessionWithPID)GetProcAddress(uxInit, MAKEINTRESOURCEA(13));
	if ($ThemesOnCreateSessionWithPID) {
		$ThemesOnCreateSessionWithPID(winlogonPid);
	}

	FreeLibrary(uxInit);
}

void RunOnce() {
	StartThemes();

	// Construct path to LegacyUpdateSetup.exe
	WCHAR setupPath[MAX_PATH];
	GetModuleFileName(NULL, setupPath, ARRAYSIZE(setupPath));
	wcsrchr(setupPath, L'\\')[1] = L'\0';
	wcsncat(setupPath, L"LegacyUpdateSetup.exe", ARRAYSIZE(setupPath) - wcslen(setupPath) - 1);

	// Execute and wait for completion
	STARTUPINFOW startupInfo = {0};
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo = {0};
	if (!CreateProcess(setupPath, L"/runonce", NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo)) {
		return;
	}

	CloseHandle(processInfo.hThread);

	// Wait for it to finish, ensuring we still pump WM_PAINT messages in the meantime
	while (WaitForSingleObject(processInfo.hProcess, 100) == WAIT_TIMEOUT) {
		MSG msg;
		while (PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_REMOVE)) {
			DispatchMessage(&msg);
		}
	}

	CloseHandle(processInfo.hProcess);
}
