// dllmain.cpp : Implementation of DLL Exports.

#include "LegacyUpdate_i.h"
#include "dllmain.h"
#include "dlldatax.h"
#include "ClassFactory.h"
#include "ElevationHelper.h"
#include "ElevationHelper.h"
#include "LegacyUpdate.h"
#include "LegacyUpdateCtrl.h"
#include "ProgressBarControl.h"
#include "Registry.h"
#include "VersionInfo.h"
#include <strsafe.h>
#include <shlwapi.h>

static LPCWSTR APPID_LegacyUpdateLib = L"{D0A82CD0-B6F0-4101-83ED-DA47D0D04830}";

HINSTANCE g_hInstance = NULL;
LONG g_serverLocks = 0;

typedef struct ClassEntry {
	const GUID *clsid;
	STDMETHODIMP (*createFunc)(IUnknown *pUnkOuter, REFIID riid, void **ppv);
	STDMETHODIMP (*updateRegistryFunc)(BOOL bRegister);
} ClassEntry;

static ClassEntry g_classEntries[] = {
	{&CLSID_LegacyUpdateCtrl,   &CLegacyUpdateCtrl::Create,   &CLegacyUpdateCtrl::UpdateRegistry},
	{&CLSID_ElevationHelper,    &CElevationHelper::Create,    &CElevationHelper::UpdateRegistry},
	{&CLSID_ProgressBarControl, &CProgressBarControl::Create, &CProgressBarControl::UpdateRegistry}
};

// DLL Entry Point
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	if (!PrxDllMain(hInstance, dwReason, lpReserved)) {
		return FALSE;
	}

	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		g_hInstance = hInstance;
		DisableThreadLibraryCalls(hInstance);
		break;

	case DLL_PROCESS_DETACH:
		g_hInstance = NULL;
		break;
	}

	return TRUE;
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void) {
	HRESULT hr = PrxDllCanUnloadNow();
	if (hr != S_OK) {
		return hr;
	}

	return g_serverLocks == 0 ? S_OK : S_FALSE;
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv) {
	if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK) {
		return S_OK;
	}

	if (ppv == NULL) {
		return E_POINTER;
	}

	*ppv = NULL;

	for (DWORD i = 0; i < ARRAYSIZE(g_classEntries); i++) {
		if (IsEqualCLSID(rclsid, *g_classEntries[i].clsid)) {
			CClassFactory *pFactory;
			HRESULT hr = CClassFactory::Create(NULL, riid, (void **)&pFactory);
			if (pFactory == NULL) {
				return E_OUTOFMEMORY;
			}

			pFactory->createFunc = g_classEntries[i].createFunc;
			pFactory->clsid = g_classEntries[i].clsid;

			*ppv = pFactory;
			return hr;
		}
	}

	return CLASS_E_CLASSNOTAVAILABLE;
}

// Shared register/unregister method
STDAPI UpdateRegistration(BOOL state) {
	HRESULT hr = OleInitialize(NULL);
	BOOL oleInitialized = SUCCEEDED(hr);
	if (hr != RPC_E_CHANGED_MODE && hr != CO_E_ALREADYINITIALIZED) {
		CHECK_HR_OR_RETURN(L"OleInitialize");
	}

	LPWSTR installPath = NULL, filename = NULL, libid = NULL;
	hr = GetInstallPath(&installPath);
	CHECK_HR_OR_GOTO_END(L"GetInstallPath");

	hr = GetOwnFileName(&filename);
	CHECK_HR_OR_GOTO_END(L"GetOwnFileName");

	// Register type library
	if (state) {
		CComPtr<ITypeLib> typeLib;
		hr = LoadTypeLib(filename, &typeLib);
		CHECK_HR_OR_GOTO_END(L"LoadTypeLib");

		hr = RegisterTypeLib(typeLib, filename, NULL);
		CHECK_HR_OR_GOTO_END(L"RegisterTypeLib");
	} else {
		CComPtr<ITypeLib> typeLib;
		hr = LoadRegTypeLib(LIBID_LegacyUpdateLib, 1, 0, LOCALE_NEUTRAL, &typeLib);
		if (hr != TYPE_E_LIBNOTREGISTERED) {
			CHECK_HR_OR_GOTO_END(L"LoadRegTypeLib");

			TLIBATTR *attrs;
			hr = typeLib->GetLibAttr(&attrs);
			CHECK_HR_OR_GOTO_END(L"GetLibAttr");

			hr = UnRegisterTypeLib(attrs->guid, attrs->wMajorVerNum, attrs->wMinorVerNum, attrs->lcid, attrs->syskind);
			typeLib->ReleaseTLibAttr(attrs);
			CHECK_HR_OR_GOTO_END(L"UnRegisterTypeLib");
		}
	}

	// Set vars used for expansions
	hr = StringFromCLSID(LIBID_LegacyUpdateLib, &libid);
	CHECK_HR_OR_GOTO_END(L"StringFromCLSID");

	SetEnvironmentVariable(L"APPID", APPID_LegacyUpdateLib);
	SetEnvironmentVariable(L"LIBID", libid);
	SetEnvironmentVariable(L"MODULE", filename);
	SetEnvironmentVariable(L"INSTALLPATH", installPath);

	CoTaskMemFree(libid);

	// Main
	if (state) {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"AppID\\%APPID%", L"DllSurrogate", REG_SZ, NULL},
			{HKEY_CLASSES_ROOT, L"AppID\\LegacyUpdate.dll", L"AppID", REG_SZ, NULL},
			{}
		};
		hr = SetRegistryEntries(entries);
	} else {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"AppID\\%APPID%", NULL, 0, DELETE_KEY},
			{HKEY_CLASSES_ROOT, L"AppID\\LegacyUpdate.dll", NULL, 0, DELETE_KEY},
			{}
		};
		hr = SetRegistryEntries(entries);
	}

	CHECK_HR_OR_GOTO_END(L"SetRegistryEntries main");

	// Register classes
	for (DWORD i = 0; i < ARRAYSIZE(g_classEntries); i++) {
		ClassEntry entry = g_classEntries[i];
		LPWSTR clsidStr;
		hr = StringFromCLSID(*entry.clsid, &clsidStr);
		CHECK_HR_OR_GOTO_END(L"StringFromCLSID");

		SetEnvironmentVariable(L"CLSID", clsidStr);
		CoTaskMemFree(clsidStr);

		hr = entry.updateRegistryFunc(state);
		CHECK_HR_OR_GOTO_END(L"SetRegistryEntries class");
	}

	// Register proxy
	hr = state ? PrxDllRegisterServer() : PrxDllUnregisterServer();
	CHECK_HR_OR_GOTO_END(L"Proxy registration");

end:
	// Clean up environment
	static LPCWSTR envVars[] = {L"APPID", L"LIBID", L"MODULE", L"INSTALLPATH", L"CLSID"};
	for (DWORD i = 0; i < ARRAYSIZE(envVars); i++) {
		SetEnvironmentVariable(envVars[i], NULL);
	}

	if (installPath) {
		LocalFree(installPath);
	}
	if (filename) {
		LocalFree(filename);
	}
	if (libid) {
		CoTaskMemFree(libid);
	}

	if (oleInitialized) {
		OleUninitialize();
	}
	return hr;
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void) {
	return UpdateRegistration(TRUE);
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void) {
	return UpdateRegistration(FALSE);
}

// DllInstall - Adds/Removes entries to the system registry per machine only.
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine) {
	return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
}
