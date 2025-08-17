// LegacyUpdateCtrl.cpp : Implementation of the CLegacyUpdateCtrl ActiveX Control class.

#include "stdafx.h"
#include "LegacyUpdateCtrl.h"
#include "Compat.h"
#include "Dispatch.h"
#include "ElevationHelper.h"
#include "Exec.h"
#include "HResult.h"
#include "LegacyUpdate.h"
#include "ProductName.h"
#include "Registry.h"
#include "User.h"
#include "Utils.h"
#include "VersionInfo.h"
#include "WULog.h"
#include <oleidl.h>
#include <shlobj.h>
#include <wuapi.h>
#include "IUpdateInstaller4.h"

const WCHAR *permittedHosts[] = {
	L"legacyupdate.net",
	L"test.legacyupdate.net"
};

static CLegacyUpdateCtrlVtbl CLegacyUpdateCtrlVtable = {
	LegacyUpdateCtrl_QueryInterface,
	LegacyUpdateCtrl_AddRef,
	LegacyUpdateCtrl_Release,
	LegacyUpdateCtrl_GetTypeInfoCount,
	LegacyUpdateCtrl_GetTypeInfo,
	LegacyUpdateCtrl_GetIDsOfNames,
	LegacyUpdateCtrl_Invoke,
	LegacyUpdateCtrl_CheckControl,
	LegacyUpdateCtrl_MessageForHresult,
	LegacyUpdateCtrl_GetOSVersionInfo,
	LegacyUpdateCtrl_RequestElevation,
	LegacyUpdateCtrl_CreateObject,
	LegacyUpdateCtrl_SetBrowserHwnd,
	LegacyUpdateCtrl_GetUserType,
	LegacyUpdateCtrl_get_IsRebootRequired,
	LegacyUpdateCtrl_get_IsWindowsUpdateDisabled,
	LegacyUpdateCtrl_RebootIfRequired,
	LegacyUpdateCtrl_ViewWindowsUpdateLog,
	LegacyUpdateCtrl_OpenWindowsUpdateSettings,
	LegacyUpdateCtrl_get_IsUsingWsusServer,
	LegacyUpdateCtrl_get_WsusServerUrl,
	LegacyUpdateCtrl_get_WsusStatusServerUrl,
	LegacyUpdateCtrl_BeforeUpdate,
	LegacyUpdateCtrl_AfterUpdate
};

STDMETHODIMP CreateLegacyUpdateCtrl(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
	if (pUnkOuter != NULL) {
		return CLASS_E_NOAGGREGATION;
	}

	CLegacyUpdateCtrl *pThis = (CLegacyUpdateCtrl *)CoTaskMemAlloc(sizeof(CLegacyUpdateCtrl));
	if (pThis == NULL) {
		return E_OUTOFMEMORY;
	}

	ZeroMemory(pThis, sizeof(CLegacyUpdateCtrl));
	pThis->lpVtbl = &CLegacyUpdateCtrlVtable;
	pThis->refCount = 1;
	pThis->windowOnly = TRUE;

	HRESULT hr = LegacyUpdateCtrl_QueryInterface(pThis, riid, ppv);
	LegacyUpdateCtrl_Release(pThis);

	return hr;
}

STDMETHODIMP LegacyUpdateCtrl_QueryInterface(CLegacyUpdateCtrl *This, REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, IID_ILegacyUpdateCtrl) || IsEqualIID(riid, IID_IOleObject) || IsEqualIID(riid, IID_IOleWindow)) {
		*ppvObject = This;
		LegacyUpdateCtrl_AddRef(This);
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE LegacyUpdateCtrl_AddRef(CLegacyUpdateCtrl *This) {
	return InterlockedIncrement(&This->refCount);
}

ULONG STDMETHODCALLTYPE LegacyUpdateCtrl_Release(CLegacyUpdateCtrl *This) {
	ULONG refCount = InterlockedDecrement(&This->refCount);

	if (refCount == 0) {
		if (This->clientSite) {
			This->clientSite->Release();
		}
		if (This->inPlaceSite) {
			This->inPlaceSite->Release();
		}
		if (This->container) {
			This->container->Release();
		}
		if (This->elevatedHelper) {
			// IDispatch_Release(This->elevatedHelper);
		}
		if (This->nonElevatedHelper) {
			// IDispatch_Release(This->nonElevatedHelper);
		}
		CoTaskMemFree(This);
	}

	return refCount;
}

STDMETHODIMP LegacyUpdateCtrl_GetTypeInfoCount(CLegacyUpdateCtrl *This, UINT *pctinfo) {
	if (pctinfo == NULL) {
		return E_POINTER;
	}
	*pctinfo = 1;
	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_GetTypeInfo(CLegacyUpdateCtrl *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) {
	return Dispatch_GetTypeInfo((IDispatch *)This, IID_ILegacyUpdateCtrl, iTInfo, lcid, ppTInfo);
}

STDMETHODIMP LegacyUpdateCtrl_GetIDsOfNames(CLegacyUpdateCtrl *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) {
	return Dispatch_GetIDsOfNames((IDispatch *)This, riid, rgszNames, cNames, lcid, rgDispId);
}

STDMETHODIMP LegacyUpdateCtrl_Invoke(CLegacyUpdateCtrl *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) {
	return Dispatch_Invoke((IDispatch *)This, dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

static CComPtr<IHTMLDocument2> GetHTMLDocument(CLegacyUpdateCtrl *This) {
	if (This->clientSite == NULL) {
		return NULL;
	}

	CComPtr<IOleContainer> container;
	HRESULT hr = This->clientSite->GetContainer(&container);
	if (!SUCCEEDED(hr) || container == NULL) {
		TRACE(L"GetDocument() failed: %ls\n", GetMessageForHresult(hr));
		return NULL;
	}

	CComPtr<IHTMLDocument2> document;
	hr = container->QueryInterface(IID_IHTMLDocument2, (void **)&document);
	if (!SUCCEEDED(hr) || document == NULL) {
		TRACE(L"GetDocument() failed: %ls\n", GetMessageForHresult(hr));
		return NULL;
	}

	return document;
}

static HWND GetIEWindowHWND(CLegacyUpdateCtrl *This) {
	CComPtr<IOleWindow> oleWindow;
	HWND hwnd = NULL;
	HRESULT hr = LegacyUpdateCtrl_QueryInterface(This, IID_IOleWindow, (void **)&oleWindow);
	if (!SUCCEEDED(hr) || !oleWindow) {
		goto end;
	}

	hr = oleWindow->GetWindow(&hwnd);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	return hwnd;

end:
	if (!SUCCEEDED(hr)) {
		TRACE(L"GetIEWindowHWND() failed: %ls\n", GetMessageForHresult(hr));
	}
	return 0;
}

static BOOL IsPermitted(CLegacyUpdateCtrl *This) {
	CComPtr<IHTMLDocument2> document = GetHTMLDocument(This);
	if (document == NULL) {
#ifdef _DEBUG
		// Allow debugging outside of IE (e.g. via PowerShell)
		TRACE(L"GetHTMLDocument() failed - allowing anyway due to debug build\n");
		return TRUE;
#else
		return FALSE;
#endif
	}

	CComPtr<IHTMLLocation> location;
	BSTR host;
	HRESULT hr = document->get_location(&location);
	if (!SUCCEEDED(hr) || location == NULL) {
		goto end;
	}

	hr = location->get_host(&host);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	for (DWORD i = 0; i < ARRAYSIZE(permittedHosts); i++) {
		if (wcscmp(host, permittedHosts[i]) == 0) {
			SysFreeString(host);
			return TRUE;
		}
	}

end:
	if (host) {
		SysFreeString(host);
	}
	if (!SUCCEEDED(hr)) {
		TRACE(L"IsPermitted() failed: %ls\n", GetMessageForHresult(hr));
	}
	return FALSE;
}

static HRESULT GetElevatedHelper(CLegacyUpdateCtrl *This, CComPtr<IElevationHelper> &retval) {
	IElevationHelper *elevatedHelper = This->elevatedHelper ? This->elevatedHelper : This->nonElevatedHelper;
	if (elevatedHelper == NULL) {
		// Use the helper directly, without elevation. It's the responsibility of the caller to ensure it
		// is already running as admin on 2k/XP, or that it has requested elevation on Vista+.
		HRESULT hr = CoCreateInstance(CLSID_ElevationHelper, NULL, CLSCTX_INPROC_SERVER, IID_IElevationHelper, (void **)&elevatedHelper);
		if (!SUCCEEDED(hr)) {
			return hr;
		}
		if (elevatedHelper == NULL) {
			return E_POINTER;
		}

		This->nonElevatedHelper = elevatedHelper;
	}

	retval = elevatedHelper;
	return S_OK;
}

#define DoIsPermittedCheck() \
	if (!IsPermitted(This)) { \
		return E_ACCESSDENIED; \
	}

STDMETHODIMP LegacyUpdateCtrl_SetClientSite(CLegacyUpdateCtrl *This, IOleClientSite *pClientSite) {
	if (This->clientSite) {
		This->clientSite->Release();
	}
	This->clientSite = pClientSite;
	if (This->clientSite) {
		This->clientSite->AddRef();
	}

	DoIsPermittedCheck();
	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_CheckControl(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	// Just return true so the site can confirm the control is working.
	*retval = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_MessageForHresult(CLegacyUpdateCtrl *This, LONG inHresult, BSTR *retval) {
	DoIsPermittedCheck();
	*retval = SysAllocString(GetMessageForHresult(inHresult));
	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_GetOSVersionInfo(CLegacyUpdateCtrl *This, OSVersionField osField, LONG systemMetric, VARIANT *retval) {
	DoIsPermittedCheck();

	VariantInit(retval);

	OSVERSIONINFOEX *versionInfo = GetVersionInfo();

	switch (osField) {
	case e_majorVer:
		retval->vt = VT_UI4;
		retval->ulVal = versionInfo->dwMajorVersion;
		break;

	case e_minorVer:
		retval->vt = VT_UI4;
		retval->ulVal = versionInfo->dwMinorVersion;
		break;

	case e_buildNumber:
		retval->vt = VT_UI4;
		retval->ulVal = versionInfo->dwBuildNumber;
		break;

	case e_platform:
		retval->vt = VT_UI4;
		retval->ulVal = versionInfo->dwPlatformId;
		break;

	case e_SPMajor:
		retval->vt = VT_I4;
		retval->lVal = versionInfo->wServicePackMajor;
		break;

	case e_SPMinor:
		retval->vt = VT_I4;
		retval->lVal = versionInfo->wServicePackMinor;
		break;

	case e_productSuite:
		retval->vt = VT_I4;
		retval->lVal = versionInfo->wSuiteMask;
		break;

	case e_productType:
		retval->vt = VT_I4;
		retval->lVal = versionInfo->wProductType;
		break;

	case e_systemMetric:
		retval->vt = VT_I4;
		retval->lVal = GetSystemMetrics(systemMetric);
		break;

	case e_SPVersionString: {
		LPWSTR data = NULL;
		DWORD size = 0;
		HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"BuildLab", 0, &data, &size);
		retval->vt = VT_BSTR;
		retval->bstrVal = SUCCEEDED(hr)
			? SysAllocStringLen(data, size - 1)
			// BuildLab doesn't exist on Windows 2000.
			: SysAllocString(versionInfo->szCSDVersion);
		if (data) {
			LocalFree(data);
		}
		break;
	}

	case e_controlVersionString: {
		LPWSTR data = NULL;
		HRESULT hr = GetOwnVersion(&data);
		if (!SUCCEEDED(hr)) {
			return hr;
		}
		retval->vt = VT_BSTR;
		retval->bstrVal = SysAllocString(data);
		break;
	}

	case e_VistaProductType: {
		DWORD productType = 0;
		GetVistaProductInfo(versionInfo->dwMajorVersion, versionInfo->dwMinorVersion, versionInfo->wServicePackMajor, versionInfo->wServicePackMinor, &productType);
		retval->vt = VT_UI4;
		retval->ulVal = productType;
		break;
	}

	case e_productName: {
		HRESULT hr = GetOSProductName(retval);
		if (!SUCCEEDED(hr)) {
			LPWSTR data = NULL;
			DWORD size = 0;
			hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName", 0, &data, &size);
			if (SUCCEEDED(hr)) {
				retval->vt = VT_BSTR;
				retval->bstrVal = SysAllocStringLen(data, size - 1);
			}
		}
		break;
	}

	case e_displayVersion: {
		LPWSTR data = NULL;
		DWORD size = 0;
		HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"DisplayVersion", 0, &data, &size);
		if (SUCCEEDED(hr)) {
			retval->vt = VT_BSTR;
			retval->bstrVal = SysAllocStringLen(data, size - 1);
			LocalFree(data);
		}
		break;
	}
	}

	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_RequestElevation(CLegacyUpdateCtrl *This) {
	DoIsPermittedCheck();

	if (This->elevatedHelper != NULL || !AtLeastWinVista()) {
		return S_OK;
	}

	// https://learn.microsoft.com/en-us/windows/win32/com/the-com-elevation-moniker
	HRESULT hr = CoCreateInstanceAsAdmin(GetIEWindowHWND(This), CLSID_ElevationHelper, IID_IElevationHelper, (void**)&This->elevatedHelper);
	if (!SUCCEEDED(hr)) {
		TRACE(L"RequestElevation() failed: %ls\n", GetMessageForHresult(hr));
	}
	return hr;
}

STDMETHODIMP LegacyUpdateCtrl_CreateObject(CLegacyUpdateCtrl *This, BSTR progID, IDispatch **retval) {
	DoIsPermittedCheck();

	HRESULT hr = S_OK;
	CComPtr<IElevationHelper> elevatedHelper;
	if (progID == NULL) {
		hr = E_INVALIDARG;
		goto end;
	}

	if (!ProgIDIsPermitted(progID)) {
		hr = E_ACCESSDENIED;
		goto end;
	}

	hr = GetElevatedHelper(This, elevatedHelper);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

 	return elevatedHelper->CreateObject(progID, retval);

end:
	if (!SUCCEEDED(hr)) {
		TRACE(L"CreateObject(%ls) failed: %ls\n", progID, GetMessageForHresult(hr));
	}
	return hr;
}

STDMETHODIMP LegacyUpdateCtrl_SetBrowserHwnd(CLegacyUpdateCtrl *This, IUpdateInstaller *installer) {
	DoIsPermittedCheck();

	if (installer == NULL) {
		return E_INVALIDARG;
	}

	CComPtr<IUpdateInstaller> updateInstaller = NULL;
	HRESULT hr = installer->QueryInterface(IID_IUpdateInstaller, (void **)&updateInstaller);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	updateInstaller->put_ParentHwnd(GetIEWindowHWND(This));
	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_GetUserType(CLegacyUpdateCtrl *This, UserType *retval) {
	DoIsPermittedCheck();

	if (IsUserAdmin()) {
		// Entire process is elevated.
		*retval = e_admin;
	} else if (This->elevatedHelper != NULL) {
		// Our control has successfully received elevation.
		*retval = e_elevated;
	} else {
		// The control has no admin rights (although it may not have requested them yet).
		*retval = e_nonAdmin;
	}

	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_get_IsRebootRequired(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	// Ask WU itself whether a reboot is required
	CComPtr<ISystemInformation> systemInfo;
	if (SUCCEEDED(systemInfo.CoCreateInstance(CLSID_SystemInformation, NULL, CLSCTX_INPROC_SERVER))) {
		if (SUCCEEDED(systemInfo->get_RebootRequired(retval)) && *retval == VARIANT_TRUE) {
			return S_OK;
		}
	}

	// Check reboot flag in registry
	HKEY subkey = NULL;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired", 0, KEY_READ | KEY_WOW64_64KEY, &subkey));
	if (SUCCEEDED(hr)) {
		RegCloseKey(subkey);
		*retval = VARIANT_TRUE;
		return S_OK;
	}

	*retval = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_get_IsWindowsUpdateDisabled(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	// Future note: These are in HKCU on NT; HKLM on 9x.
	// Remove links and access to Windows Update
	DWORD value = 0;
	HRESULT hr = GetRegistryDword(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", L"NoWindowsUpdate", KEY_WOW64_64KEY, &value);
	if (SUCCEEDED(hr) && value == 1) {
		*retval = VARIANT_TRUE;
		return S_OK;
	}

	// Remove access to use all Windows Update features
	hr = GetRegistryDword(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\WindowsUpdate", L"DisableWindowsUpdateAccess", KEY_WOW64_64KEY, &value);
	if (SUCCEEDED(hr) && value == 1) {
		*retval = VARIANT_TRUE;
		return S_OK;
	}

	*retval = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_RebootIfRequired(CLegacyUpdateCtrl *This) {
	DoIsPermittedCheck();

	HRESULT hr = S_OK;
	VARIANT_BOOL isRebootRequired = VARIANT_FALSE;
	if (SUCCEEDED(LegacyUpdateCtrl_get_IsRebootRequired(This, &isRebootRequired)) && isRebootRequired == VARIANT_TRUE) {
		// Calling Commit() is recommended on Windows 10, to ensure feature updates are properly prepared
		// prior to the reboot. If IUpdateInstaller4 doesn't exist, we can skip this.
		CComPtr<IUpdateInstaller4> installer;
		hr = installer.CoCreateInstance(CLSID_UpdateInstaller, NULL, CLSCTX_INPROC_SERVER);
		if (SUCCEEDED(hr) && hr != REGDB_E_CLASSNOTREG) {
			hr = installer->Commit(0);
			if (!SUCCEEDED(hr)) {
				return hr;
			}
		}

		CComPtr<IElevationHelper> elevatedHelper;
		hr = GetElevatedHelper(This, elevatedHelper);
		if (!SUCCEEDED(hr)) {
			return hr;
		}

		hr = elevatedHelper->Reboot();
	}

	return hr;
}

STDMETHODIMP LegacyUpdateCtrl_ViewWindowsUpdateLog(CLegacyUpdateCtrl *This) {
	DoIsPermittedCheck();

	HRESULT hr = StartLauncher(L"/log", FALSE);
	if (!SUCCEEDED(hr)) {
		// Try directly
		hr = ::ViewWindowsUpdateLog(SW_SHOWDEFAULT);
	}

	return hr;
}

STDMETHODIMP LegacyUpdateCtrl_OpenWindowsUpdateSettings(CLegacyUpdateCtrl *This) {
	DoIsPermittedCheck();

	HRESULT hr = StartLauncher(L"/options", FALSE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"OpenWindowsUpdateSettings() failed, falling back: %ls\n", GetMessageForHresult(hr));

		// Might happen if the site isn't trusted, and the user rejected the IE medium integrity prompt.
		// Use the basic Automatic Updates dialog directly from COM.
		CComPtr<IAutomaticUpdates> automaticUpdates;
		hr = automaticUpdates.CoCreateInstance(CLSID_AutomaticUpdates, NULL, CLSCTX_INPROC_SERVER);

		if (SUCCEEDED(hr)) {
			hr = automaticUpdates->ShowSettingsDialog();
		}

		if (!SUCCEEDED(hr)) {
			TRACE(L"OpenWindowsUpdateSettings() failed: %ls\n", GetMessageForHresult(hr));
		}
	}

	return hr;
}

STDMETHODIMP LegacyUpdateCtrl_get_IsUsingWsusServer(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	DWORD useWUServer = 0;
	HRESULT hr = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU", L"UseWUServer", 0, &useWUServer);
	*retval = SUCCEEDED(hr) && useWUServer == 1 ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_get_WsusServerUrl(CLegacyUpdateCtrl *This, BSTR *retval) {
	DoIsPermittedCheck();

	LPWSTR data = NULL;
	DWORD size = 0;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", L"WUServer", 0, &data, &size);
	*retval = SUCCEEDED(hr) ? SysAllocStringLen(data, size - 1) : NULL;
	if (data) {
		LocalFree(data);
	}
	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_get_WsusStatusServerUrl(CLegacyUpdateCtrl *This, BSTR *retval) {
	DoIsPermittedCheck();

	LPWSTR data = NULL;
	DWORD size = 0;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", L"WUStatusServer", 0, &data, &size);
	*retval = SUCCEEDED(hr) ? SysAllocStringLen(data, size - 1) : NULL;
	if (data) {
		LocalFree(data);
	}
	return S_OK;
}

STDMETHODIMP LegacyUpdateCtrl_BeforeUpdate(CLegacyUpdateCtrl *This) {
	DoIsPermittedCheck();

	CComPtr<IElevationHelper> elevatedHelper;
	HRESULT hr = GetElevatedHelper(This, elevatedHelper);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	return elevatedHelper->BeforeUpdate();
}

STDMETHODIMP LegacyUpdateCtrl_AfterUpdate(CLegacyUpdateCtrl *This) {
	DoIsPermittedCheck();

	CComPtr<IElevationHelper> elevatedHelper;
	HRESULT hr = GetElevatedHelper(This, elevatedHelper);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	return elevatedHelper->AfterUpdate();
}
