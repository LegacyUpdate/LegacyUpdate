#include "Compat.h"
#include <windows.h>
#include <shellscalingapi.h>
#include "resource.h"

typedef BOOL (WINAPI *_GetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);

typedef BOOL (WINAPI *_SetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);
typedef HRESULT (WINAPI *_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
typedef void (WINAPI *_SetProcessDPIAware)();

typedef HANDLE (WINAPI *_CreateActCtxW)(const ACTCTX *);
typedef BOOL (WINAPI *_ActivateActCtx)(HANDLE, ULONG_PTR *);
typedef BOOL (WINAPI *_DeactivateActCtx)(DWORD, ULONG_PTR);
typedef void (WINAPI *_ReleaseActCtx)(HANDLE);
typedef BOOL (WINAPI *_GetCurrentActCtx)(HANDLE *);

static BOOL productInfoLoaded = FALSE;
static _GetProductInfo $GetProductInfo;

static BOOL actCtxLoaded = FALSE;
static _CreateActCtxW $CreateActCtx;
static _ActivateActCtx $ActivateActCtx;
static _DeactivateActCtx $DeactivateActCtx;
static _ReleaseActCtx $ReleaseActCtx;
static _GetCurrentActCtx $GetCurrentActCtx;

static BOOL comctlLoaded = FALSE;
static HMODULE hComctl32 = NULL;

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

// Handling for SxS contexts (mainly for comctl 6.0)
void IsolationAwareStart(ULONG_PTR *cookie) {
	*cookie = 0;

	if (!actCtxLoaded) {
		actCtxLoaded = TRUE;
		HMODULE kernel32 = GetModuleHandle(L"kernel32.dll");
		$CreateActCtx = (_CreateActCtxW)GetProcAddress(kernel32, "CreateActCtxW");
		$ActivateActCtx = (_ActivateActCtx)GetProcAddress(kernel32, "ActivateActCtx");
		$DeactivateActCtx = (_DeactivateActCtx)GetProcAddress(kernel32, "DeactivateActCtx");
		$ReleaseActCtx = (_ReleaseActCtx)GetProcAddress(kernel32, "ReleaseActCtx");
		$GetCurrentActCtx = (_GetCurrentActCtx)GetProcAddress(kernel32, "GetCurrentActCtx");
	}

	if (!$CreateActCtx || !$ActivateActCtx || !$ReleaseActCtx) {
		return;
	}

	// Borrowing the manifest from shell32.dll
	WCHAR shell32[MAX_PATH];
	GetModuleFileName(GetModuleHandle(L"shell32.dll"), shell32, ARRAYSIZE(shell32));

	ACTCTX actCtx = {0};
	actCtx.cbSize = sizeof(actCtx);
	actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
	actCtx.lpSource = shell32;
	actCtx.lpResourceName = MAKEINTRESOURCE(124);

	HANDLE hActCtx = $CreateActCtx(&actCtx);
	if (hActCtx != INVALID_HANDLE_VALUE) {
		if (!$ActivateActCtx(hActCtx, cookie)) {
			$ReleaseActCtx(hActCtx);
			hActCtx = INVALID_HANDLE_VALUE;
		}
	}
}

void IsolationAwareEnd(ULONG_PTR *cookie) {
	if (!$DeactivateActCtx || !$GetCurrentActCtx) {
		return;
	}

	if (*cookie != 0) {
		HANDLE hActCtx = INVALID_HANDLE_VALUE;
		if ($GetCurrentActCtx(&hActCtx) && hActCtx != INVALID_HANDLE_VALUE) {
			$DeactivateActCtx(0, *cookie);
			$ReleaseActCtx(hActCtx);
		}
		*cookie = 0;
	}
}
