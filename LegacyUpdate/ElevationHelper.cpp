// ElevationHelper.cpp : Implementation of CElevationHelper

#include "ElevationHelper.h"
#include "Compat.h"
#include "HResult.h"
#include "NGen.h"
#include "Utils.h"
#include <strsafe.h>
#include <ccomptr.h>
#include <new>

const WCHAR *permittedProgIDs[] = {
	L"Microsoft.Update."
};

DEFINE_UUIDOF(CElevationHelper, CLSID_ElevationHelper);

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

STDMETHODIMP CreateElevationHelper(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
	if (pUnkOuter != NULL) {
		return CLASS_E_NOAGGREGATION;
	}

	CElevationHelper *pThis = (CElevationHelper *)CoTaskMemAlloc(sizeof(CElevationHelper));
	if (pThis == NULL) {
		return E_OUTOFMEMORY;
	}

	new(pThis) CElevationHelper();
	// TODO: Only do this if we're in dllhost
	BecomeDPIAware();
	HRESULT hr = pThis->QueryInterface(riid, ppv);
	pThis->Release();
	return hr;
}

CElevationHelper::~CElevationHelper() {
}

STDMETHODIMP CElevationHelper::QueryInterface(REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown)) {
		*ppvObject = (IUnknown *)(IElevationHelper *)this;
		AddRef();
		return S_OK;
	}

	HRESULT hr = IDispatchImpl<IElevationHelper, &LIBID_LegacyUpdateLib>::QueryInterface(riid, ppvObject);
	if (SUCCEEDED(hr)) {
		return hr;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CElevationHelper::AddRef() {
	return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) CElevationHelper::Release() {
	ULONG count = InterlockedDecrement(&m_refCount);
	if (count == 0) {
		this->~CElevationHelper();
		CoTaskMemFree(this);
	}
	return count;
}

STDMETHODIMP CElevationHelper::CreateObject(BSTR progID, IDispatch **retval) {
	if (progID == NULL || retval == NULL) {
		return E_INVALIDARG;
	}

	*retval = NULL;
	HRESULT hr = S_OK;
	CComPtr<IDispatch> object;
	CLSID clsid;

	if (!ProgIDIsPermitted(progID)) {
		hr = E_ACCESSDENIED;
		goto end;
	}

	hr = CLSIDFromProgID(progID, &clsid);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	hr = object.CoCreateInstance(clsid, IID_IDispatch, NULL, CLSCTX_INPROC_SERVER);
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

STDMETHODIMP CElevationHelper::Reboot() {
	return ::Reboot();
}

STDMETHODIMP CElevationHelper::BeforeUpdate() {
	return PauseResumeNGenQueue(FALSE);
}

STDMETHODIMP CElevationHelper::AfterUpdate() {
	return PauseResumeNGenQueue(TRUE);
}
