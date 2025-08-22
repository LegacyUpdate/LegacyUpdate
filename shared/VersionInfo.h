#pragma once

#include <windows.h>
#include "stdafx.h"

// Windows 11 Copper (22H2). "WIN10" typo is from the original sdkddkvers.h
#ifndef NTDDI_WIN10_CU
#define NTDDI_WIN10_CU 0x0A00000D
#endif

#define BUILD_WIN10_1507 10240
#define BUILD_WIN10_1511 10586
#define BUILD_WIN10_1607 14393
#define BUILD_WIN10_1703 15063
#define BUILD_WIN10_1709 16299
#define BUILD_WIN10_1803 17134
#define BUILD_WIN10_1809 17763
#define BUILD_WIN10_1903 18362
#define BUILD_WIN10_1909 18363
#define BUILD_WIN10_2004 19041
#define BUILD_WIN10_20H2 19042
#define BUILD_WIN10_21H1 19043
#define BUILD_WIN10_21H2 19044
#define BUILD_WIN10_22H2 19045
#define BUILD_WIN11_21H1 22000
#define BUILD_WIN11_22H2 22621
#define BUILD_WIN11_23H2 22631
#define BUILD_WIN11_24H2 26100

// Undocumented IsOS() flags
#define OS_STARTER        38 // Starter Edition
#define OS_STORAGESERVER  40 // Windows Storage Server 2003
#define OS_COMPUTECLUSTER 41 // Windows Compute Cluster 2003
#define OS_SERVERR2       42 // Windows Server 2003 R2 (in combination with edition)
#define OS_EMBPOS         43 // Windows Embedded for Point of Service
#define OS_HOMESERVER     43 // Windows Home Server (2007)
#define OS_WINFLP         44 // Windows Fundamentals for Legacy PCs
#define OS_EMBSTD2009     45 // Windows Embedded Standard 2009
#define OS_EMBPOS2009     46 // Windows Embedded POSReady 2009

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#define OWN_MODULE ((HMODULE)&__ImageBase)

static OSVERSIONINFOEX _versionInfo;

static OSVERSIONINFOEX *GetVersionInfo() {
	if (_versionInfo.dwOSVersionInfoSize == 0) {
		ZeroMemory(&_versionInfo, sizeof(OSVERSIONINFOEX));
		_versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((LPOSVERSIONINFO)&_versionInfo);
	}
	return &_versionInfo;
}

static ALWAYS_INLINE WORD GetWinVer() {
	return __builtin_bswap16(LOWORD(GetVersion()));
}

#define _IS_OS_MACRO(name, ver) \
	static ALWAYS_INLINE BOOL IsWin ## name() { \
		return GetWinVer() == ver; \
	} \
	static ALWAYS_INLINE BOOL AtLeastWin ## name() { \
		return GetWinVer() >= ver; \
	} \
	static ALWAYS_INLINE BOOL AtMostWin ## name() { \
		return GetWinVer() <= ver; \
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

#define _IS_BUILD_MACRO(ver) \
	static ALWAYS_INLINE BOOL IsWin ## ver() { \
		return GetVersionInfo()->dwBuildNumber == BUILD_WIN ## ver; \
	} \
	static ALWAYS_INLINE BOOL AtLeastWin ## ver() { \
		return GetVersionInfo()->dwBuildNumber >= BUILD_WIN ## ver; \
	} \
	static ALWAYS_INLINE BOOL AtMostWin ## ver() { \
		return GetVersionInfo()->dwBuildNumber <= BUILD_WIN ## ver; \
	}

_IS_BUILD_MACRO(10_1507)
_IS_BUILD_MACRO(10_1511)
_IS_BUILD_MACRO(10_1607)
_IS_BUILD_MACRO(10_1703)
_IS_BUILD_MACRO(10_1709)
_IS_BUILD_MACRO(10_1803)
_IS_BUILD_MACRO(10_1809)
_IS_BUILD_MACRO(10_1903)
_IS_BUILD_MACRO(10_1909)
_IS_BUILD_MACRO(10_2004)
_IS_BUILD_MACRO(10_20H2)
_IS_BUILD_MACRO(10_21H1)
_IS_BUILD_MACRO(10_21H2)
_IS_BUILD_MACRO(10_22H2)
_IS_BUILD_MACRO(11_21H1)
_IS_BUILD_MACRO(11_22H2)
_IS_BUILD_MACRO(11_23H2)
_IS_BUILD_MACRO(11_24H2)
#undef _IS_BUILD_MACRO

static ALWAYS_INLINE void GetOwnFileName(LPWSTR *filename) {
	*filename = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
	GetModuleFileName((HMODULE)&__ImageBase, *filename, MAX_PATH);
}
