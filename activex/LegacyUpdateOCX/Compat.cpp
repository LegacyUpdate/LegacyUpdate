#include "stdafx.h"
#include <comdef.h>
#include "Utils.h"

#pragma comment(lib, "advapi32.lib")

typedef BOOL (WINAPI *_InitiateSystemShutdownExW)(LPSTR, LPSTR, DWORD, BOOL, BOOL, DWORD);
typedef BOOL (WINAPI *_GetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);

// TODO: Latest MSDN docs say XP+, but VS2010 docs say 2k+. Who's correct?
_InitiateSystemShutdownExW $InitiateSystemShutdownExW = (_InitiateSystemShutdownExW)GetProcAddress(GetModuleHandle(L"advapi32.dll"), "InitiateSystemShutdownExW");

// Vista+
_GetProductInfo $GetProductInfo = (_GetProductInfo)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetProductInfo");

// Defined as being Vista+, older versions ignore the flag.
#ifndef EWX_RESTARTAPPS
#define EWX_RESTARTAPPS 0x00000040
#endif

void Reboot() {
	// Make sure we have permission to shut down.
	HANDLE token;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
		LUID shutdownLuid;
		if (LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &shutdownLuid) != 0) {
			// Ask the system nicely to give us shutdown privilege.
			TOKEN_PRIVILEGES privileges;
			privileges.PrivilegeCount = 1;
			privileges.Privileges[0].Luid = shutdownLuid;
			privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (!AdjustTokenPrivileges(token, FALSE, &privileges, 0, NULL, NULL)) {
				HRESULT result = HRESULT_FROM_WIN32(GetLastError());
				TRACE("AdjustTokenPrivileges() failed: %ls\n", GetMessageForHresult(result));
			}
		}
	} else {
		HRESULT result = HRESULT_FROM_WIN32(GetLastError());
		TRACE("OpenProcessToken() failed: %ls\n", GetMessageForHresult(result));
	}

	if ($InitiateSystemShutdownExW) {
		// XP+: Reboot with reason "Operating System: Security fix (Unplanned)"
		if (!$InitiateSystemShutdownExW(NULL, NULL, 0, FALSE, TRUE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX)) {
			HRESULT result = HRESULT_FROM_WIN32(GetLastError());
			TRACE("InitiateSystemShutdownExW() failed: %ls\n", GetMessageForHresult(result));
		}
	} else {
		// Win2k: Use ExitWindowsEx, which is only guaranteed to work for the current logged in user
		if (!ExitWindowsEx(EWX_REBOOT | EWX_RESTARTAPPS | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX)) {
			HRESULT result = HRESULT_FROM_WIN32(GetLastError());
			TRACE("ExitWindowsEx() failed: %ls\n", GetMessageForHresult(result));
		}
	}
}

BOOL GetVistaProductInfo(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType) {
	if ($GetProductInfo) {
		return $GetProductInfo(dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
	} else {
		*pdwReturnedProductType = PRODUCT_UNDEFINED;
		return FALSE;
	}
}