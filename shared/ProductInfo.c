#include <windows.h>

typedef BOOL (WINAPI *_GetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);

static BOOL productInfoLoaded = FALSE;
static _GetProductInfo $GetProductInfo;

BOOL GetVistaProductInfo(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType) {
	if (!productInfoLoaded) {
		productInfoLoaded = TRUE;
		$GetProductInfo = (_GetProductInfo)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetProductInfo");
	}

	if ($GetProductInfo) {
		return $GetProductInfo(dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
	}

	*pdwReturnedProductType = PRODUCT_UNDEFINED;
	return FALSE;
}
