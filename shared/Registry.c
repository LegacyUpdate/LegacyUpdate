#include "stdafx.h"
#include "Registry.h"
#include "VersionInfo.h"

static ALWAYS_INLINE REGSAM GetWow64Flag(REGSAM options) {
#ifdef _WIN64
	return options;
#else
	if (AtLeastWinXP2002()) {
		return options;
	}

	// Filter out WOW64 keys, which are not supported on Windows 2000
	return options & ~(KEY_WOW64_64KEY | KEY_WOW64_32KEY);
#endif
}

HRESULT GetRegistryString(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, REGSAM options, LPWSTR *data, LPDWORD size) {
	HKEY subkey = NULL;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, GetWow64Flag(KEY_READ | options), &subkey));
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	if (data) {
		DWORD length = 512 * sizeof(WCHAR);
		LPWSTR buffer = (LPWSTR)LocalAlloc(LPTR, length);
		if (!buffer) {
			hr = E_OUTOFMEMORY;
			goto end;
		}

		LSTATUS status = 0;
		do {
			status = RegQueryValueEx(subkey, valueName, NULL, NULL, (BYTE *)buffer, &length);
			if (status == ERROR_MORE_DATA) {
				length += 256 * sizeof(WCHAR);
				LPWSTR newBuffer = (LPWSTR)LocalReAlloc(buffer, length, LMEM_MOVEABLE);
				if (!newBuffer) {
					LocalFree(buffer);
					hr = E_OUTOFMEMORY;
					goto end;
				}

				buffer = newBuffer;
			} else if (status != ERROR_SUCCESS) {
				hr = HRESULT_FROM_WIN32(status);
				LocalFree(buffer);
				goto end;
			}
		} while (status == ERROR_MORE_DATA);

		*data = buffer;

		if (size) {
			*size = length / sizeof(WCHAR);
		}
	}

end:
	if (subkey) {
		RegCloseKey(subkey);
	}
	if (!SUCCEEDED(hr)) {
		if (data) {
			*data = NULL;
		}
		if (size) {
			*size = 0;
		}
	}
	return hr;
}

HRESULT GetRegistryDword(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, REGSAM options, LPDWORD data) {
	HKEY subkey = NULL;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, GetWow64Flag(KEY_READ | options), &subkey));
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	if (data) {
		DWORD length = sizeof(DWORD);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(subkey, valueName, NULL, NULL, (LPBYTE)data, &length));
	}

end:
	if (subkey) {
		RegCloseKey(subkey);
	}
	return hr;
}
