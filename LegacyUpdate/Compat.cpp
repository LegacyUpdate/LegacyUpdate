#include "Compat.h"
#include <windows.h>
#include <shellscalingapi.h>

typedef BOOL (WINAPI *_GetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);

typedef BOOL (WINAPI *_SetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);
typedef HRESULT (WINAPI *_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
typedef void (WINAPI *_SetProcessDPIAware)();

_GetProductInfo $GetProductInfo = (_GetProductInfo)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetProductInfo");

BOOL GetVistaProductInfo(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType) {
	if ($GetProductInfo) {
		return $GetProductInfo(dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
	} else {
		*pdwReturnedProductType = PRODUCT_UNDEFINED;
		return FALSE;
	}
}

void BecomeDPIAware() {
	// Make the process DPI-aware... hopefully
	// Windows 10 1703+ per-monitor v2
	_SetProcessDpiAwarenessContext $SetProcessDpiAwarenessContext = (_SetProcessDpiAwarenessContext)GetProcAddress(LoadLibrary(L"user32.dll"), "SetProcessDpiAwarenessContext");
	if ($SetProcessDpiAwarenessContext) {
		$SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		return;
	}

	// Windows 8.1 - 10 1607 per-monitor v1
	_SetProcessDpiAwareness $SetProcessDpiAwareness = (_SetProcessDpiAwareness)GetProcAddress(LoadLibrary(L"shcore.dll"), "SetProcessDpiAwareness");
	if ($SetProcessDpiAwareness) {
		$SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		return;
	}

	// Windows Vista - 8
	_SetProcessDPIAware $SetProcessDPIAware = (_SetProcessDPIAware)GetProcAddress(LoadLibrary(L"user32.dll"), "SetProcessDPIAware");
	if ($SetProcessDPIAware) {
		$SetProcessDPIAware();
	}
}
