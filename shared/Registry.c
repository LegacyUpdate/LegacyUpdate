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
	if (data == NULL) {
		return E_POINTER;
	}

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

		DWORD ourSize = length / sizeof(WCHAR);
		buffer[ourSize] = L'\0';
		*data = buffer;

		if (size) {
			*size = ourSize;
		}
	}

end:
	if (subkey) {
		RegCloseKey(subkey);
	}
	if (!SUCCEEDED(hr)) {
		*data = NULL;
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

HRESULT SetRegistryEntries(RegistryEntry entries[]) {
	for (DWORD i = 0; entries[i].hKey != NULL; i++) {
		RegistryEntry entry = entries[i];
		LPWSTR expandedSubKey = NULL, expandedData = NULL;

		if (entry.lpSubKey) {
			DWORD length = ExpandEnvironmentStrings((LPCWSTR)entry.lpSubKey, NULL, 0);
			expandedSubKey = (LPWSTR)LocalAlloc(LPTR, length * sizeof(WCHAR));
			ExpandEnvironmentStrings((LPCWSTR)entry.lpSubKey, expandedSubKey, length);
			entry.lpSubKey = expandedSubKey;
		}

		HRESULT hr;
		if (entry.uData.lpData == DELETE_KEY) {
#if WINVER < _WIN32_WINNT_WIN2K
			hr = E_NOTIMPL;
#else
			hr = HRESULT_FROM_WIN32(SHDeleteKey(entry.hKey, entry.lpSubKey));
#endif
			if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
				hr = S_OK;
			}
			if (!SUCCEEDED(hr)) {
				TRACE(L"Delete %ls failed: %08x", entry.lpSubKey, hr);
				if (expandedSubKey) {
					LocalFree(expandedSubKey);
				}
			}
		} else {
			HKEY key;
			hr = HRESULT_FROM_WIN32(RegCreateKeyEx(entry.hKey, entry.lpSubKey, 0, NULL, 0, GetWow64Flag(KEY_WRITE | entry.samDesired), NULL, &key, NULL));
			if (!SUCCEEDED(hr)) {
				TRACE(L"Create %ls failed: %08x", entry.lpSubKey, hr);
				if (expandedSubKey) {
					LocalFree(expandedSubKey);
				}
				return hr;
			}

			if (entry.uData.lpData == DELETE_VALUE) {
				hr = HRESULT_FROM_WIN32(RegDeleteValue(key, entry.lpValueName));
				if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
					hr = S_OK;
				}
			} else if (entry.uData.lpData != NULL) {
				if (entry.dwType == REG_SZ) {
					if (entry.uData.lpData) {
						DWORD length = ExpandEnvironmentStrings((LPCWSTR)entry.uData.lpData, NULL, 0);
						expandedData = (LPWSTR)LocalAlloc(LPTR, length * sizeof(WCHAR));
						ExpandEnvironmentStrings((LPCWSTR)entry.uData.lpData, expandedData, length);
						entry.uData.lpData = expandedData;

						entry.dataSize = (DWORD)(lstrlen((LPCWSTR)entry.uData.lpData) + 1) * sizeof(WCHAR);
					} else {
						entry.dataSize = 0;
					}

					hr = HRESULT_FROM_WIN32(RegSetValueEx(key, entry.lpValueName, 0, entry.dwType, (const BYTE *)entry.uData.lpData, entry.dataSize));
				} else if (entry.dwType == REG_DWORD) {
					hr = HRESULT_FROM_WIN32(RegSetValueEx(key, entry.lpValueName, 0, entry.dwType, (const BYTE *)&entry.uData.dwData, sizeof(entry.uData.dwData)));
				}
			}

			if (!SUCCEEDED(hr)) {
				TRACE(L"Set %ls -> %ls failed: %08x", entry.lpSubKey, entry.lpValueName, hr);
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
