#include "stdafx.h"
#include "Registry.h"
#include <malloc.h>

HRESULT GetRegistryString(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, DWORD options, LPWSTR *data, LPDWORD size) {
	HKEY subkey;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, KEY_READ | options, &subkey));
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	if (data != NULL) {
		DWORD length = 512 * sizeof(WCHAR);
		LPWSTR buffer = (LPWSTR)LocalAlloc(LPTR, length);
		LSTATUS status;
		do {
			status = RegQueryValueEx(subkey, valueName, NULL, NULL, (BYTE *)buffer, &length);
			if (status == ERROR_MORE_DATA) {
				length += 256 * sizeof(WCHAR);
				buffer = (LPWSTR)LocalReAlloc(buffer, length, LMEM_MOVEABLE);
			} else if (status != ERROR_SUCCESS) {
				hr = HRESULT_FROM_WIN32(status);
				goto end;
			}
		} while (status == ERROR_MORE_DATA);

		*data = buffer;

		if (size != NULL) {
			*size = length / sizeof(WCHAR);
		}
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

HRESULT GetRegistryDword(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, DWORD options, LPDWORD data) {
	HKEY subkey;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, KEY_READ | options, &subkey));
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	if (data != NULL) {
		DWORD length = sizeof(DWORD);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(subkey, valueName, NULL, NULL, (LPBYTE)data, &length));
	}

end:
	if (subkey != NULL) {
		RegCloseKey(subkey);
	}
	return hr;
}
