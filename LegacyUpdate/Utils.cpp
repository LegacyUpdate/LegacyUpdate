#include "stdafx.h"
#include <comdef.h>
#include <atlstr.h>
#include "HResult.h"
#include "WMI.h"

#pragma comment(lib, "version.lib")
#pragma comment(lib, "advapi32.lib")

// Defined as being Vista+, older versions ignore the flag.
#ifndef EWX_RESTARTAPPS
#define EWX_RESTARTAPPS 0x00000040
#endif

static BOOL _loadedProductName = FALSE;
static CComVariant _productName;

HRESULT GetOSProductName(LPVARIANT productName) {
	if (!_loadedProductName) {
		_loadedProductName = TRUE;
		VariantInit(&_productName);

		if (IsWinXP2003()) {
			// If this is Windows Storage Server 2003, load the brand string from wssbrand.dll
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
				hr = AtlHresultFromLastError();
				TRACE("AdjustTokenPrivileges() failed: %ls\n", GetMessageForHresult(hr));
			}
		}
	} else {
		hr = AtlHresultFromLastError();
		TRACE("OpenProcessToken() failed: %ls\n", GetMessageForHresult(hr));
	}

	// Reboot with reason "Operating System: Security fix (Unplanned)"
	if (!InitiateSystemShutdownEx(NULL, NULL, 0, FALSE, TRUE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX)) {
		hr = AtlHresultFromLastError();
		TRACE("InitiateSystemShutdownExW() failed: %ls\n", GetMessageForHresult(hr));

		// Try ExitWindowsEx instead
		// Win2k: Use ExitWindowsEx, which is only guaranteed to work for the current logged in user
		if (!ExitWindowsEx(EWX_REBOOT | EWX_RESTARTAPPS | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX)) {
			hr = AtlHresultFromLastError();
			TRACE("ExitWindowsEx() failed: %ls\n", GetMessageForHresult(hr));
		}
	} else {
		hr = S_OK;
	}

	return hr;
}
