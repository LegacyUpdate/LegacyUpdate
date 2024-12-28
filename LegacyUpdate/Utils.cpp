#include "stdafx.h"
#include <comdef.h>
#include <atlstr.h>
#include "HResult.h"
#include "WMI.h"
#include "VersionInfo.h"

#pragma comment(lib, "version.lib")
#pragma comment(lib, "advapi32.lib")

typedef DWORD (WINAPI *_InitiateShutdownW)(LPWSTR lpMachineName, LPWSTR lpMessage, DWORD dwGracePeriod, DWORD dwShutdownFlags, DWORD dwReason);

static BOOL _loadedProductName = FALSE;
static CComVariant _productName;

HRESULT GetOSProductName(LPVARIANT productName) {
	if (!_loadedProductName) {
		_loadedProductName = TRUE;
		VariantInit(&_productName);

		if (IsWinXP2003()) {
			// Special case for Windows Storage Server 2003: Load the brand string from wssbrand.dll. On this edition, the
			// brand string from WMI remains "Microsoft Windows Server 2003".
			// TODO: This actually unconditionally shows the WSS name on all Server 2003. How does the OS decide it's WSS?
			// TODO: Add XP Media Center Edition, Tablet PC Edition, Compute Cluster 2003?
			HMODULE wssbrand = LoadLibrary(L"wssbrand.dll");
			if (wssbrand) {
				WCHAR brand[256];
				LoadString(wssbrand, 1102, brand, ARRAYSIZE(brand));
				_productName.vt = VT_BSTR;
				_productName.bstrVal = SysAllocString(brand);
				FreeLibrary(wssbrand);
			}
		}

		if (_productName.vt == VT_EMPTY) {
			// Get from WMI
			HRESULT hr = QueryWMIProperty(L"SELECT Caption FROM Win32_OperatingSystem", L"Caption", &_productName);
			if (!SUCCEEDED(hr)) {
				return hr;
			}
		}
	}

	VariantCopy(productName, &_productName);
	return S_OK;
}

HRESULT Reboot() {
	HRESULT hr = E_FAIL;

	// Make sure we have permission to shut down
	HANDLE token;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
		hr = AtlHresultFromLastError();
		TRACE("OpenProcessToken() failed: %ls\n", GetMessageForHresult(hr));
		goto end;
	}

	LUID shutdownLuid;
	if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &shutdownLuid)) {
		hr = AtlHresultFromLastError();
		TRACE("LookupPrivilegeValue() failed: %ls\n", GetMessageForHresult(hr));
		goto end;
	}

	// Ask the system nicely to give us shutdown privilege
	TOKEN_PRIVILEGES privileges;
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
