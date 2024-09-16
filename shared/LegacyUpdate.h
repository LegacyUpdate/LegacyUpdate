#pragma once

#include <windows.h>
#include "Registry.h"

static LPWSTR _installPath;

static inline HRESULT GetInstallPath(LPWSTR *path) {
	*path = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
	HRESULT hr = S_OK;

	if (!_installPath) {
		DWORD size = 0;
		hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LegacyUpdate", L"InstallLocation", KEY_WOW64_64KEY, &_installPath, &size);
		if (SUCCEEDED(hr)) {
			lstrcpy(*path, _installPath);
			return S_OK;
		}

		// Do our best to guess where it should be
		LPWSTR programFiles;
		hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion", L"ProgramFilesDir", KEY_WOW64_64KEY, &programFiles, &size);
		if (SUCCEEDED(hr)) {
			_installPath = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
			wsprintf(_installPath, L"%ls\\Legacy Update", programFiles);
			LocalFree(programFiles);
		}
	}

	lstrcpy(*path, _installPath);
	return hr;
}
