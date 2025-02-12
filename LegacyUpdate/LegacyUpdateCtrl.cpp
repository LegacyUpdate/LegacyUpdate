// LegacyUpdateCtrl.cpp : Implementation of the CLegacyUpdateCtrl ActiveX Control class.

#include "stdafx.h"
#include "LegacyUpdateCtrl.h"
#include "Compat.h"
#include "ElevationHelper.h"
#include "Exec.h"
#include "HResult.h"
#include "LegacyUpdate.h"
#include "Registry.h"
#include "User.h"
#include "Utils.h"
#include "VersionInfo.h"
#include "WULog.h"
#include <atlbase.h>
#include <ShlObj.h>
#include <wuapi.h>
#include "IUpdateInstaller4.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const BSTR permittedHosts[] = {
	L"legacyupdate.net",
	L"test.legacyupdate.net",
	NULL
};

// CLegacyUpdateCtrl message handlers

IHTMLDocument2 *CLegacyUpdateCtrl::GetHTMLDocument() {
	CComPtr<IOleClientSite> clientSite;
	HRESULT hr = GetClientSite(&clientSite);
	if (!SUCCEEDED(hr) || clientSite == NULL) {
		TRACE("GetDocument() failed: %ls\n", GetMessageForHresult(hr));
		return NULL;
	}

	CComPtr<IOleContainer> container;
	hr = clientSite->GetContainer(&container);
	if (!SUCCEEDED(hr) || container == NULL) {
		TRACE("GetDocument() failed: %ls\n", GetMessageForHresult(hr));
		return NULL;
	}

	CComPtr<IHTMLDocument2> document;
	hr = container->QueryInterface(IID_IHTMLDocument2, (void **)&document);
	if (!SUCCEEDED(hr) || document == NULL) {
		TRACE("GetDocument() failed: %ls\n", GetMessageForHresult(hr));
		return NULL;
	}

	return document.Detach();
}

HWND CLegacyUpdateCtrl::GetIEWindowHWND() {
	CComPtr<IOleWindow> oleWindow;
	HRESULT hr = QueryInterface(IID_IOleWindow, (void **)&oleWindow);
	if (!SUCCEEDED(hr) || !oleWindow) {
		goto end;
	}

	HWND hwnd;
	hr = oleWindow->GetWindow(&hwnd);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	return hwnd;

end:
	if (!SUCCEEDED(hr)) {
		TRACE("GetIEWindowHWND() failed: %ls\n", GetMessageForHresult(hr));
	}
	return 0;
}

BOOL CLegacyUpdateCtrl::IsPermitted(void) {
	CComPtr<IHTMLDocument2> document = GetHTMLDocument();
	if (document == NULL) {
#ifdef _DEBUG
		// Allow debugging outside of IE (e.g. via PowerShell)
		TRACE("GetHTMLDocument() failed - allowing anyway due to debug build\n");
		return TRUE;
#else
		return FALSE;
#endif
	}

	CComPtr<IHTMLLocation> location;
	CComBSTR host;
	HRESULT hr = document->get_location(&location);
	if (!SUCCEEDED(hr) || location == NULL) {
		goto end;
	}

	hr = location->get_host(&host);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	for (int i = 0; permittedHosts[i] != NULL; i++) {
		if (wcscmp(host, permittedHosts[i]) == 0) {
			return TRUE;
		}
	}

end:
	if (!SUCCEEDED(hr)) {
		TRACE("IsPermitted() failed: %ls\n", GetMessageForHresult(hr));
	}
	return FALSE;
}

STDMETHODIMP CLegacyUpdateCtrl::GetElevatedHelper(CComPtr<IElevationHelper> &retval) {
	CComPtr<IElevationHelper> elevatedHelper = m_elevatedHelper ? m_elevatedHelper : m_nonElevatedHelper;
	if (elevatedHelper == NULL) {
		// Use the helper directly, without elevation. It's the responsibility of the caller to ensure it
		// is already running as admin on 2k/XP, or that it has requested elevation on Vista+.
		HRESULT hr = m_nonElevatedHelper.CoCreateInstance(CLSID_ElevationHelper, NULL, CLSCTX_INPROC_SERVER);
		if (!SUCCEEDED(hr)) {
			return hr;
		}

		elevatedHelper = m_nonElevatedHelper;
	}

	retval = elevatedHelper;
	return S_OK;
}

#define DoIsPermittedCheck() \
	if (!IsPermitted()) { \
		return E_ACCESSDENIED; \
	}

STDMETHODIMP CLegacyUpdateCtrl::SetClientSite(IOleClientSite *pClientSite) {
	HRESULT hr = IOleObjectImpl::SetClientSite(pClientSite);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	DoIsPermittedCheck();
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::CheckControl(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	// Just return true so the site can confirm the control is working.
	*retval = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::MessageForHresult(LONG inHresult, BSTR *retval) {
	DoIsPermittedCheck();
	*retval = SysAllocString(GetMessageForHresult(inHresult));
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::GetOSVersionInfo(OSVersionField osField, LONG systemMetric, VARIANT *retval) {
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
		LPWSTR data;
		DWORD size;
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
		LPWSTR data;
		HRESULT hr = GetOwnVersion(&data);
		if (!SUCCEEDED(hr)) {
			return hr;
		}
		retval->vt = VT_BSTR;
		retval->bstrVal = SysAllocString(data);
		break;
	}

	case e_VistaProductType: {
		DWORD productType;
		GetVistaProductInfo(versionInfo->dwMajorVersion, versionInfo->dwMinorVersion, versionInfo->wServicePackMajor, versionInfo->wServicePackMinor, &productType);
		retval->vt = VT_UI4;
		retval->ulVal = productType;
		break;
	}

	case e_productName: {
		HRESULT hr = GetOSProductName(retval);
		if (!SUCCEEDED(hr)) {
			LPWSTR data;
			DWORD size;
			hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName", 0, &data, &size);
			if (SUCCEEDED(hr)) {
				retval->vt = VT_BSTR;
				retval->bstrVal = SysAllocStringLen(data, size - 1);
			}
		}
		break;
	}

	case e_displayVersion: {
		LPWSTR data;
		DWORD size;
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

STDMETHODIMP CLegacyUpdateCtrl::RequestElevation() {
	DoIsPermittedCheck();

	if (m_elevatedHelper != NULL || !AtLeastWinVista()) {
		return S_OK;
	}

	// https://learn.microsoft.com/en-us/windows/win32/com/the-com-elevation-moniker
	HRESULT hr = CoCreateInstanceAsAdmin(GetIEWindowHWND(), CLSID_ElevationHelper, IID_IElevationHelper, (void**)&m_elevatedHelper);
	if (!SUCCEEDED(hr)) {
		TRACE("RequestElevation() failed: %ls\n", GetMessageForHresult(hr));
	}
	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::CreateObject(BSTR progID, IDispatch **retval) {
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

	hr = GetElevatedHelper(elevatedHelper);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	return elevatedHelper->CreateObject(progID, retval);

end:
	if (!SUCCEEDED(hr)) {
		TRACE("CreateObject(%ls) failed: %ls\n", progID, GetMessageForHresult(hr));
	}
	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::SetBrowserHwnd(IUpdateInstaller *installer) {
	DoIsPermittedCheck();

	if (installer == NULL) {
		return E_INVALIDARG;
	}

	CComPtr<IUpdateInstaller> updateInstaller = NULL;
	HRESULT hr = installer->QueryInterface(IID_IUpdateInstaller, (void **)&updateInstaller);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	updateInstaller->put_ParentHwnd(GetIEWindowHWND());
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::GetUserType(UserType *retval) {
	DoIsPermittedCheck();

	if (IsUserAdmin()) {
		// Entire process is elevated.
		*retval = e_admin;
	} else if (m_elevatedHelper != NULL) {
		// Our control has successfully received elevation.
		*retval = e_elevated;
	} else {
		// The control has no admin rights (although it may not have requested them yet).
		*retval = e_nonAdmin;
	}

	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_IsRebootRequired(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	// Ask WU itself whether a reboot is required
	CComPtr<ISystemInformation> systemInfo;
	if (SUCCEEDED(systemInfo.CoCreateInstance(CLSID_SystemInformation, NULL, CLSCTX_INPROC_SERVER))) {
		if (SUCCEEDED(systemInfo->get_RebootRequired(retval)) && *retval == VARIANT_TRUE) {
			return S_OK;
		}
	}

	// Check reboot flag in registry
	HKEY subkey;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired", KEY_WOW64_64KEY, KEY_READ, &subkey));
	if (SUCCEEDED(hr)) {
		RegCloseKey(subkey);
		*retval = VARIANT_TRUE;
		return S_OK;
	}

	*retval = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_IsWindowsUpdateDisabled(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	// Future note: These are in HKCU on NT; HKLM on 9x.
	// Remove links and access to Windows Update
	DWORD value;
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

STDMETHODIMP CLegacyUpdateCtrl::RebootIfRequired(void) {
	DoIsPermittedCheck();

	HRESULT hr = S_OK;
	VARIANT_BOOL isRebootRequired;
	if (SUCCEEDED(get_IsRebootRequired(&isRebootRequired)) && isRebootRequired == VARIANT_TRUE) {
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
		hr = GetElevatedHelper(elevatedHelper);
		if (!SUCCEEDED(hr)) {
			return hr;
		}

		hr = elevatedHelper->Reboot();
	}

	return hr;
}

static HRESULT StartLauncher(LPWSTR params, BOOL wait) {
	LPWSTR path;
	HRESULT hr = GetInstallPath(&path);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	PathAppend(path, L"LegacyUpdate.exe");

	DWORD code;
	hr = Exec(L"open", path, params, NULL, SW_SHOW, wait, &code);
	if (SUCCEEDED(hr)) {
		hr = HRESULT_FROM_WIN32(code);
	}

	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::ViewWindowsUpdateLog(void) {
	DoIsPermittedCheck();

	HRESULT hr = StartLauncher(L"/log", FALSE);
	if (!SUCCEEDED(hr)) {
		// Try directly
		hr = ::ViewWindowsUpdateLog(SW_SHOWDEFAULT);
	}

	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::OpenWindowsUpdateSettings(void) {
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

STDMETHODIMP CLegacyUpdateCtrl::get_IsUsingWsusServer(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	DWORD useWUServer;
	HRESULT hr = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU", L"UseWUServer", 0, &useWUServer);
	*retval = SUCCEEDED(hr) && useWUServer == 1 ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_WsusServerUrl(BSTR *retval) {
	DoIsPermittedCheck();

	LPWSTR data;
	DWORD size;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", L"WUServer", 0, &data, &size);
	*retval = SUCCEEDED(hr) ? SysAllocStringLen(data, size - 1) : NULL;
	if (data) {
		LocalFree(data);
	}
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_WsusStatusServerUrl(BSTR *retval) {
	DoIsPermittedCheck();

	LPWSTR data;
	DWORD size;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", L"WUStatusServer", 0, &data, &size);
	*retval = SUCCEEDED(hr) ? SysAllocStringLen(data, size - 1) : NULL;
	if (data) {
		LocalFree(data);
	}
	return S_OK;
}
