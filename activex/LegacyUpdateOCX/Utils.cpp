#include "stdafx.h"
#include <comdef.h>

#pragma comment(lib, "version.lib")

extern "C" IMAGE_DOS_HEADER __ImageBase;

#define OwnModule ((HMODULE)&__ImageBase)

static BOOL _loadedVersionInfo = FALSE;
static OSVERSIONINFOEX _versionInfo;

static BOOL _loadedOwnVersion = FALSE;
static LPWSTR _version;
static UINT _versionSize;

OSVERSIONINFOEX *GetVersionInfo() {
	if (!_loadedVersionInfo) {
		_loadedVersionInfo = true;
		ZeroMemory(&_versionInfo, sizeof(OSVERSIONINFOEX));
		_versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((LPOSVERSIONINFO)&_versionInfo);
	}
	return &_versionInfo;
}

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
			return HRESULT_FROM_WIN32(GetLastError());
		}

		LPVOID verInfo = new BYTE[verInfoSize];
		if (!GetFileVersionInfo(filename, verHandle, verInfoSize, verInfo)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (!VerQueryValue(verInfo, L"\\StringFileInfo\\040904B0\\ProductVersion", (LPVOID *)&_version, &_versionSize)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}
	}

	*version = _version;
	*size = _versionSize;
	return _version == NULL ? E_FAIL : NOERROR;
}

HRESULT GetRegistryString(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, LPDWORD type, LPWSTR *data, LPDWORD size) {
	HKEY subkey;
	HRESULT result = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, KEY_READ, &subkey));
	if (!SUCCEEDED(result)) {
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
				result = HRESULT_FROM_WIN32(status);
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
	if (!SUCCEEDED(result)) {
		if (data != NULL) {
			*data = NULL;
		}
		if (size != NULL) {
			*size = 0;
		}
	}
	return result;
}

HRESULT GetRegistryDword(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, LPDWORD type, LPDWORD data) {
	HKEY subkey;
	HRESULT result = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, KEY_READ, &subkey));
	if (!SUCCEEDED(result)) {
		goto end;
	}

	if (data != NULL) {
		DWORD length = sizeof(DWORD);
		result = HRESULT_FROM_WIN32(RegQueryValueEx(subkey, valueName, NULL, type, (LPBYTE)data, &length));
		if (!SUCCEEDED(result)) {
			goto end;
		}
	}

end:
	if (subkey != NULL) {
		RegCloseKey(subkey);
	}
	return result;
}

LPWSTR GetMessageForHresult(HRESULT result) {
	_com_error *error = new _com_error(result);
	CString message = error->ErrorMessage();
	BSTR outMessage = message.AllocSysString();
	return outMessage;
}
