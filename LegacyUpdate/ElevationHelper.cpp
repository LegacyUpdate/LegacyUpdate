// ElevationHelper.cpp : Implementation of CElevationHelper
#include "stdafx.h"
#include "ElevationHelper.h"
#include "HResult.h"
#include "Utils.h"
#include <strsafe.h>

const BSTR permittedProgIDs[] = {
	L"Microsoft.Update.",
	NULL
};
const int permittedProgIDsMax = 1;

BOOL ProgIDIsPermitted(PWSTR progID) {
	if (progID == NULL) {
		return FALSE;
	}

	for (int i = 0; i < permittedProgIDsMax; i++) {
		if (wcsncmp(progID, permittedProgIDs[i], wcslen(permittedProgIDs[i])) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

STDMETHODIMP CoCreateInstanceAsAdmin(HWND hwnd, __in REFCLSID rclsid, __in REFIID riid, __deref_out void **ppv) {
	WCHAR clsidString[45];
	StringFromGUID2(rclsid, clsidString, ARRAYSIZE(clsidString));

	WCHAR monikerName[75];
	HRESULT hr = StringCchPrintf(monikerName, ARRAYSIZE(monikerName), L"Elevation:Administrator!new:%s", clsidString);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	BIND_OPTS3 bindOpts;
	memset(&bindOpts, 0, sizeof(bindOpts));
	bindOpts.cbStruct = sizeof(bindOpts);
	bindOpts.hwnd = hwnd;
	bindOpts.dwClassContext = CLSCTX_LOCAL_SERVER;
	return CoGetObject(monikerName, &bindOpts, riid, ppv);
}

#ifndef PROCESS_PER_MONITOR_DPI_AWARE
typedef int PROCESS_DPI_AWARENESS;
#define PROCESS_PER_MONITOR_DPI_AWARE 2
#endif

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
typedef int DPI_AWARENESS_CONTEXT;
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif

typedef BOOL (WINAPI *_SetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value);
typedef HRESULT (WINAPI *_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
typedef void (WINAPI *_SetProcessDPIAware)();

static void BecomeDPIAware() {
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

CElevationHelper::CElevationHelper() {
	BecomeDPIAware();
}

HRESULT CElevationHelper::CreateObject(BSTR progID, IDispatch **retval) {
	if (progID == NULL) {
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;
	CComPtr<IDispatch> object;
	if (!ProgIDIsPermitted(progID)) {
		hr = E_ACCESSDENIED;
		goto end;
	}

	CLSID clsid;
	hr = CLSIDFromProgID(progID, &clsid);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	hr = object.CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	*retval = object.Detach();

end:
	if (!SUCCEEDED(hr)) {
		TRACE("CreateObject(%ls) failed: %ls\n", progID, GetMessageForHresult(hr));
	}
	return hr;
}
