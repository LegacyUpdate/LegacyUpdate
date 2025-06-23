// ElevationHelper.cpp : Implementation of CElevationHelper
#include "stdafx.h"
#include "Compat.h"
#include "ElevationHelper.h"
#include "HResult.h"
#include "NGen.h"
#include "Utils.h"
#include <strsafe.h>

const BSTR permittedProgIDs[] = {
	L"Microsoft.Update."
};

BOOL ProgIDIsPermitted(PWSTR progID) {
	if (progID == NULL) {
		return FALSE;
	}

	for (DWORD i = 0; i < ARRAYSIZE(permittedProgIDs); i++) {
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
	HRESULT hr = StringCchPrintf(monikerName, ARRAYSIZE(monikerName), L"Elevation:Administrator!new:%ls", clsidString);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	BIND_OPTS3 bindOpts;
	ZeroMemory(&bindOpts, sizeof(bindOpts));
	bindOpts.cbStruct = sizeof(bindOpts);
	bindOpts.hwnd = hwnd;
	bindOpts.dwClassContext = CLSCTX_LOCAL_SERVER;
	return CoGetObject(monikerName, &bindOpts, riid, ppv);
}

CElevationHelper::CElevationHelper() {
	BecomeDPIAware();
}

STDMETHODIMP CElevationHelper::CreateObject(BSTR progID, IDispatch **retval) {
	if (progID == NULL || retval == NULL) {
		return E_INVALIDARG;
	}

	*retval = NULL;
	HRESULT hr = S_OK;
	CComPtr<IDispatch> object;
	CLSID clsid = {0};

	if (!ProgIDIsPermitted(progID)) {
		hr = E_ACCESSDENIED;
		goto end;
	}

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
		TRACE(L"CreateObject(%ls) failed: %ls\n", progID, GetMessageForHresult(hr));
	}
	return hr;
}

STDMETHODIMP CElevationHelper::Reboot(void) {
	return ::Reboot();
}

STDMETHODIMP CElevationHelper::BeforeUpdate(void) {
	return PauseResumeNGenQueue(FALSE);
}

STDMETHODIMP CElevationHelper::AfterUpdate(void) {
	return PauseResumeNGenQueue(TRUE);
}
