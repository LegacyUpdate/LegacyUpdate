// LegacyUpdateCtrl.cpp : Implementation of the CLegacyUpdateCtrl ActiveX Control class.

#include "LegacyUpdateCtrl.h"
#include "Compat.h"
#include "ElevationHelper.h"
#include "Exec.h"
#include "HResult.h"
#include "LegacyUpdate.h"
#include "ProductName.h"
#include "Registry.h"
#include "User.h"
#include "Utils.h"
#include "Version.h"
#include "VersionInfo.h"
#include "WULog.h"
#include <new>
#include <oleidl.h>
#include <shlobj.h>
#include <wuapi.h>
#include "IUpdateInstaller4.h"

const WCHAR *permittedHosts[] = {
	L"legacyupdate.net",
	L"test.legacyupdate.net"
};

#define LEGACYUPDATECTRL_MISCSTATUS (OLEMISC_RECOMPOSEONRESIZE | OLEMISC_CANTLINKINSIDE | OLEMISC_INSIDEOUT | OLEMISC_ACTIVATEWHENVISIBLE | OLEMISC_SETCLIENTSITEFIRST)

DEFINE_UUIDOF(CLegacyUpdateCtrl, CLSID_LegacyUpdateCtrl);

STDMETHODIMP CLegacyUpdateCtrl::Create(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
	if (pUnkOuter != NULL) {
		return CLASS_E_NOAGGREGATION;
	}

	CLegacyUpdateCtrl *pThis = (CLegacyUpdateCtrl *)CoTaskMemAlloc(sizeof(CLegacyUpdateCtrl));
	if (pThis == NULL) {
		return E_OUTOFMEMORY;
	}

	new(pThis) CLegacyUpdateCtrl();
	HRESULT hr = pThis->QueryInterface(riid, ppv);
	pThis->Release();

	return hr;
}

CLegacyUpdateCtrl::~CLegacyUpdateCtrl(void) {
	if (m_clientSite) {
		m_clientSite->Release();
		m_clientSite = NULL;
	}
	if (m_adviseSink) {
		m_adviseSink->Release();
		m_adviseSink = NULL;
	}
	if (m_elevatedHelper) {
		m_elevatedHelper->Release();
		m_elevatedHelper = NULL;
	}
	if (m_nonElevatedHelper) {
		m_nonElevatedHelper->Release();
		m_nonElevatedHelper = NULL;
	}
}

STDMETHODIMP CLegacyUpdateCtrl::UpdateRegistry(BOOL bRegister) {
	if (bRegister) {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.Control", NULL, REG_SZ, (LPVOID)L"Legacy Update Control"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.Control\\CurVer", NULL, REG_SZ, (LPVOID)L"LegacyUpdate.Control.1"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.Control.1", NULL, REG_SZ, (LPVOID)L"Legacy Update Control"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.Control.1\\CLSID", NULL, REG_SZ, (LPVOID)L"%CLSID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%", NULL, REG_SZ, (LPVOID)L"Legacy Update Control"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%", L"AppID", REG_SZ, (LPVOID)L"%APPID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\ProgID", NULL, REG_SZ, (LPVOID)L"LegacyUpdate.Control.1"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\VersionIndependentProgID", NULL, REG_SZ, (LPVOID)L"LegacyUpdate.Control"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Programmable", NULL, REG_SZ, NULL},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\InprocServer32", NULL, REG_SZ, (LPVOID)L"%MODULE%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\InprocServer32", L"ThreadingModel", REG_SZ, (LPVOID)L"Apartment"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Control", NULL, REG_SZ, NULL},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\TypeLib", NULL, REG_SZ, (LPVOID)L"%LIBID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Version", NULL, REG_SZ, (LPVOID)L"1.0"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\MiscStatus", NULL, REG_DWORD, (LPVOID)LEGACYUPDATECTRL_MISCSTATUS},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Implemented Categories\\{7DD95801-9882-11CF-9FA9-00AA006C42C4}", NULL, REG_SZ, NULL},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Implemented Categories\\{7DD95802-9882-11CF-9FA9-00AA006C42C4}", NULL, REG_SZ, NULL},
			{}
		};
		return SetRegistryEntries(entries);
	} else {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.Control", NULL, 0, DELETE_KEY},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.Control.1", NULL, 0, DELETE_KEY},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%", NULL, 0, DELETE_KEY},
			{}
		};
		return SetRegistryEntries(entries);
	}
}

#pragma mark - IUnknown

STDMETHODIMP CLegacyUpdateCtrl::QueryInterface(REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IOleObject)) {
		*ppvObject = &m_IOleObject;
		AddRef();
		return S_OK;
	}

	return IDispatchImpl<ILegacyUpdateCtrl, &LIBID_LegacyUpdateLib>::QueryInterface(riid, ppvObject);
}

STDMETHODIMP_(ULONG) CLegacyUpdateCtrl::AddRef(void) {
	return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) CLegacyUpdateCtrl::Release(void) {
	ULONG count = InterlockedDecrement(&m_refCount);
	if (count == 0) {
		this->~CLegacyUpdateCtrl();
		CoTaskMemFree(this);
	}
	return count;
}

#pragma mark - ILegacyUpdateCtrl

STDMETHODIMP CLegacyUpdateCtrl::GetHTMLDocument(IHTMLDocument2 **retval) {
	if (m_clientSite == NULL) {
		return E_ACCESSDENIED;
	}

	CComPtr<IOleContainer> container;
	HRESULT hr = m_clientSite->GetContainer(&container);
	CHECK_HR_OR_RETURN(L"GetContainer");

	hr = container->QueryInterface(IID_IHTMLDocument2, (void **)retval);
	CHECK_HR_OR_RETURN(L"QueryInterface IID_IHTMLDocument2");
	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::IsPermitted(void) {
	CComPtr<IHTMLDocument2> document;
	CComPtr<IHTMLLocation> location;
	BSTR host = NULL;
	HRESULT hr = GetHTMLDocument(&document);
	if (!SUCCEEDED(hr)) {
#ifdef _DEBUG
		// Allow debugging outside of IE (e.g. via PowerShell)
		TRACE(L"GetHTMLDocument() failed - allowing anyway due to debug build");
		return S_OK;
#else
		goto end;
#endif
	}

	hr = document->get_location(&location);
	CHECK_HR_OR_GOTO_END(L"get_location");
	if (location == NULL) {
		hr = E_ACCESSDENIED;
		goto end;
	}

	hr = location->get_host(&host);
	CHECK_HR_OR_GOTO_END(L"get_host");

	for (DWORD i = 0; i < ARRAYSIZE(permittedHosts); i++) {
		if (wcscmp(host, permittedHosts[i]) == 0) {
			SysFreeString(host);
			return S_OK;
		}
	}

end:
	if (host) {
		SysFreeString(host);
	}
	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::GetIEWindowHWND(HWND *retval) {
	*retval = NULL;

	if (m_clientSite == NULL) {
		return E_FAIL;
	}

	CComPtr<IOleWindow> oleWindow;
	HRESULT hr = m_clientSite->QueryInterface(IID_IOleWindow, (void **)&oleWindow);
	CHECK_HR_OR_RETURN(L"QueryInterface");

	hr = oleWindow->GetWindow(retval);
	CHECK_HR_OR_RETURN(L"GetWindow");
	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::GetElevatedHelper(IElevationHelper **retval) {
	IElevationHelper *elevatedHelper = m_elevatedHelper ? m_elevatedHelper : m_nonElevatedHelper;
	if (elevatedHelper == NULL) {
		// Use the helper directly, without elevation. It's the responsibility of the caller to ensure it
		// is already running as admin on 2k/XP, or that it has requested elevation on Vista+.
		HRESULT hr = CoCreateInstance(CLSID_ElevationHelper, NULL, CLSCTX_INPROC_SERVER, IID_IElevationHelper, (void **)&elevatedHelper);
		CHECK_HR_OR_RETURN(L"CoCreateInstance");
		if (elevatedHelper == NULL) {
			return E_POINTER;
		}

		m_nonElevatedHelper = elevatedHelper;
	}

	*retval = elevatedHelper;
	return S_OK;
}

#define DoIsPermittedCheck(void) { \
		HRESULT hr = IsPermitted(); \
		if (!SUCCEEDED(hr)) { \
			return hr; \
		} \
	}

STDMETHODIMP CLegacyUpdateCtrl::CheckControl(VARIANT_BOOL *retval) {
	if (retval == NULL) {
		return E_POINTER;
	}

	DoIsPermittedCheck();
	*retval = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::MessageForHresult(LONG inHresult, BSTR *retval) {
	DoIsPermittedCheck();
	LPWSTR message = GetMessageForHresult(inHresult);
	*retval = SysAllocString(message);
	LocalFree(message);
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

	case e_controlVersionString:
		retval->vt = VT_BSTR;
		retval->bstrVal = SysAllocString(L"" VERSION_STRING);
		break;

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
				LocalFree(data);
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

STDMETHODIMP CLegacyUpdateCtrl::RequestElevation(void) {
	DoIsPermittedCheck();

	if (m_elevatedHelper != NULL || !AtLeastWinVista()) {
		return S_OK;
	}

	// https://learn.microsoft.com/en-us/windows/win32/com/the-com-elevation-moniker
	HWND hwnd;
	GetIEWindowHWND(&hwnd);
	HRESULT hr = CoCreateInstanceAsAdmin(hwnd, CLSID_ElevationHelper, IID_IElevationHelper, (void**)&m_elevatedHelper);
	CHECK_HR_OR_RETURN(L"CoCreateInstanceAsAdmin");
	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::CreateObject(BSTR progID, IDispatch **retval) {
	DoIsPermittedCheck();

	if (progID == NULL) {
		return E_INVALIDARG;
	}

	if (!ProgIDIsPermitted(progID)) {
		return E_ACCESSDENIED;
	}

	IElevationHelper *elevatedHelper;
	HRESULT hr = GetElevatedHelper(&elevatedHelper);
	CHECK_HR_OR_RETURN(L"GetElevatedHelper");

	return elevatedHelper->CreateObject(progID, retval);
}

STDMETHODIMP CLegacyUpdateCtrl::SetBrowserHwnd(IUpdateInstaller *installer) {
	DoIsPermittedCheck();

	if (installer == NULL) {
		return E_INVALIDARG;
	}

	IElevationHelper *elevatedHelper;
	HRESULT hr = GetElevatedHelper(&elevatedHelper);
	CHECK_HR_OR_RETURN(L"GetElevatedHelper");

	HWND hwnd;
	hr = GetIEWindowHWND(&hwnd);
	CHECK_HR_OR_RETURN(L"GetIEWindowHWND");

	return elevatedHelper->SetBrowserHwnd(installer, hwnd);
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
	HKEY subkey = NULL;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired", 0, GetRegistryWow64Flag(KEY_READ | KEY_WOW64_64KEY), &subkey));
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

STDMETHODIMP CLegacyUpdateCtrl::RebootIfRequired(void) {
	DoIsPermittedCheck();

	VARIANT_BOOL isRebootRequired = VARIANT_FALSE;
	HRESULT hr = get_IsRebootRequired(&isRebootRequired);
	if (SUCCEEDED(hr) && isRebootRequired == VARIANT_TRUE) {
		// Calling Commit() is recommended on Windows 10, to ensure feature updates are properly prepared prior to the
		// reboot. If IUpdateInstaller4 doesn't exist, we can skip this.
		CComPtr<IUpdateInstaller4> installer;
		hr = installer.CoCreateInstance(CLSID_UpdateInstaller, NULL, CLSCTX_INPROC_SERVER);
		if (SUCCEEDED(hr) && hr != REGDB_E_CLASSNOTREG) {
			hr = installer->Commit(0);
			CHECK_HR_OR_RETURN(L"Commit");
		}

		IElevationHelper *elevatedHelper;
		hr = GetElevatedHelper(&elevatedHelper);
		CHECK_HR_OR_RETURN(L"GetElevatedHelper");

		hr = elevatedHelper->Reboot();
		CHECK_HR_OR_RETURN(L"Reboot");
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

	CHECK_HR_OR_RETURN(L"ViewWindowsUpdateLog");
	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::OpenWindowsUpdateSettings(void) {
	DoIsPermittedCheck();

	HRESULT hr = StartLauncher(L"/options", FALSE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"OpenWindowsUpdateSettings() failed, falling back: %08x", hr);

		// Might happen if the site isn't trusted, and the user rejected the IE medium integrity prompt.
		// Use the basic Automatic Updates dialog directly from COM.
		CComPtr<IAutomaticUpdates> automaticUpdates;
		hr = automaticUpdates.CoCreateInstance(CLSID_AutomaticUpdates, NULL, CLSCTX_INPROC_SERVER);
		CHECK_HR_OR_RETURN(L"CoCreateInstance CLSID_AutomaticUpdates");

		hr = automaticUpdates->ShowSettingsDialog();
		CHECK_HR_OR_RETURN(L"ShowSettingsDialog");
	}

	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::get_IsUsingWsusServer(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	DWORD useWUServer = 0;
	HRESULT hr = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU", L"UseWUServer", 0, &useWUServer);
	*retval = SUCCEEDED(hr) && useWUServer == 1 ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_WsusServerUrl(BSTR *retval) {
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

STDMETHODIMP CLegacyUpdateCtrl::get_WsusStatusServerUrl(BSTR *retval) {
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

STDMETHODIMP CLegacyUpdateCtrl::BeforeUpdate(void) {
	DoIsPermittedCheck();

	IElevationHelper *elevatedHelper;
	HRESULT hr = GetElevatedHelper(&elevatedHelper);
	CHECK_HR_OR_RETURN(L"GetElevatedHelper");

	return elevatedHelper->BeforeUpdate();
}

STDMETHODIMP CLegacyUpdateCtrl::AfterUpdate(void) {
	DoIsPermittedCheck();

	IElevationHelper *elevatedHelper;
	HRESULT hr = GetElevatedHelper(&elevatedHelper);
	CHECK_HR_OR_RETURN(L"GetElevatedHelper");

	return elevatedHelper->AfterUpdate();
}
