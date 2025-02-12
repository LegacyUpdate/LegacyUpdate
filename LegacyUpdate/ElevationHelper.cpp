// ElevationHelper.cpp : Implementation of CElevationHelper
#include "stdafx.h"
#include "Compat.h"
#include "dllmain.h"
#include "ElevationHelper.h"
#include "HResult.h"
#include "LegacyUpdate.h"
#include "Registry.h"
#include "Utils.h"
#include "VersionInfo.h"
#include <strsafe.h>

const BSTR permittedProgIDs[] = {
	L"Microsoft.Update.",
	NULL
};

BOOL ProgIDIsPermitted(PWSTR progID) {
	if (progID == NULL) {
		return FALSE;
	}

	for (int i = 0; permittedProgIDs[i] != NULL; i++) {
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

CElevationHelper::CElevationHelper() {
	BecomeDPIAware();
}

STDMETHODIMP CElevationHelper::UpdateRegistry(BOOL bRegister) {
	if (bRegister) {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper", NULL, REG_SZ, L"Legacy Update Elevation Helper"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper\\CurVer", NULL, REG_SZ, L"LegacyUpdate.ElevationHelper.1"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper.1", NULL, REG_SZ, L"Legacy Update Elevation Helper"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper.1\\CLSID", NULL, REG_SZ, L"%CLSID_ElevationHelper%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%", NULL, REG_SZ, L"Legacy Update Elevation Helper"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%", L"AppID", REG_SZ, L"%APPID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%", L"LocalizedString", REG_SZ, L"@%MODULE%,-1"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%\\ProgID", NULL, REG_SZ, L"LegacyUpdate.ElevationHelper.1"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%\\VersionIndependentProgID", NULL, REG_SZ, L"LegacyUpdate.ElevationHelper"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%\\InprocServer32", NULL, REG_SZ, L"%MODULE%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%\\InprocServer32", L"ThreadingModel", REG_SZ, L"Apartment"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%\\TypeLib", NULL, REG_SZ, L"%LIBID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%\\Version", NULL, REG_SZ, L"1.0"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%\\Elevation", L"Enabled", REG_DWORD, (LPVOID)1},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%\\Elevation", L"IconReference", REG_SZ, L"@%INSTALLPATH%\\LegacyUpdate.exe,-100"},
			{}
		};
		return SetRegistryEntries(entries, TRUE);
	} else {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper", NULL, REG_SZ, DELETE_THIS},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ElevationHelper.1", NULL, REG_SZ, DELETE_THIS},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ElevationHelper%", NULL, REG_SZ, DELETE_THIS},
			{}
		};
		return SetRegistryEntries(entries, TRUE);
	}
}

STDMETHODIMP CElevationHelper::CreateObject(BSTR progID, IDispatch **retval) {
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

STDMETHODIMP CElevationHelper::Reboot(void) {
	return ::Reboot();
}
