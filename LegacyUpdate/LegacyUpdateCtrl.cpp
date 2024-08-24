// LegacyUpdateCtrl.cpp : Implementation of the CLegacyUpdateCtrl ActiveX Control class.

#include "stdafx.h"
#include "LegacyUpdateCtrl.h"
#include "Utils.h"
#include "Compat.h"
#include "HResult.h"
#include "VersionInfo.h"
#include "ElevationHelper.h"
#include <atlbase.h>
#include <atlcom.h>
#include <ShellAPI.h>
#include <ShlObj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const BSTR permittedHosts[] = {
	L"legacyupdate.net",
	L"test.legacyupdate.net",
	NULL
};
const int permittedHostsMax = 2;

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
	hr = container->QueryInterface(IID_IHTMLDocument2, (void**)&document);
	if (!SUCCEEDED(hr) || document == NULL) {
		TRACE("GetDocument() failed: %ls\n", GetMessageForHresult(hr));
		return NULL;
	}

	return document.Detach();
}

HWND CLegacyUpdateCtrl::GetIEWindowHWND() {
	CComPtr<IOleWindow> oleWindow;
	HRESULT hr = QueryInterface(IID_IOleWindow, (void**)&oleWindow);
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

	for (int i = 0; i < permittedHostsMax; i++) {
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

#define DoIsPermittedCheck() \
	if (!IsPermitted()) { \
		return E_ACCESSDENIED; \
	}

STDMETHODIMP CLegacyUpdateCtrl::CheckControl(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	// Just return true so the site can confirm the control is working.
	*retval = TRUE;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::MessageForHresult(LONG inHresult, BSTR *retval) {
	DoIsPermittedCheck();
	*retval = GetMessageForHresult(inHresult);
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::GetOSVersionInfo(OSVersionField osField, LONG systemMetric, VARIANT *retval) {
	DoIsPermittedCheck();

	VariantInit(retval);
	retval->vt = VT_I4;

	OSVERSIONINFOEX *versionInfo = GetVersionInfo();

	switch (osField) {
	case e_majorVer:
		retval->lVal = versionInfo->dwMajorVersion;
		break;

	case e_minorVer:
		retval->lVal = versionInfo->dwMinorVersion;
		break;

	case e_buildNumber:
		retval->lVal = versionInfo->dwBuildNumber;
		break;

	case e_platform:
		retval->lVal = versionInfo->dwPlatformId;
		break;

	case e_SPMajor:
		retval->lVal = versionInfo->wServicePackMajor;
		break;

	case e_SPMinor:
		retval->lVal = versionInfo->wServicePackMinor;
		break;

	case e_productSuite:
		retval->lVal = versionInfo->wSuiteMask;
		break;

	case e_productType:
		retval->lVal = versionInfo->wProductType;
		break;

	case e_systemMetric:
		retval->lVal = GetSystemMetrics(systemMetric);
		break;

	case e_SPVersionString: {
		LPWSTR data;
		DWORD size;
		HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"BuildLab", NULL, &data, &size);
		if (SUCCEEDED(hr)) {
			retval->vt = VT_BSTR;
			retval->bstrVal = SysAllocStringLen(data, size - 1);
		} else {
			// BuildLab doesn't exist on Windows 2000.
			retval->vt = VT_BSTR;
			retval->bstrVal = SysAllocString(versionInfo->szCSDVersion);
		}
		break;
	}

	case e_controlVersionString: {
		LPWSTR data;
		DWORD size;
		HRESULT hr = GetOwnVersion(&data, &size);
		if (!SUCCEEDED(hr)) {
			return hr;
		}
		retval->vt = VT_BSTR;
		retval->bstrVal = SysAllocStringLen(data, size - 1);
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
			hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName", NULL, &data, &size);
			if (SUCCEEDED(hr)) {
				retval->vt = VT_BSTR;
				retval->bstrVal = SysAllocStringLen(data, size - 1);
			} else {
				VariantClear(retval);
			}
		}
		break;
	}

	case e_displayVersion: {
		LPWSTR data;
		DWORD size;
		HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"DisplayVersion", NULL, &data, &size);
		if (SUCCEEDED(hr)) {
			retval->vt = VT_BSTR;
			retval->bstrVal = SysAllocStringLen(data, size - 1);
		} else {
			VariantClear(retval);
		}
		break;
	}
	}

	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::RequestElevation() {
	DoIsPermittedCheck();

	if (m_elevatedHelper != NULL || !IsOSVersionOrLater(6, 0)) {
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

	elevatedHelper = m_elevatedHelper ? m_elevatedHelper : m_nonElevatedHelper;
	if (elevatedHelper == NULL) {
		// Use the helper directly, without elevation. It's the responsibility of the caller to ensure it
		// is already running as admin on 2k/XP, or that it has requested elevation on Vista+.
		m_nonElevatedHelper.CoCreateInstance(CLSID_ElevationHelper, NULL, CLSCTX_INPROC_SERVER);
		if (!SUCCEEDED(hr)) {
				goto end;
		}
		elevatedHelper = m_nonElevatedHelper;
	}

	return elevatedHelper->CreateObject(progID, retval);

end:
	if (!SUCCEEDED(hr)) {
		TRACE("CreateObject(%ls) failed: %ls\n", progID, GetMessageForHresult(hr));
	}
	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::GetUserType(UserType *retval) {
	DoIsPermittedCheck();

	if (IsUserAnAdmin()) {
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

	HKEY subkey;
	HRESULT hr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired", 0, KEY_READ, &subkey);
	*retval = subkey != NULL;
	if (subkey != NULL) {
		RegCloseKey(subkey);
	}
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_IsWindowsUpdateDisabled(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	DWORD noWU;
	HRESULT hr = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", L"NoWindowsUpdate", NULL, &noWU);
	if (SUCCEEDED(hr) && noWU == 1) {
		*retval = TRUE;
		return S_OK;
	}

	DWORD disableWUAccess;
	hr = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\WindowsUpdate", L"DisableWindowsUpdateAccess", NULL, &disableWUAccess);
	if (SUCCEEDED(hr) && disableWUAccess == 1) {
		*retval = TRUE;
		return S_OK;
	}

	*retval = FALSE;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::RebootIfRequired(void) {
	DoIsPermittedCheck();

	VARIANT_BOOL isRebootRequired;
	if (SUCCEEDED(get_IsRebootRequired(&isRebootRequired)) && isRebootRequired) {
		Reboot();
	}
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::ViewWindowsUpdateLog(void) {
	DoIsPermittedCheck();

	WCHAR windir[MAX_PATH];
	HRESULT hr = SHGetFolderPath(0, CSIDL_WINDOWS, NULL, 0, windir);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SHGetFolderPath() failed: %ls\n", GetMessageForHresult(hr));
		return hr;
	}

	// Try Windows Server 2003 Resource Kit (or MSYS/Cygwin/etc) tail.exe, falling back to directly
	// opening the file (most likely in Notepad).
	// Ignore C4311 and C4302, which is for typecasts. It is due to ShellExec and should be safe to bypass.
	#pragma warning(disable: 4311 4302)
	if ((int)ShellExecute(NULL, L"open", L"tail.exe", L"-f WindowsUpdate.log", windir, SW_SHOWDEFAULT) > 32) {
		return S_OK;
	}
	ShellExecute(NULL, L"open", L"WindowsUpdate.log", NULL, windir, SW_SHOWDEFAULT);
	#pragma warning(default: 4311 4302)
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::OpenWindowsUpdateSettings(void) {
	DoIsPermittedCheck();

	// Some issues arise from the working directory being SysWOW64 rather than System32. Notably,
	// Windows Vista - 8.1 don't have wuauclt.exe in SysWOW64. Disable WOW64 redirection temporarily
	// to work around this.
	PVOID oldValue;
	BOOL isRedirected = DisableWow64FsRedirection(&oldValue);

	WCHAR systemDir[MAX_PATH];
	HRESULT hr = SHGetFolderPath(0, CSIDL_SYSTEM, NULL, 0, systemDir);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SHGetFolderPath() failed: %ls\n", GetMessageForHresult(hr));
		return hr;
	}

	if (IsOSVersionOrLater(10, 0)) {
		// Windows 10+: Open Settings app
		ShellExecute(NULL, NULL, L"ms-settings:windowsupdate-options", NULL, systemDir, SW_SHOWDEFAULT);
	} else if (IsOSVersionOrLater(6, 0)) {
		// Windows Vista, 7, 8: Open Windows Update control panel
		ShellExecute(NULL, NULL, L"wuauclt.exe", L"/ShowOptions", systemDir, SW_SHOWDEFAULT);
	} else {
		// Windows 2000, XP: Open Automatic Updates control panel
		ShellExecute(NULL, NULL, L"wuaucpl.cpl", NULL, systemDir, SW_SHOWDEFAULT);
	}

	// Revert WOW64 redirection if we changed it.
	if (isRedirected) {
		RevertWow64FsRedirection(oldValue);
	}
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_IsUsingWsusServer(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	DWORD useWUServer;
	HRESULT hr = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU", L"UseWUServer", NULL, &useWUServer);
	*retval = SUCCEEDED(hr) && useWUServer == 1;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_WsusServerUrl(BSTR *retval) {
	DoIsPermittedCheck();

	LPWSTR data;
	DWORD size;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", L"WUServer", NULL, &data, &size);
	*retval = SUCCEEDED(hr) ? SysAllocStringLen(data, size - 1) : NULL;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_WsusStatusServerUrl(BSTR *retval) {
	DoIsPermittedCheck();

	LPWSTR data;
	DWORD size;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", L"WUStatusServer", NULL, &data, &size);
	*retval = SUCCEEDED(hr) ? SysAllocStringLen(data, size - 1) : NULL;
	return S_OK;
}
