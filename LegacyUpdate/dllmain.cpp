// dllmain.cpp : Implementation of DLL Exports.

#include "LegacyUpdate_i.h"
#include "dllmain.h"
#include <strsafe.h>

#include "dlldatax.h"
#include "Registry.h"
#include "LegacyUpdate.h"
#include "../shared/LegacyUpdate.h"
#include "ClassFactory.h"
#include "LegacyUpdateCtrl.h"
#include "ElevationHelper.h"
#include "ProgressBarControl.h"

HINSTANCE g_hInstance = NULL;
LONG g_serverLocks = 0;

typedef struct ClassEntry {
	const GUID *clsid;
	STDMETHODIMP (*createFunc)(IUnknown *pUnkOuter, REFIID riid, void **ppv);
} ClassEntry;

static ClassEntry g_classEntries[] = {
	{&CLSID_LegacyUpdateCtrl,   CreateLegacyUpdateCtrl},
	{&CLSID_ElevationHelper,    CreateElevationHelper},
	{&CLSID_ProgressBarControl, CreateProgressBarControl}
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
			HRESULT hr = CreateClassFactory(NULL, riid, (void **)&pFactory);
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

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void) {
	// registers object, typelib and all interfaces in typelib
	// TODO
	HRESULT hr = S_OK;

	// Fix the icon path
	HKEY subkey = NULL;
	hr = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_CLASSES_ROOT, L"CLSID\\{84F517AD-6438-478F-BEA8-F0B808DC257F}\\Elevation", 0, KEY_WRITE, &subkey));
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	LPWSTR installPath = NULL;
	hr = GetInstallPath(&installPath);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	WCHAR iconRef[512];
	hr = StringCchPrintf((LPWSTR)&iconRef, ARRAYSIZE(iconRef), L"@%ls\\LegacyUpdate.exe,-100", installPath);
	LocalFree(installPath);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = HRESULT_FROM_WIN32(RegSetValueEx(subkey, L"IconReference", 0, REG_SZ, (LPBYTE)iconRef, (DWORD)(lstrlen(iconRef) + 1) * sizeof(TCHAR)));
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = HRESULT_FROM_WIN32(RegCloseKey(subkey));
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	return PrxDllRegisterServer();
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void) {
	// TODO
	HRESULT hr = S_OK;
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = PrxDllRegisterServer();
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	return PrxDllUnregisterServer();
}

// DllInstall - Adds/Removes entries to the system registry per machine only.
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine) {
	HRESULT hr = E_FAIL;

	if (bInstall) {
		hr = DllRegisterServer();
		if (!SUCCEEDED(hr)) {
			DllUnregisterServer();
		}
	} else {
		hr = DllUnregisterServer();
	}

	return hr;
}
