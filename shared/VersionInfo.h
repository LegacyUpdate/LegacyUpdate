#pragma once

#include <windows.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

static BOOL _loadedVersionInfo = FALSE;
static OSVERSIONINFOEX _versionInfo;

static inline OSVERSIONINFOEX *GetVersionInfo() {
	if (!_loadedVersionInfo) {
		_loadedVersionInfo = TRUE;
		ZeroMemory(&_versionInfo, sizeof(OSVERSIONINFOEX));
		_versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((LPOSVERSIONINFO)&_versionInfo);
	}
	return &_versionInfo;
}

static inline BOOL IsOSVersionEqual(DWORD major, DWORD minor) {
	OSVERSIONINFOEX *versionInfo = GetVersionInfo();
	return versionInfo->dwMajorVersion == major && versionInfo->dwMinorVersion == minor;
}

static inline BOOL IsOSVersionOrLater(DWORD major, DWORD minor) {
	OSVERSIONINFOEX *versionInfo = GetVersionInfo();
	return versionInfo->dwMajorVersion > major || (versionInfo->dwMajorVersion == major && versionInfo->dwMinorVersion >= minor);
}

static inline BOOL IsOSVersionOrEarlier(DWORD major, DWORD minor) {
	OSVERSIONINFOEX *versionInfo = GetVersionInfo();
	return versionInfo->dwMajorVersion < major || (versionInfo->dwMajorVersion == major && versionInfo->dwMinorVersion <= minor);
}

EXTERN_C HRESULT GetOwnVersion(LPWSTR *version);

static inline void GetOwnFileName(LPWSTR *filename) {
	*filename = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
	GetModuleFileName((HMODULE)&__ImageBase, *filename, MAX_PATH);
}
