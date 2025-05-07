// dllmain.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "LegacyUpdate_i.h"
#include "dllmain.h"

#include <strsafe.h>

#include "dlldatax.h"
#include "Registry.h"
#include "LegacyUpdate.h"

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
	// registers object, typelib and all interfaces in typelib
	HRESULT hr = _AtlModule.DllRegisterServer();
	if (!SUCCEEDED(hr)) {
		return hr;
	}

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

	hr = RegCloseKey(subkey);

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
	HRESULT hr = _AtlModule.DllUnregisterServer();
#ifdef _MERGE_PROXYSTUB
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = PrxDllRegisterServer();
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = PrxDllUnregisterServer();
#endif
	return hr;
}


// DllInstall - Adds/Removes entries to the system registry per machine only.
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine) {
	HRESULT hr = E_FAIL;

	// Prevent per-user registration (regsvr32 /i:user)
	AtlSetPerUserRegistration(false);

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
