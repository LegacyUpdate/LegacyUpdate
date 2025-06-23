#include "stdafx.h"
#include <comdef.h>
#include <atlstr.h>
#include "Exec.h"
#include "HResult.h"
#include "LegacyUpdate.h"
#include "VersionInfo.h"
#include "Wow64.h"

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib")

typedef DWORD (WINAPI *_InitiateShutdownW)(LPWSTR lpMachineName, LPWSTR lpMessage, DWORD dwGracePeriod, DWORD dwShutdownFlags, DWORD dwReason);

HRESULT StartLauncher(LPWSTR params, BOOL wait) {
	LPWSTR path = NULL;
	HRESULT hr = GetInstallPath(&path);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	PathAppend(path, L"LegacyUpdate.exe");

	DWORD code = 0;
	hr = Exec(NULL, path, params, NULL, SW_SHOWDEFAULT, wait, &code);
	if (SUCCEEDED(hr)) {
		hr = HRESULT_FROM_WIN32(code);
	}

	return hr;
}

HRESULT Reboot() {
	HRESULT hr = E_FAIL;

	HANDLE token = NULL;
	TOKEN_PRIVILEGES privileges = {0};
	LUID shutdownLuid = {0};

	// Make sure we have permission to shut down
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
		hr = AtlHresultFromLastError();
		TRACE("OpenProcessToken() failed: %ls\n", GetMessageForHresult(hr));
		goto end;
	}

	if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &shutdownLuid)) {
		hr = AtlHresultFromLastError();
		TRACE("LookupPrivilegeValue() failed: %ls\n", GetMessageForHresult(hr));
		goto end;
	}

	// Ask the system nicely to give us shutdown privilege
	privileges.PrivilegeCount = 1;
	privileges.Privileges[0].Luid = shutdownLuid;
	privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(token, FALSE, &privileges, 0, NULL, NULL)) {
		hr = AtlHresultFromLastError();
		TRACE("AdjustTokenPrivileges() failed: %ls\n", GetMessageForHresult(hr));
		goto end;
	}

	// Reboot with reason "Operating System: Security fix (Unplanned)", ensuring to install updates.
	// Try InitiateShutdown first (Vista+)
	_InitiateShutdownW $InitiateShutdownW = (_InitiateShutdownW)GetProcAddress(GetModuleHandle(L"advapi32.dll"), "InitiateShutdownW");
	if ($InitiateShutdownW) {
		hr = HRESULT_FROM_WIN32($InitiateShutdownW(NULL, NULL, 0, SHUTDOWN_RESTART | SHUTDOWN_INSTALL_UPDATES, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX));
	}

	// Try InitiateSystemShutdownEx (2k/XP)
	if (!SUCCEEDED(hr)) {
		TRACE("InitiateShutdown() failed: %ls\n", GetMessageForHresult(hr));

		if (InitiateSystemShutdownEx(NULL, NULL, 0, FALSE, TRUE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX) == 0) {
			hr = AtlHresultFromLastError();
			TRACE("InitiateSystemShutdownExW() failed: %ls\n", GetMessageForHresult(hr));
		}
	}

	// Last-ditch attempt ExitWindowsEx (only guaranteed to work for the current logged in user)
	if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX)) {
		hr = AtlHresultFromLastError();
		TRACE("ExitWindowsEx() failed: %ls\n", GetMessageForHresult(hr));
	}

end:
	if (token) {
		CloseHandle(token);
	}

	return hr;
}
