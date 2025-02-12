#include "stdafx.h"
#include "Registry.h"
#include "VersionInfo.h"
#include <shlwapi.h>

LPVOID DELETE_KEY = (LPVOID)INT_MIN;
LPVOID DELETE_VALUE = (LPVOID)(INT_MIN + 1);

static ALWAYS_INLINE REGSAM GetWow64Flag(REGSAM options) {
#ifndef _WIN64
	if (!AtLeastWinXP2002()) {
		// Filter out WOW64 keys, which are not supported on Windows 2000
		return options & ~(KEY_WOW64_64KEY | KEY_WOW64_32KEY);
	}
#endif

	return options;
}

HRESULT GetRegistryString(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, REGSAM options, LPWSTR *data, LPDWORD size) {
	HKEY subkey = NULL;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, GetWow64Flag(KEY_READ | options), &subkey));
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	if (data) {
		DWORD length = 0;
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(subkey, valueName, NULL, NULL, NULL, &length));
		if (!SUCCEEDED(hr)) {
			goto end;
		}

		// Allocate space for data plus a trailing null
		LPWSTR buffer = (LPWSTR)LocalAlloc(LPTR, length + sizeof(WCHAR));
		if (!buffer) {
			hr = E_OUTOFMEMORY;
			goto end;
		}

		hr = HRESULT_FROM_WIN32(RegQueryValueEx(subkey, valueName, NULL, NULL, (BYTE *)buffer, &length));
		if (!SUCCEEDED(hr)) {
			LocalFree(buffer);
			goto end;
		}

		buffer[length / sizeof(WCHAR)] = L'\0';
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

HRESULT SetRegistryEntries(RegistryEntry entries[], BOOL expandEnv) {
	for (size_t i = 0; entries[i].hKey != NULL; i++) {
		RegistryEntry entry = entries[i];
		LPWSTR expandedSubKey = NULL, expandedData = NULL;

		if (expandEnv && entry.lpSubKey) {
			DWORD length = ExpandEnvironmentStrings((LPCWSTR)entry.lpSubKey, NULL, 0);
			expandedSubKey = (LPWSTR)LocalAlloc(LPTR, length * sizeof(WCHAR));
			ExpandEnvironmentStrings((LPCWSTR)entry.lpSubKey, expandedSubKey, length);
			entry.lpSubKey = expandedSubKey;
		}

		HRESULT hr;
		if (entry.lpData == DELETE_KEY) {
			hr = HRESULT_FROM_WIN32(SHDeleteKey(entry.hKey, entry.lpSubKey));
			if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
				hr = S_OK;
			}
			if (!SUCCEEDED(hr)) {
				TRACE(L"delete key %ls fail %08x", entry.lpSubKey, hr);
				if (expandedSubKey) {
					LocalFree(expandedSubKey);
				}
			}
		} else {
			HKEY key;
			hr = HRESULT_FROM_WIN32(RegCreateKeyEx(entry.hKey, entry.lpSubKey, 0, NULL, 0, GetWow64Flag(KEY_WRITE | entry.samDesired), NULL, &key, NULL));
			if (!SUCCEEDED(hr)) {
				if (expandedSubKey) {
					LocalFree(expandedSubKey);
				}
				return hr;
			}

			if (entry.lpData == DELETE_VALUE) {
				hr = HRESULT_FROM_WIN32(RegDeleteValue(key, entry.lpValueName));
				if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
					hr = S_OK;
				}
				TRACE(L"delete val fail %08x", hr);
			} else {
				if (entry.dwType == REG_SZ) {
					if (expandEnv) {
						DWORD length = ExpandEnvironmentStrings((LPCWSTR)entry.lpData, NULL, 0);
						expandedData = (LPWSTR)LocalAlloc(LPTR, length * sizeof(WCHAR));
						ExpandEnvironmentStrings((LPCWSTR)entry.lpData, expandedData, length);
						entry.lpData = expandedData;
					}

					entry.dataSize = (DWORD)(lstrlen((LPCWSTR)entry.lpData) + 1) * sizeof(WCHAR);
				} else {
					entry.dataSize = sizeof(entry.lpData);
					entry.lpData = (LPVOID)&entry.lpData;
				}

				hr = HRESULT_FROM_WIN32(RegSetValueEx(key, entry.lpValueName, 0, entry.dwType, (const BYTE *)entry.lpData, entry.dataSize));
			}

			if (expandedSubKey) {
				LocalFree(expandedSubKey);
			}
			if (expandedData) {
				LocalFree(expandedData);
			}

			RegCloseKey(key);
		}

		if (!SUCCEEDED(hr)) {
			return hr;
		}
	}

	return S_OK;
}
