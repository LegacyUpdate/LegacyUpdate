// ElevationHelper.cpp : Implementation of CElevationHelper
#include "stdafx.h"
#include "ElevationHelper.h"
#include "Utils.h"
#include <strsafe.h>

const BSTR permittedProgIDs[] = {
	L"Microsoft.Update.",
	NULL
};
const int permittedProgIDsMax = 1;

const BSTR nonElevatedProgIDs[] = {
	L"Microsoft.Update.AgentInfo",
	L"Microsoft.Update.SystemInfo",
	NULL
};
const int nonElevatedProgIDsMax = 2;

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

BOOL ProgIDNeedsElevation(PWSTR progID) {
	if (progID == NULL) {
		return FALSE;
	}

	for (int i = 0; i < nonElevatedProgIDsMax; i++) {
		if (wcsncmp(progID, nonElevatedProgIDs[i], wcslen(nonElevatedProgIDs[i])) == 0) {
			return FALSE;
		}
	}
	return TRUE;
}

STDMETHODIMP CoCreateInstanceAsAdmin(HWND hwnd, __in REFCLSID rclsid, __in REFIID riid, __deref_out void **ppv) {
	WCHAR clsidString[45];
	StringFromGUID2(rclsid, clsidString, ARRAYSIZE(clsidString));

	WCHAR monikerName[75];
	HRESULT hresult = StringCchPrintf(monikerName, ARRAYSIZE(monikerName), L"Elevation:Administrator!new:%s", clsidString);
	if (!SUCCEEDED(hresult)) {
		return hresult;
	}

	BIND_OPTS3 bindOpts;
	memset(&bindOpts, 0, sizeof(bindOpts));
	bindOpts.cbStruct = sizeof(bindOpts);
	bindOpts.hwnd = hwnd;
	bindOpts.dwClassContext = CLSCTX_LOCAL_SERVER;
	return CoGetObject(monikerName, &bindOpts, riid, ppv);
}

HRESULT CElevationHelper::CreateObject(BSTR progID, IDispatch **retval) {
	if (progID == NULL) {
		return E_INVALIDARG;
	}

	HRESULT result = S_OK;
	if (!ProgIDIsPermitted(progID)) {
		result = E_ACCESSDENIED;
		goto end;
	}

	CLSID clsid;
	result = CLSIDFromProgID(progID, &clsid);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	IDispatch *object;
	result = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IDispatch, (void**)&object);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	*retval = object;

end:
	if (!SUCCEEDED(result)) {
		TRACE("CreateObject(%ls) failed: %ls\n", progID, GetMessageForHresult(result));
	}
	return result;
}
