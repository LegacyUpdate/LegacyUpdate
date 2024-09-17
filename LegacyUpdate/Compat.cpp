#include "stdafx.h"
#include <comdef.h>
#include "Utils.h"

typedef BOOL (WINAPI *_GetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);

_GetProductInfo $GetProductInfo = (_GetProductInfo)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetProductInfo");

BOOL GetVistaProductInfo(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType) {
	if ($GetProductInfo) {
		return $GetProductInfo(dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
	} else {
		*pdwReturnedProductType = PRODUCT_UNDEFINED;
		return FALSE;
	}
}
