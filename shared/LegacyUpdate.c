#include "stdafx.h"
#include <windows.h>
#include "Registry.h"

#if _WIN64
	#define ProgramFilesEnv L"%ProgramFilesW6432%"
#else
	#define ProgramFilesEnv L"%ProgramFiles%"
#endif

static LPWSTR _installPath;

EXTERN_C HRESULT GetInstallPath(LPWSTR *path) {
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
		hr = S_OK;
		_installPath = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
		if (ExpandEnvironmentStrings(ProgramFilesEnv L"\\Legacy Update", _installPath, MAX_PATH) == 0) {
			hr = HRESULT_FROM_WIN32(GetLastError());
		}
	}

	lstrcpy(*path, _installPath);
	return hr;
}
