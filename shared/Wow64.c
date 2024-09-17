#include "stdafx.h"
#include "Wow64.h"

typedef BOOL (WINAPI *_Wow64DisableWow64FsRedirection)(PVOID *OldValue);
typedef BOOL (WINAPI *_Wow64RevertWow64FsRedirection)(PVOID OldValue);

static BOOL _loadedWow64;
_Wow64DisableWow64FsRedirection $Wow64DisableWow64FsRedirection;
_Wow64RevertWow64FsRedirection $Wow64RevertWow64FsRedirection;

static void LoadWow64Symbols() {
	if (!_loadedWow64) {
		_loadedWow64 = TRUE;

		HMODULE kernel32 = GetModuleHandle(L"kernel32.dll");
		$Wow64DisableWow64FsRedirection = (_Wow64DisableWow64FsRedirection)GetProcAddress(kernel32, "Wow64DisableWow64FsRedirection");
		$Wow64RevertWow64FsRedirection = (_Wow64RevertWow64FsRedirection)GetProcAddress(kernel32, "Wow64RevertWow64FsRedirection");
	}
}

BOOL DisableWow64FsRedirection(PVOID *OldValue) {
	LoadWow64Symbols();

	if ($Wow64DisableWow64FsRedirection) {
		return $Wow64DisableWow64FsRedirection(OldValue);
	} else {
		return TRUE;
	}
}

BOOL RevertWow64FsRedirection(PVOID OldValue) {
	LoadWow64Symbols();

	if ($Wow64RevertWow64FsRedirection) {
		return $Wow64RevertWow64FsRedirection(OldValue);
	} else {
		return TRUE;
	}
}
