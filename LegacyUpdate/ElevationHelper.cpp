// ElevationHelper.cpp : Implementation of CElevationHelper

#include "ElevationHelper.h"
#include "Compat.h"
#include "HResult.h"
#include "LegacyUpdate.h"
#include "NGen.h"
#include "Registry.h"
#include "Utils.h"
#include "VersionInfo.h"
#include <strsafe.h>
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
	WCHAR clsidString[39];
	StringFromGUID2(rclsid, clsidString, ARRAYSIZE(clsidString));

	WCHAR monikerName[75];
	wsprintf(monikerName, L"Elevation:Administrator!new:%ls", clsidString);

	BIND_OPTS3 bindOpts;
	ZeroMemory(&bindOpts, sizeof(bindOpts));
	bindOpts.cbStruct = sizeof(bindOpts);
	bindOpts.hwnd = hwnd;
	bindOpts.dwClassContext = CLSCTX_LOCAL_SERVER;
	return CoGetObject(monikerName, (BIND_OPTS *)&bindOpts, riid, ppv);
}

STDMETHODIMP CElevationHelper::Create(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
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
	CHECK_HR_OR_RETURN(L"QueryInterface");
	pThis->Release();
	return hr;
}

CElevationHelper::~CElevationHelper(void) {
}

STDMETHODIMP CElevationHelper::UpdateRegistry(BOOL bRegister) {
	if (bRegister) {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper", NULL, REG_SZ, (LPVOID)L"Legacy Update Elevation Helper"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper\\CurVer", NULL, REG_SZ, (LPVOID)L"LegacyUpdate.ElevationHelper.1"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper.1", NULL, REG_SZ, (LPVOID)L"Legacy Update Elevation Helper"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper.1\\CLSID", NULL, REG_SZ, (LPVOID)L"%CLSID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%", NULL, REG_SZ, (LPVOID)L"Legacy Update Elevation Helper"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%", L"AppID", REG_SZ, (LPVOID)L"%APPID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%", L"LocalizedString", REG_SZ, (LPVOID)L"@%MODULE%,-1"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\ProgID", NULL, REG_SZ, (LPVOID)L"LegacyUpdate.ElevationHelper.1"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\VersionIndependentProgID", NULL, REG_SZ, (LPVOID)L"LegacyUpdate.ElevationHelper"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\InprocServer32", NULL, REG_SZ, (LPVOID)L"%MODULE%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\InprocServer32", L"ThreadingModel", REG_SZ, (LPVOID)L"Apartment"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\TypeLib", NULL, REG_SZ, (LPVOID)L"%LIBID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Version", NULL, REG_SZ, (LPVOID)L"1.0"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Elevation", L"Enabled", REG_DWORD, (LPVOID)1},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Elevation", L"IconReference", REG_SZ, (LPVOID)L"@%INSTALLPATH%\\LegacyUpdate.exe,-100"},
			{}
		};
		return SetRegistryEntries(entries);
	} else {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper", NULL, 0, DELETE_KEY},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper.1", NULL, 0, DELETE_KEY},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%", NULL, 0, DELETE_KEY},
			{}
		};
		return SetRegistryEntries(entries);
	}
}

#pragma mark - IUnknown

STDMETHODIMP CElevationHelper::QueryInterface(REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	*ppvObject = NULL;

	return IDispatchImpl<IElevationHelper, &LIBID_LegacyUpdateLib>::QueryInterface(riid, ppvObject);
}

STDMETHODIMP_(ULONG) CElevationHelper::AddRef(void) {
	return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) CElevationHelper::Release(void) {
	ULONG count = InterlockedDecrement(&m_refCount);
	if (count == 0) {
		this->~CElevationHelper();
		CoTaskMemFree(this);
	}
	return count;
}

#pragma mark - IElevationHelper

STDMETHODIMP CElevationHelper::CreateObject(BSTR progID, IDispatch **retval) {
	if (progID == NULL || retval == NULL) {
		return E_INVALIDARG;
	}

	*retval = NULL;
	HRESULT hr = S_OK;
	CComPtr<IDispatch> object;
	CLSID clsid;

	if (!ProgIDIsPermitted(progID)) {
		return E_ACCESSDENIED;
	}

	hr = CLSIDFromProgID(progID, &clsid);
	CHECK_HR_OR_RETURN(L"CLSIDFromProgID");

	hr = object.CoCreateInstance(clsid, IID_IDispatch, NULL, CLSCTX_INPROC_SERVER);
	CHECK_HR_OR_RETURN(L"CoCreateInstance");

	*retval = object.Detach();
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
