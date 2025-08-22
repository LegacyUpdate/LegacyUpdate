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
	HRESULT hr = OleInitialize(NULL);
	BOOL oleInitialized = SUCCEEDED(hr);
	if (!SUCCEEDED(hr) && hr != RPC_E_CHANGED_MODE && hr != CO_E_ALREADYINITIALIZED) {
		TRACE(L"OleInitialize failed: %08x", hr);
		return hr;
	}

	LPWSTR installPath;
	hr = GetInstallPath(&installPath);
	if (!SUCCEEDED(hr)) {
		TRACE(L"GetInstallPath failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	LPWSTR filename;
	GetOwnFileName(&filename);

	// Register type library
	CComPtr<ITypeLib> typeLib;
	hr = LoadTypeLib(filename, &typeLib);
	if (!SUCCEEDED(hr)) {
		TRACE(L"LoadTypeLib failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	hr = RegisterTypeLib(typeLib, filename, NULL);
	if (!SUCCEEDED(hr)) {
		TRACE(L"RegisterTypeLib failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	// Get IDs
	LPWSTR appid, libid, clsidCtrl, clsidElevationHelper, clsidProgressBar;
	if (!SUCCEEDED(StringFromCLSID(LIBID_LegacyUpdateLib, &libid)) ||
		!SUCCEEDED(StringFromCLSID(CLSID_LegacyUpdateCtrl, &clsidCtrl)) ||
		!SUCCEEDED(StringFromCLSID(CLSID_ElevationHelper, &clsidElevationHelper)) ||
		!SUCCEEDED(StringFromCLSID(CLSID_ProgressBarControl, &clsidProgressBar))) {
		TRACE(L"StringFromCLSID failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return E_FAIL;
	}

	// Set vars used for expansions
	SetEnvironmentVariable(L"APPID", APPID_LegacyUpdateLib);
	SetEnvironmentVariable(L"LIBID", libid);
	SetEnvironmentVariable(L"CLSID_LegacyUpdateCtrl", clsidCtrl);
	SetEnvironmentVariable(L"CLSID_ElevationHelper", clsidElevationHelper);
	SetEnvironmentVariable(L"CLSID_ProgressBarControl", clsidProgressBar);
	SetEnvironmentVariable(L"MODULE", filename);
	SetEnvironmentVariable(L"INSTALLPATH", installPath);

	// Main
	RegistryEntry mainEntries[] = {
		{HKEY_CLASSES_ROOT, L"AppID\\%APPID%", L"DllSurrogate", REG_SZ, NULL},
		{HKEY_CLASSES_ROOT, L"AppID\\LegacyUpdate.dll", L"AppID", REG_SZ, NULL},
		{}
	};
	hr = SetRegistryEntries(mainEntries, TRUE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SetRegistryEntries main failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	// Register classes
	hr = CLegacyUpdateCtrl::UpdateRegistry(TRUE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SetRegistryEntries LegacyUpdateCtrl failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	hr = CElevationHelper::UpdateRegistry(TRUE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SetRegistryEntries ElevationHelper failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	hr = CProgressBarControl::UpdateRegistry(TRUE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SetRegistryEntries ProgressBarControl failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	hr = PrxDllRegisterServer();
	if (!SUCCEEDED(hr)) {
		TRACE(L"PrxDllRegisterServer failed: %08x", hr);
	}
	if (oleInitialized) {
		OleUninitialize();
	}
	return hr;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void) {
	HRESULT hr = OleInitialize(NULL);
	BOOL oleInitialized = SUCCEEDED(hr);
	if (!SUCCEEDED(hr) && hr != RPC_E_CHANGED_MODE && hr != CO_E_ALREADYINITIALIZED) {
		TRACE(L"OleInitialize failed: %08x", hr);
		return hr;
	}

	// Unregister type library
	CComPtr<ITypeLib> typeLib;
	hr = LoadRegTypeLib(LIBID_LegacyUpdateLib, 1, 0, LOCALE_NEUTRAL, &typeLib);
	if (!SUCCEEDED(hr) && hr != TYPE_E_LIBNOTREGISTERED) {
		TRACE(L"LoadRegTypeLib failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	if (hr != TYPE_E_LIBNOTREGISTERED) {
		TLIBATTR *attrs;
		hr = typeLib->GetLibAttr(&attrs);
		if (!SUCCEEDED(hr)) {
			TRACE(L"GetLibAttr failed: %08x", hr);
			if (oleInitialized) {
				OleUninitialize();
			}
			return hr;
		}

		hr = UnRegisterTypeLib(attrs->guid, attrs->wMajorVerNum, attrs->wMinorVerNum, attrs->lcid, attrs->syskind);
		typeLib->ReleaseTLibAttr(attrs);
		if (!SUCCEEDED(hr)) {
			TRACE(L"UnRegisterTypeLib failed: %08x", hr);
			if (oleInitialized) {
				OleUninitialize();
			}
			return hr;
		}
	}

	// Get IDs
	LPWSTR clsidCtrl, clsidElevationHelper, clsidProgressBar;
	if (!SUCCEEDED(StringFromCLSID(CLSID_LegacyUpdateCtrl, &clsidCtrl)) ||
		!SUCCEEDED(StringFromCLSID(CLSID_ElevationHelper, &clsidElevationHelper)) ||
		!SUCCEEDED(StringFromCLSID(CLSID_ProgressBarControl, &clsidProgressBar))) {
		TRACE(L"StringFromCLSID failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return E_FAIL;
	}

	// Set vars used for expansions
	SetEnvironmentVariable(L"APPID", APPID_LegacyUpdateLib);
	SetEnvironmentVariable(L"CLSID_LegacyUpdateCtrl", clsidCtrl);
	SetEnvironmentVariable(L"CLSID_ElevationHelper", clsidElevationHelper);
	SetEnvironmentVariable(L"CLSID_ProgressBarControl", clsidProgressBar);

	// Delete registry entries
	RegistryEntry entries[] = {
		{HKEY_CLASSES_ROOT, L"AppID\\%APPID%", NULL, 0, DELETE_KEY},
		{HKEY_CLASSES_ROOT, L"AppID\\LegacyUpdate.dll", NULL, 0, DELETE_KEY},
		{}
	};
	hr = SetRegistryEntries(entries, TRUE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SetRegistryEntries main failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	// Unregister classes
	hr = CLegacyUpdateCtrl::UpdateRegistry(FALSE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SetRegistryEntries LegacyUpdateCtrl failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	hr = CElevationHelper::UpdateRegistry(FALSE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SetRegistryEntries ElevationHelper failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	hr = CProgressBarControl::UpdateRegistry(FALSE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SetRegistryEntries ProgressBarControl failed: %08x", hr);
		if (oleInitialized) {
			OleUninitialize();
		}
		return hr;
	}

	hr = PrxDllUnregisterServer();
	if (!SUCCEEDED(hr)) {
		TRACE(L"PrxDllUnregisterServer failed: %08x", hr);
	}
	if (oleInitialized) {
		OleUninitialize();
	}
	return hr;
}

// DllInstall - Adds/Removes entries to the system registry per machine only.
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine) {
	return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
}
