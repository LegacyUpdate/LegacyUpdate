#include "stdafx.h"
#include "Wow64.h"

#if !_WIN64

typedef void (WINAPI *_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
typedef BOOL (WINAPI *_Wow64DisableWow64FsRedirection)(PVOID *OldValue);
typedef BOOL (WINAPI *_Wow64RevertWow64FsRedirection)(PVOID OldValue);

static BOOL _loadedWow64;
static _GetNativeSystemInfo $GetNativeSystemInfo = NULL;
static _Wow64DisableWow64FsRedirection $Wow64DisableWow64FsRedirection;
static _Wow64RevertWow64FsRedirection $Wow64RevertWow64FsRedirection;

static void LoadWow64Symbols(void) {
	if (!_loadedWow64) {
		_loadedWow64 = TRUE;

		HMODULE kernel32 = GetModuleHandle(L"kernel32.dll");
		$GetNativeSystemInfo = (_GetNativeSystemInfo)GetProcAddress(kernel32, "GetNativeSystemInfo");
		$Wow64DisableWow64FsRedirection = (_Wow64DisableWow64FsRedirection)GetProcAddress(kernel32, "Wow64DisableWow64FsRedirection");
		$Wow64RevertWow64FsRedirection = (_Wow64RevertWow64FsRedirection)GetProcAddress(kernel32, "Wow64RevertWow64FsRedirection");
	}
}

void OurGetNativeSystemInfo(LPSYSTEM_INFO lpSystemInfo) {
	LoadWow64Symbols();

	if ($GetNativeSystemInfo) {
		$GetNativeSystemInfo(lpSystemInfo);
		return;
	}

	GetSystemInfo(lpSystemInfo);
}

BOOL DisableWow64FsRedirection(PVOID *OldValue) {
	LoadWow64Symbols();

	if ($Wow64DisableWow64FsRedirection) {
		return $Wow64DisableWow64FsRedirection(OldValue);
	}

	return TRUE;
}

BOOL RevertWow64FsRedirection(PVOID OldValue) {
	LoadWow64Symbols();

	if ($Wow64RevertWow64FsRedirection) {
		return $Wow64RevertWow64FsRedirection(OldValue);
	}

	return TRUE;
}

#endif
