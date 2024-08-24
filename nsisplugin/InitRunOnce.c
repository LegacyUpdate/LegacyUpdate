#include <windows.h>
#include "GetPidForName.h"
#include "HResult.h"
#include "VersionInfo.h"

typedef int (__fastcall *_ThemesOnCreateSessionWithPID)(unsigned int pid);

// This is only used by setup in RunOnce mode. This is here because it needs to be executed with the
// native architecture, and the setup process is always 32-bit. Most likely UxInit.dll uses structs
// that don't match between 32-bit and 64-bit.
// Function signature required by Rundll32.exe.
EXTERN_C __declspec(dllexport)
void __cdecl InitRunOnce(HWND hwnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow) {
	// Only run if we're Vista or later
	if (!IsOSVersionOrLater(6, 0)) {
		return;
	}

	// Only run if we're SYSTEM
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
