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

extern "C" IMAGE_DOS_HEADER __ImageBase;

#define OwnModule ((HMODULE)&__ImageBase)

static BOOL _loadedProductName = FALSE;
static CComVariant _productName;

static BOOL _loadedOwnVersion = FALSE;
static LPWSTR _version;
static UINT _versionSize;

void GetOwnFileName(LPWSTR *filename, LPDWORD size) {
	*filename = (LPWSTR)malloc(MAX_PATH);
	*size = GetModuleFileName(OwnModule, *filename, MAX_PATH);
}

HRESULT GetOwnVersion(LPWSTR *version, LPDWORD size) {
	if (!_loadedOwnVersion) {
		LPWSTR filename;
		DWORD filenameSize;
		GetOwnFileName(&filename, &filenameSize);

		DWORD verHandle;
		DWORD verInfoSize = GetFileVersionInfoSize(filename, &verHandle);
		if (verInfoSize == 0) {
			return AtlHresultFromLastError();
		}

		LPVOID verInfo = new BYTE[verInfoSize];
		if (!GetFileVersionInfo(filename, verHandle, verInfoSize, verInfo)) {
			return AtlHresultFromLastError();
		}

		if (!VerQueryValue(verInfo, L"\\StringFileInfo\\040904B0\\ProductVersion", (LPVOID *)&_version, &_versionSize)) {
			return AtlHresultFromLastError();
		}
	}

	*version = _version;
	*size = _versionSize;
	return _version == NULL ? E_FAIL : NOERROR;
}

HRESULT GetRegistryString(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, LPDWORD type, LPWSTR *data, LPDWORD size) {
	HKEY subkey;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, KEY_READ, &subkey));
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	if (data != NULL && size != NULL) {
		DWORD length = 512 * sizeof(WCHAR);
		LPWSTR buffer = (LPWSTR)malloc(length);
		LSTATUS status;
		do {
			status = RegQueryValueEx(subkey, valueName, NULL, type, (BYTE *)buffer, &length);
			if (status == ERROR_MORE_DATA) {
				length += 256 * sizeof(WCHAR);
				buffer = (LPWSTR)realloc(buffer, length);
			} else if (status != ERROR_SUCCESS) {
				hr = HRESULT_FROM_WIN32(status);
				goto end;
			}
		} while (status == ERROR_MORE_DATA);

		*data = buffer;
		*size = length / sizeof(WCHAR);
	}

end:
	if (subkey != NULL) {
		RegCloseKey(subkey);
	}
	if (!SUCCEEDED(hr)) {
		if (data != NULL) {
			*data = NULL;
		}
		if (size != NULL) {
			*size = 0;
		}
	}
	return hr;
}

HRESULT GetRegistryDword(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, LPDWORD type, LPDWORD data) {
	HKEY subkey;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, KEY_READ, &subkey));
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	if (data != NULL) {
		DWORD length = sizeof(DWORD);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(subkey, valueName, NULL, type, (LPBYTE)data, &length));
		if (!SUCCEEDED(hr)) {
			goto end;
		}
	}

end:
	if (subkey != NULL) {
		RegCloseKey(subkey);
	}
	return hr;
}

HRESULT GetOSProductName(VARIANT *pProductName) {
	if (_loadedProductName) {
		VariantCopy(pProductName, &_productName);
		return S_OK;
	}

	VariantInit(&_productName);
	_loadedProductName = true;
	return QueryWMIProperty(L"SELECT Caption FROM Win32_OperatingSystem", L"Caption", &_productName);
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
