#include "stdafx.h"
#include "VersionInfo.h"

static BOOL _loadedVersionInfo = FALSE;
static OSVERSIONINFOEX _versionInfo;

OSVERSIONINFOEX *GetVersionInfo() {
	if (!_loadedVersionInfo) {
		_loadedVersionInfo = true;
		ZeroMemory(&_versionInfo, sizeof(OSVERSIONINFOEX));
		_versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((LPOSVERSIONINFO)&_versionInfo);
	}
	return &_versionInfo;
}

BOOL IsOSVersionOrLater(DWORD major, DWORD minor) {
	OSVERSIONINFOEX *versionInfo = GetVersionInfo();
	return versionInfo->dwMajorVersion > major || (versionInfo->dwMajorVersion == major && versionInfo->dwMinorVersion >= minor);
}

BOOL IsOSVersionOrEarlier(DWORD major, DWORD minor) {
	OSVERSIONINFOEX *versionInfo = GetVersionInfo();
	return versionInfo->dwMajorVersion < major || (versionInfo->dwMajorVersion == major && versionInfo->dwMinorVersion <= minor);
}
