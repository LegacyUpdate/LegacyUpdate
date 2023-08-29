#include "stdafx.h"
#include <comdef.h>
#include "Utils.h"

typedef BOOL (WINAPI *_Wow64DisableWow64FsRedirection)(PVOID *OldValue);
typedef BOOL (WINAPI *_Wow64RevertWow64FsRedirection)(PVOID OldValue);
typedef BOOL (WINAPI *_GetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);

// XP+
_Wow64DisableWow64FsRedirection $Wow64DisableWow64FsRedirection = (_Wow64DisableWow64FsRedirection)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "Wow64DisableWow64FsRedirection");
_Wow64RevertWow64FsRedirection $Wow64RevertWow64FsRedirection = (_Wow64RevertWow64FsRedirection)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "Wow64RevertWow64FsRedirection");

// Vista+
_GetProductInfo $GetProductInfo = (_GetProductInfo)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetProductInfo");

BOOL GetVistaProductInfo(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType) {
	if ($GetProductInfo) {
		return $GetProductInfo(dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
	} else {
		*pdwReturnedProductType = PRODUCT_UNDEFINED;
		return FALSE;
	}
}

BOOL DisableWow64FsRedirection(PVOID *OldValue) {
	if ($Wow64DisableWow64FsRedirection) {
		return $Wow64DisableWow64FsRedirection(OldValue);
	} else {
		return TRUE;
	}
}

BOOL RevertWow64FsRedirection(PVOID OldValue) {
	if ($Wow64RevertWow64FsRedirection) {
		return $Wow64RevertWow64FsRedirection(OldValue);
	} else {
		return TRUE;
	}
}
