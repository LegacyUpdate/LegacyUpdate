// ElevationHelper.cpp : Implementation of CElevationHelper
#include "stdafx.h"
#include "Compat.h"
#include "ElevationHelper.h"
#include "HResult.h"
#include "NGen.h"
#include "Utils.h"
#include <strsafe.h>

const WCHAR *permittedProgIDs[] = {
	L"Microsoft.Update."
};

static CElevationHelperVtbl CElevationHelperVtable = {
	ElevationHelper_QueryInterface,
	ElevationHelper_AddRef,
	ElevationHelper_Release,
	ElevationHelper_GetTypeInfoCount,
	ElevationHelper_GetTypeInfo,
	ElevationHelper_GetIDsOfNames,
	ElevationHelper_Invoke,
	ElevationHelper_CreateObject,
	ElevationHelper_Reboot,
	ElevationHelper_BeforeUpdate,
	ElevationHelper_AfterUpdate
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

STDMETHODIMP CoCreateInstanceAsAdmin(HWND hwnd, REFCLSID rclsid, REFIID riid, void **ppv) {
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
	return CoGetObject(monikerName, (BIND_OPTS *)&bindOpts, riid, ppv);
}

EXTERN_C HRESULT CreateElevationHelper(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
	if (pUnkOuter != NULL) {
		return CLASS_E_NOAGGREGATION;
	}

	CElevationHelper *pThis = (CElevationHelper *)CoTaskMemAlloc(sizeof(CElevationHelper));
	if (pThis == NULL) {
		return E_OUTOFMEMORY;
	}

	ZeroMemory(pThis, sizeof(CElevationHelper));
	pThis->lpVtbl = &CElevationHelperVtable;
	pThis->refCount = 1;

	BecomeDPIAware();

	HRESULT hr = ElevationHelper_QueryInterface(pThis, riid, ppv);
	ElevationHelper_Release(pThis);

	return hr;
}

STDMETHODIMP ElevationHelper_QueryInterface(CElevationHelper *This, REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, IID_IElevationHelper)) {
		*ppvObject = This;
		ElevationHelper_AddRef(This);
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ElevationHelper_AddRef(CElevationHelper *This) {
	return InterlockedIncrement(&This->refCount);
}

ULONG STDMETHODCALLTYPE ElevationHelper_Release(CElevationHelper *This) {
	ULONG refCount = InterlockedDecrement(&This->refCount);

	if (refCount == 0) {
		CoTaskMemFree(This);
	}

	return refCount;
}

STDMETHODIMP ElevationHelper_GetTypeInfoCount(CElevationHelper *This, UINT *pctinfo) {
	if (pctinfo == NULL) {
		return E_POINTER;
	}
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP ElevationHelper_GetTypeInfo(CElevationHelper *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) {
	return E_NOTIMPL;
}

STDMETHODIMP ElevationHelper_GetIDsOfNames(CElevationHelper *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) {
	return E_NOTIMPL;
}

STDMETHODIMP ElevationHelper_Invoke(CElevationHelper *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) {
	return E_NOTIMPL;
}

STDMETHODIMP ElevationHelper_CreateObject(CElevationHelper *This, BSTR progID, IDispatch **retval) {
	if (progID == NULL || retval == NULL) {
		return E_INVALIDARG;
	}

	*retval = NULL;
	HRESULT hr = S_OK;
	IDispatch *object;
	CLSID clsid;

	if (!ProgIDIsPermitted(progID)) {
		hr = E_ACCESSDENIED;
		goto end;
	}

	hr = CLSIDFromProgID(progID, &clsid);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IDispatch, (void **)&object);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	*retval = object;

end:
	if (!SUCCEEDED(hr)) {
		TRACE(L"CreateObject(%ls) failed: %ls\n", progID, GetMessageForHresult(hr));
	}
	return hr;
}

STDMETHODIMP ElevationHelper_Reboot(CElevationHelper *This) {
	return Reboot();
}

STDMETHODIMP ElevationHelper_BeforeUpdate(CElevationHelper *This) {
	return PauseResumeNGenQueue(FALSE);
}

STDMETHODIMP ElevationHelper_AfterUpdate(CElevationHelper *This) {
	return PauseResumeNGenQueue(TRUE);
}
