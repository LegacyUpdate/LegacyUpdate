#include <windows.h>
#include "GetPidForName.h"
#include "MsgBox.h"
#include "VersionInfo.h"

typedef DWORD (__fastcall *_ThemeWaitForServiceReady)(DWORD timeout);
typedef DWORD (__fastcall *_ThemeWatchForStart)();

static void StartThemes() {
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

	// Windows 7 moves this to UxInit.dll
	HMODULE shsvcs = LoadLibrary(L"UxInit.dll");
	if (!shsvcs) {
		shsvcs = LoadLibrary(L"shsvcs.dll");
		if (!shsvcs) {
			return;
		}
	}

	// Get functions by ordinals
	_ThemeWaitForServiceReady $ThemeWaitForServiceReady = (_ThemeWaitForServiceReady)GetProcAddress(shsvcs, MAKEINTRESOURCEA(2));
	_ThemeWatchForStart $ThemeWatchForStart = (_ThemeWatchForStart)GetProcAddress(shsvcs, MAKEINTRESOURCEA(1));

	// 1. Wait up to 1000ms for Themes to start
	if ($ThemeWaitForServiceReady) {
		$ThemeWaitForServiceReady(1000);
	}

	// 2. Prompt Themes to start a session for the SYSTEM desktop
	if ($ThemeWatchForStart) {
		$ThemeWatchForStart();
	}

	FreeLibrary(shsvcs);
}

void RunOnce() {
	StartThemes();

	// Construct path to LegacyUpdateSetup.exe
	WCHAR setupPath[MAX_PATH];
	GetModuleFileName(NULL, setupPath, ARRAYSIZE(setupPath));
	wcsrchr(setupPath, L'\\')[1] = L'\0';
	wcsncat(setupPath, L"LegacyUpdateSetup.exe", ARRAYSIZE(setupPath) - wcslen(setupPath) - 1);

	// Execute and wait for completion
	STARTUPINFO startupInfo = {0};
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo = {0};
	if (!CreateProcess(setupPath, L"/runonce", NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo)) {
#ifdef _DEBUG
		// Run cmd.exe instead
		if (!CreateProcess(L"C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo)) {
			PostQuitMessage(0);
			return;
		}
#endif

		MsgBox(NULL, L"Continuing Legacy Update setup failed", NULL, MB_OK | MB_ICONERROR);

#ifndef _DEBUG
		PostQuitMessage(0);
		return;
#endif
	}

	CloseHandle(processInfo.hThread);

	// Wait for it to finish
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(processInfo.hProcess);
	PostQuitMessage(0);
}
