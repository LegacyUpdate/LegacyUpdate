// dllmain.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "LegacyUpdate_i.h"
#include "dllmain.h"
#include "dlldatax.h"
#include "ElevationHelper.h"
#include "LegacyUpdate.h"
#include "LegacyUpdateCtrl.h"
#include "ProgressBarControl.h"
#include "Registry.h"
#include "VersionInfo.h"

CLegacyUpdateModule _AtlModule;
HINSTANCE g_hInstance;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved)) {
		return FALSE;
	}
#endif

	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		g_hInstance = hInstance;
		break;

	case DLL_PROCESS_DETACH:
		g_hInstance = NULL;
		break;
	}

	return _AtlModule.DllMain(dwReason, lpReserved);
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void) {
#ifdef _MERGE_PROXYSTUB
	HRESULT hr = PrxDllCanUnloadNow();
	if (hr != S_OK) {
		return hr;
	}
#endif
	return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv) {
#ifdef _MERGE_PROXYSTUB
	if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK) {
		return S_OK;
	}
#endif
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void) {
	LPWSTR installPath;
	HRESULT hr = GetInstallPath(&installPath);
	if (!SUCCEEDED(hr)) {
			return hr;
	}

	LPWSTR filename;
	GetOwnFileName(&filename);

	// Register type library
	CComPtr<ITypeLib> typeLib;
	hr = LoadTypeLib(filename, &typeLib);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = RegisterTypeLib(typeLib, filename, NULL);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	// Get IDs
	LPWSTR libid, clsidCtrl, clsidElevationHelper, clsidProgressBar;
	if (!SUCCEEDED(StringFromCLSID(LIBID_LegacyUpdateLib, &libid)) ||
		!SUCCEEDED(StringFromCLSID(CLSID_LegacyUpdateCtrl, &clsidCtrl)) ||
		!SUCCEEDED(StringFromCLSID(CLSID_ElevationHelper, &clsidElevationHelper)) ||
		!SUCCEEDED(StringFromCLSID(CLSID_ProgressBarControl, &clsidProgressBar))) {
		return E_FAIL;
	}

	// Set vars used for expansions
	SetEnvironmentVariable(L"APPID", CLegacyUpdateModule::GetAppId());
	SetEnvironmentVariable(L"LIBID", libid);
	SetEnvironmentVariable(L"CLSID_LegacyUpdateCtrl", clsidCtrl);
	SetEnvironmentVariable(L"CLSID_ElevationHelper", clsidElevationHelper);
	SetEnvironmentVariable(L"CLSID_ProgressBarControl", clsidProgressBar);
	SetEnvironmentVariable(L"MODULE", filename);
	SetEnvironmentVariable(L"INSTALLPATH", installPath);

	// Main
	RegistryEntry mainEntries[] = {
		{HKEY_CLASSES_ROOT, L"AppID\\%APPID%", L"DllSurrogate", REG_SZ, L""},
		{HKEY_CLASSES_ROOT, L"AppID\\LegacyUpdate.dll", L"AppID", REG_SZ, L""},
		{}
	};
	hr = SetRegistryEntries(mainEntries, TRUE);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	// Register classes
	hr = CLegacyUpdateCtrl::UpdateRegistry(TRUE);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = CElevationHelper::UpdateRegistry(TRUE);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = CProgressBarControl::UpdateRegistry(TRUE);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

#ifdef _MERGE_PROXYSTUB
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = PrxDllRegisterServer();
#endif
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void) {
	LPWSTR filename;
	GetOwnFileName(&filename);

	// Unregister type library
	CComPtr<ITypeLib> typeLib;
	HRESULT hr = LoadTypeLib(filename, &typeLib);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	TLIBATTR *attrs;
	hr = typeLib->GetLibAttr(&attrs);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = UnRegisterTypeLib(attrs->guid, attrs->wMajorVerNum, attrs->wMinorVerNum, attrs->lcid, attrs->syskind);
	typeLib->ReleaseTLibAttr(attrs);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	// Get IDs
	LPWSTR clsidCtrl, clsidElevationHelper, clsidProgressBar;
	if (!SUCCEEDED(StringFromCLSID(CLSID_LegacyUpdateCtrl, &clsidCtrl)) ||
		!SUCCEEDED(StringFromCLSID(CLSID_ElevationHelper, &clsidElevationHelper)) ||
		!SUCCEEDED(StringFromCLSID(CLSID_ProgressBarControl, &clsidProgressBar))) {
		return E_FAIL;
	}

	// Set vars used for expansions
	SetEnvironmentVariable(L"APPID", CLegacyUpdateModule::GetAppId());
	SetEnvironmentVariable(L"CLSID_LegacyUpdateCtrl", clsidCtrl);
	SetEnvironmentVariable(L"CLSID_ElevationHelper", clsidElevationHelper);
	SetEnvironmentVariable(L"CLSID_ProgressBarControl", clsidProgressBar);

	// Delete registry entries
	RegistryEntry entries[] = {
		{HKEY_CLASSES_ROOT, L"AppID\\%APPID%", L"DllSurrogate", 0, DELETE_THIS},
		{HKEY_CLASSES_ROOT, L"AppID\\LegacyUpdate.dll", L"AppID", 0, DELETE_THIS},
		{}
	};
	hr = SetRegistryEntries(entries, TRUE);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	// Unregister classes
	hr = CLegacyUpdateCtrl::UpdateRegistry(FALSE);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = CElevationHelper::UpdateRegistry(FALSE);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = CProgressBarControl::UpdateRegistry(FALSE);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

#ifdef _MERGE_PROXYSTUB
	hr = PrxDllRegisterServer();
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = PrxDllUnregisterServer();
#endif
	return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine) {
	return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
}
