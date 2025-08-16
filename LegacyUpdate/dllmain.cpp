// dllmain.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "LegacyUpdate_i.h"
#include "dllmain.h"
#include <strsafe.h>
#include "Registry.h"
#include "LegacyUpdate.h"
#include "../shared/LegacyUpdate.h"
#include "LegacyUpdateCtrl.h"
#include "ElevationHelper.h"
#include "ProgressBarControl.h"

#ifdef __cplusplus
extern "C" {
#endif

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

// Class factory
typedef struct CClassFactory {
	const struct CClassFactoryVtbl *lpVtbl;
	LONG refCount;
	STDMETHODIMP (*createFunc)(IUnknown *pUnkOuter, REFIID riid, void **ppv);
	const GUID *clsid;
} CClassFactory;

typedef struct CClassFactoryVtbl {
	// IUnknown
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(CClassFactory *This, REFIID riid, void **ppvObject);
	ULONG   (STDMETHODCALLTYPE *AddRef)(CClassFactory *This);
	ULONG   (STDMETHODCALLTYPE *Release)(CClassFactory *This);

	// IClassFactory
	HRESULT (STDMETHODCALLTYPE *CreateInstance)(CClassFactory *This, IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
	HRESULT (STDMETHODCALLTYPE *LockServer)(CClassFactory *This, BOOL fLock);
} CClassFactoryVtbl;

static STDMETHODIMP ClassFactory_QueryInterface(CClassFactory *This, REFIID riid, void **ppvObject);
static ULONG STDMETHODCALLTYPE ClassFactory_AddRef(CClassFactory *This);
static ULONG STDMETHODCALLTYPE ClassFactory_Release(CClassFactory *This);
static STDMETHODIMP ClassFactory_CreateInstance(CClassFactory *This, IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
static STDMETHODIMP ClassFactory_LockServer(CClassFactory *This, BOOL fLock);

static CClassFactoryVtbl CClassFactoryVtable = {
	ClassFactory_QueryInterface,
	ClassFactory_AddRef,
	ClassFactory_Release,
	ClassFactory_CreateInstance,
	ClassFactory_LockServer
};

static STDMETHODIMP ClassFactory_QueryInterface(CClassFactory *This, REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	if (IsEqualIID(riid, IID_IUnknown) ||
		IsEqualIID(riid, IID_IClassFactory)) {
		*ppvObject = This;
		ClassFactory_AddRef(This);
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE ClassFactory_AddRef(CClassFactory *This) {
	return InterlockedIncrement(&This->refCount);
}

static ULONG STDMETHODCALLTYPE ClassFactory_Release(CClassFactory *This) {
	ULONG refCount = InterlockedDecrement(&This->refCount);

	if (refCount == 0) {
		CoTaskMemFree(This);
	}

	return refCount;
}

static STDMETHODIMP ClassFactory_CreateInstance(CClassFactory *This, IUnknown *pUnkOuter, REFIID riid, void **ppvObject) {
	return This->createFunc(pUnkOuter, riid, ppvObject);
}

static STDMETHODIMP ClassFactory_LockServer(CClassFactory *This, BOOL fLock) {
	if (fLock) {
		InterlockedIncrement(&g_serverLocks);
	} else {
		InterlockedDecrement(&g_serverLocks);
	}
	return S_OK;
}

// DLL Entry Point
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
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
	return g_serverLocks == 0 ? S_OK : S_FALSE;
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv) {
	if (ppv == NULL) {
		return E_POINTER;
	}

	*ppv = NULL;

	for (DWORD i = 0; i < ARRAYSIZE(g_classEntries); i++) {
		if (IsEqualCLSID(rclsid, *g_classEntries[i].clsid)) {
			CClassFactory *pFactory = (CClassFactory *)CoTaskMemAlloc(sizeof(CClassFactory));
			if (pFactory == NULL) {
				return E_OUTOFMEMORY;
			}

			pFactory->lpVtbl = &CClassFactoryVtable;
			pFactory->refCount = 1;
			pFactory->createFunc = g_classEntries[i].createFunc;
			pFactory->clsid = g_classEntries[i].clsid;

			HRESULT hr = ClassFactory_QueryInterface(pFactory, riid, ppv);
			ClassFactory_Release(pFactory);
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
	return hr;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void) {
	// TODO
	return S_OK;
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

#ifdef __cplusplus
}
#endif
