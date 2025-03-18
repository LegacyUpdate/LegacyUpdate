#include "stdafx.h"
#include <comdef.h>
#include <atlstr.h>
#include <shlwapi.h>
#include "Exec.h"
#include "HResult.h"
#include "LegacyUpdate.h"
#include "VersionInfo.h"
#include "WMI.h"

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib")

typedef DWORD (WINAPI *_InitiateShutdownW)(LPWSTR lpMachineName, LPWSTR lpMessage, DWORD dwGracePeriod, DWORD dwShutdownFlags, DWORD dwReason);

static BOOL _loadedProductName = FALSE;
static CComVariant _productName;

typedef struct {
	DWORD version;
	DWORD osFlag;
	LPWSTR library;
	UINT stringIDs[3];
} WinNT5Variant;

static const WinNT5Variant nt5Variants[] = {
	// XP
	{0x0501, OS_TABLETPC,       L"winbrand.dll", { 180, 2000}},
	{0x0501, OS_MEDIACENTER,    L"winbrand.dll", { 180, 2001}},
	{0x0501, OS_STARTER,        L"winbrand.dll", { 180, 2002}},
	{0x0501, OS_EMBPOS,         L"winbrand.dll", { 180, 2003}},
	{0x0501, OS_WINFLP,         L"winbrand.dll", { 180, 2004}},
	{0x0501, OS_EMBSTD2009,     L"winbrand.dll", { 180, 2005}},
	{0x0501, OS_EMBPOS2009,     L"winbrand.dll", { 180, 2006}},
	// Check for XP Embedded last as WES2009 also identifies as OS_EMBEDDED.
	{0x0501, OS_EMBEDDED,       L"sysdm.cpl",    { 180, 189}},

	// Server 2003
	{0x0502, OS_APPLIANCE,      L"winbrand.dll", { 181, 2002}},
	{0x0502, OS_STORAGESERVER,  L"wssbrand.dll", {1101, 1102}},
	{0x0502, OS_COMPUTECLUSTER, L"hpcbrand.dll", {1101, 1102, 1103}},
	{0x0502, OS_HOMESERVER,     L"whsbrand.dll", {1101, 1102}},
};

HRESULT GetOSProductName(LPVARIANT productName) {
	if (!_loadedProductName) {
		_loadedProductName = TRUE;
		VariantInit(&_productName);

		// Handle the absolute disaster of Windows XP/Server 2003 edition branding
		WORD winver = GetWinVer();
		if (HIBYTE(winver) == 5) {
			WinNT5Variant variant = {};
			for (DWORD i = 0; i < ARRAYSIZE(nt5Variants); i++) {
				if (winver == nt5Variants[i].version && IsOS(nt5Variants[i].osFlag)) {
					variant = nt5Variants[i];
					break;
				}
			}

			if (variant.version) {
				HMODULE brandDll = LoadLibraryEx(variant.library, NULL, LOAD_LIBRARY_AS_DATAFILE);
				if (brandDll) {
					WCHAR brandStr[1024];
					ZeroMemory(brandStr, ARRAYSIZE(brandStr));

					DWORD j = 0;
					while (variant.stringIDs[j] != 0) {
						UINT id = variant.stringIDs[j];
						WCHAR str[340];
						if (id == 180 || id == 181) {
							// Get "Microsoft Windows XP" or "Microsoft Windows Server 2003" string
							HMODULE sysdm = LoadLibraryEx(L"sysdm.cpl", NULL, LOAD_LIBRARY_AS_DATAFILE);
							if (sysdm) {
								LoadString(sysdm, id, str, ARRAYSIZE(str));
								FreeLibrary(sysdm);
							}
						} else {
							LoadString(brandDll, id, str, ARRAYSIZE(str));
						}

						if (j > 0) {
							wcscat(brandStr, L" ");
						}

						wcscat(brandStr, str);
						j++;
					}

					_productName.vt = VT_BSTR;
					_productName.bstrVal = SysAllocString(brandStr);
					FreeLibrary(brandDll);
				}
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

HRESULT StartLauncher(LPWSTR params, BOOL wait) {
	LPWSTR path;
	HRESULT hr = GetInstallPath(&path);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	PathAppend(path, L"LegacyUpdate.exe");

	DWORD code;
	hr = Exec(L"open", path, params, NULL, SW_SHOW, wait, &code);
	if (SUCCEEDED(hr)) {
		hr = HRESULT_FROM_WIN32(code);
	}

	return hr;
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
