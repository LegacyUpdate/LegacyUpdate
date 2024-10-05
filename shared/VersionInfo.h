#pragma once

#include <windows.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

static BOOL _loadedVersionInfo = FALSE;
static OSVERSIONINFOEX _versionInfo;
static DWORD _winVer;

static inline OSVERSIONINFOEX *GetVersionInfo() {
	if (!_loadedVersionInfo) {
		_loadedVersionInfo = TRUE;
		ZeroMemory(&_versionInfo, sizeof(OSVERSIONINFOEX));
		_versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((LPOSVERSIONINFO)&_versionInfo);
		_winVer = (_versionInfo.dwMajorVersion << 8) | _versionInfo.dwMinorVersion;
	}
	return &_versionInfo;
}

#define _IS_OS_MACRO(name, ver) \
	static inline BOOL IsWin ## name() { \
		return LOWORD(GetVersion()) == ver; \
	} \
	static inline BOOL AtLeastWin ## name() { \
		return LOWORD(GetVersion()) >= ver; \
	} \
	static inline BOOL AtMostWin ## name() { \
		return LOWORD(GetVersion()) <= ver; \
	}

_IS_OS_MACRO(NT4,    0x0400)
_IS_OS_MACRO(2000,   0x0500)
_IS_OS_MACRO(XP2002, 0x0501)
_IS_OS_MACRO(XP2003, 0x0502)
_IS_OS_MACRO(Vista,  0x0600)
_IS_OS_MACRO(7,      0x0601)
_IS_OS_MACRO(8,      0x0602)
_IS_OS_MACRO(8_1,    0x0603)
_IS_OS_MACRO(10,     0x0A00)
#undef _IS_OS_MACRO

EXTERN_C HRESULT GetOwnVersion(LPWSTR *version);

static inline void GetOwnFileName(LPWSTR *filename) {
	*filename = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
	GetModuleFileName((HMODULE)&__ImageBase, *filename, MAX_PATH);
}
