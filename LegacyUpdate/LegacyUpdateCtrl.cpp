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

DEFINE_UUIDOF(CLegacyUpdateCtrl, CLSID_LegacyUpdateCtrl);

STDMETHODIMP CreateLegacyUpdateCtrl(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
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

CLegacyUpdateCtrl::~CLegacyUpdateCtrl() {
	if (m_clientSite) {
		m_clientSite->Release();
		m_clientSite = NULL;
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

STDMETHODIMP_(ULONG) CLegacyUpdateCtrl::AddRef() {
	return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) CLegacyUpdateCtrl::Release() {
	ULONG count = InterlockedDecrement(&m_refCount);
	if (count == 0) {
		this->~CLegacyUpdateCtrl();
		CoTaskMemFree(this);
	}
	return count;
}

#pragma mark - IOleObject

STDMETHODIMP CLegacyUpdateCtrl_IOleObject::SetClientSite(IOleClientSite *pClientSite) {
	if (m_pParent->m_clientSite) {
		m_pParent->m_clientSite->Release();
	}
	m_pParent->m_clientSite = pClientSite;
	if (m_pParent->m_clientSite) {
		m_pParent->m_clientSite->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl_IOleObject::GetClientSite(IOleClientSite **ppClientSite) {
	if (ppClientSite == NULL) {
		return E_POINTER;
	}

	if (m_pParent->m_clientSite == NULL) {
		*ppClientSite = NULL;
		return E_FAIL;
	}

	m_pParent->m_clientSite->AddRef();
	*ppClientSite = m_pParent->m_clientSite;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl_IOleObject::Close(DWORD dwSaveOption) {
	if (m_pParent->m_clientSite) {
		m_pParent->m_clientSite->Release();
		m_pParent->m_clientSite = NULL;
	}
	return S_OK;
}

#pragma mark - ILegacyUpdateCtrl

STDMETHODIMP CLegacyUpdateCtrl::GetHTMLDocument(IHTMLDocument2 **retval) {
	if (m_clientSite == NULL) {
		return E_ACCESSDENIED;
	}

	CComPtr<IOleContainer> container;
	HRESULT hr = m_clientSite->GetContainer(&container);
	if (!SUCCEEDED(hr)) {
		TRACE(L"GetDocument() failed: %ls", GetMessageForHresult(hr));
		return hr;
	}

	hr = container->QueryInterface(IID_IHTMLDocument2, (void **)retval);
	if (!SUCCEEDED(hr)) {
		TRACE(L"GetDocument() failed: %ls", GetMessageForHresult(hr));
	}

	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::IsPermitted() {
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
	if (!SUCCEEDED(hr)) {
		goto end;
	}
	if (location == NULL) {
		hr = E_ACCESSDENIED;
		goto end;
	}

	hr = location->get_host(&host);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

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
	if (!SUCCEEDED(hr)) {
		TRACE(L"IsPermitted() failed: %ls", GetMessageForHresult(hr));
	}
	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::GetIEWindowHWND(HWND *retval) {
	*retval = NULL;
	CComPtr<IOleWindow> oleWindow;
	HRESULT hr = QueryInterface(IID_IOleWindow, (void **)&oleWindow);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	hr = oleWindow->GetWindow(retval);

end:
	if (!SUCCEEDED(hr)) {
		TRACE(L"GetIEWindowHWND() failed: %ls", GetMessageForHresult(hr));
	}
	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::GetElevatedHelper(IElevationHelper **retval) {
	IElevationHelper *elevatedHelper = m_elevatedHelper ? m_elevatedHelper : m_nonElevatedHelper;
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

		m_nonElevatedHelper = elevatedHelper;
	}

	*retval = elevatedHelper;
	return S_OK;
}

#define DoIsPermittedCheck() { \
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

STDMETHODIMP CLegacyUpdateCtrl::RequestElevation() {
	DoIsPermittedCheck();

	if (m_elevatedHelper != NULL || !AtLeastWinVista()) {
		return S_OK;
	}

	HWND hwnd;
	GetIEWindowHWND(&hwnd);
	HRESULT hr = CoCreateInstanceAsAdmin(hwnd, CLSID_ElevationHelper, IID_IElevationHelper, (void**)&m_elevatedHelper);
	if (!SUCCEEDED(hr)) {
		TRACE(L"RequestElevation() failed: %ls", GetMessageForHresult(hr));
	}
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
	if (!SUCCEEDED(hr)) {
		return hr;
	}

 	return elevatedHelper->CreateObject(progID, retval);
}

STDMETHODIMP CLegacyUpdateCtrl::SetBrowserHwnd(IUpdateInstaller *installer) {
	DoIsPermittedCheck();

	if (installer == NULL) {
		return E_INVALIDARG;
	}

	CComPtr<IUpdateInstaller> updateInstaller;
	HRESULT hr = installer->QueryInterface(IID_IUpdateInstaller, (void **)&updateInstaller);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	HWND hwnd;
	GetIEWindowHWND(&hwnd);
	updateInstaller->put_ParentHwnd(hwnd);

	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::GetUserType(UserType *retval) {
	DoIsPermittedCheck();

	if (IsUserAdmin()) {
		*retval = e_admin;
	} else if (m_elevatedHelper != NULL) {
		*retval = e_elevated;
	} else {
		*retval = e_nonAdmin;
	}

	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_IsRebootRequired(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	CComPtr<ISystemInformation> systemInfo;
	if (SUCCEEDED(systemInfo.CoCreateInstance(CLSID_SystemInformation, NULL, CLSCTX_INPROC_SERVER))) {
		if (SUCCEEDED(systemInfo->get_RebootRequired(retval)) && *retval == VARIANT_TRUE) {
			return S_OK;
		}
	}

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

STDMETHODIMP CLegacyUpdateCtrl::get_IsWindowsUpdateDisabled(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	DWORD value = 0;
	HRESULT hr = GetRegistryDword(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", L"NoWindowsUpdate", KEY_WOW64_64KEY, &value);
	if (SUCCEEDED(hr) && value == 1) {
		*retval = VARIANT_TRUE;
		return S_OK;
	}

	hr = GetRegistryDword(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\WindowsUpdate", L"DisableWindowsUpdateAccess", KEY_WOW64_64KEY, &value);
	if (SUCCEEDED(hr) && value == 1) {
		*retval = VARIANT_TRUE;
		return S_OK;
	}

	*retval = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::RebootIfRequired() {
	DoIsPermittedCheck();

	VARIANT_BOOL isRebootRequired = VARIANT_FALSE;
	HRESULT hr = get_IsRebootRequired(&isRebootRequired);
	if (SUCCEEDED(hr) && isRebootRequired == VARIANT_TRUE) {
		CComPtr<IUpdateInstaller4> installer;
		hr = installer.CoCreateInstance(CLSID_UpdateInstaller, NULL, CLSCTX_INPROC_SERVER);
		if (SUCCEEDED(hr) && hr != REGDB_E_CLASSNOTREG) {
			hr = installer->Commit(0);
			if (!SUCCEEDED(hr)) {
				return hr;
			}
		}

		IElevationHelper *elevatedHelper;
		hr = GetElevatedHelper(&elevatedHelper);
		if (!SUCCEEDED(hr)) {
			return hr;
		}

		hr = elevatedHelper->Reboot();
	}

	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::ViewWindowsUpdateLog() {
	DoIsPermittedCheck();

	HRESULT hr = StartLauncher(L"/log", FALSE);
	if (!SUCCEEDED(hr)) {
		hr = ::ViewWindowsUpdateLog(SW_SHOWDEFAULT);
	}

	return hr;
}

STDMETHODIMP CLegacyUpdateCtrl::OpenWindowsUpdateSettings() {
	DoIsPermittedCheck();

	HRESULT hr = StartLauncher(L"/options", FALSE);
	if (!SUCCEEDED(hr)) {
		TRACE(L"OpenWindowsUpdateSettings() failed, falling back: %ls", GetMessageForHresult(hr));

		CComPtr<IAutomaticUpdates> automaticUpdates;
		hr = automaticUpdates.CoCreateInstance(CLSID_AutomaticUpdates, NULL, CLSCTX_INPROC_SERVER);

		if (SUCCEEDED(hr)) {
			hr = automaticUpdates->ShowSettingsDialog();
		}

		if (!SUCCEEDED(hr)) {
			TRACE(L"OpenWindowsUpdateSettings() failed: %ls", GetMessageForHresult(hr));
		}
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

STDMETHODIMP CLegacyUpdateCtrl::BeforeUpdate() {
	DoIsPermittedCheck();

	IElevationHelper *elevatedHelper;
	HRESULT hr = GetElevatedHelper(&elevatedHelper);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	return elevatedHelper->BeforeUpdate();
}

STDMETHODIMP CLegacyUpdateCtrl::AfterUpdate() {
	DoIsPermittedCheck();

	IElevationHelper *elevatedHelper;
	HRESULT hr = GetElevatedHelper(&elevatedHelper);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	return elevatedHelper->AfterUpdate();
}
