#include "Utils.h"
#include <shlwapi.h>
#include <winreg.h>
#include "Exec.h"
#include "LegacyUpdate.h"
#include "WULog.h"

#ifndef SHUTDOWN_RESTART
#define SHUTDOWN_RESTART 0x00000004
#endif

#ifndef SHUTDOWN_INSTALL_UPDATES
#define SHUTDOWN_INSTALL_UPDATES 0x00000040
#endif

typedef DWORD (WINAPI *_InitiateShutdownW)(LPWSTR lpMachineName, LPWSTR lpMessage, DWORD dwGracePeriod, DWORD dwShutdownFlags, DWORD dwReason);

HRESULT StartLauncher(LPCWSTR params, BOOL wait) {
	LPWSTR path = NULL;
	HRESULT hr = GetInstallPath(&path);
	CHECK_HR_OR_RETURN(L"GetInstallPath");

	PathAppend(path, L"LegacyUpdate.exe");

	DWORD code = 0;
	hr = Exec(NULL, path, params, NULL, SW_SHOWDEFAULT, wait, &code);
	if (SUCCEEDED(hr)) {
		hr = HRESULT_FROM_WIN32(code);
	}

	LocalFree(path);
	return hr;
}

HRESULT Reboot() {
	HRESULT hr = E_FAIL;

	HANDLE token = NULL;
	TOKEN_PRIVILEGES privileges;
	LUID shutdownLuid;
	_InitiateShutdownW $InitiateShutdownW = (_InitiateShutdownW)GetProcAddress(GetModuleHandle(L"advapi32.dll"), "InitiateShutdownW");

	// Make sure we have permission to shut down
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		CHECK_HR_OR_GOTO_END(L"OpenProcessToken");
	}

	if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &shutdownLuid)) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		CHECK_HR_OR_GOTO_END(L"LookupPrivilegeValue");
	}

	// Ask the system nicely to give us shutdown privilege
	privileges.PrivilegeCount = 1;
	privileges.Privileges[0].Luid = shutdownLuid;
	privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(token, FALSE, &privileges, 0, NULL, NULL)) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		CHECK_HR_OR_GOTO_END(L"AdjustTokenPrivileges");
	}

	// Reboot with reason "Operating System: Security fix (Unplanned)", ensuring to install updates.
	// Try InitiateShutdown first (Vista+)
	if ($InitiateShutdownW) {
		hr = HRESULT_FROM_WIN32($InitiateShutdownW(NULL, NULL, 0, SHUTDOWN_RESTART | SHUTDOWN_INSTALL_UPDATES, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX));
	}

	// Try InitiateSystemShutdownEx (2k/XP)
	if (!SUCCEEDED(hr)) {
		TRACE(L"InitiateShutdown failed: %08x", hr);

		if (InitiateSystemShutdownEx(NULL, NULL, 0, FALSE, TRUE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX) == 0) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			CHECK_HR_OR_GOTO_END(L"InitiateSystemShutdownEx");
		}
	}

	// Last-ditch attempt ExitWindowsEx (only guaranteed to work for the current logged in user)
		if (!SUCCEEDED(hr) && !ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX)) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		CHECK_HR_OR_GOTO_END(L"ExitWindowsEx");
	}

end:
	if (token) {
		CloseHandle(token);
	}

	return hr;
}

void operator delete(void *ptr) {
	free(ptr);
}
