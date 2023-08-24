// LegacyUpdateCtrl.cpp : Implementation of the CLegacyUpdateCtrl ActiveX Control class.

#include "stdafx.h"
#include "LegacyUpdateCtrl.h"
#include "Utils.h"
#include "Compat.h"
#include <atlbase.h>
#include <atlcom.h>
#include <ShellAPI.h>
#include <ShlObj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const BSTR permittedProgIDs[] = {
	L"Microsoft.Update.",
	NULL
};
const int permittedProgIDsMax = 1;

const BSTR permittedHosts[] = {
	L"legacyupdate.net",
	L"test.legacyupdate.net",
	NULL
};
const int permittedHostsMax = 2;

// CLegacyUpdateCtrl message handlers

IHTMLDocument2 *CLegacyUpdateCtrl::GetHTMLDocument() {
	IOleClientSite *clientSite;
	HRESULT result = GetClientSite(&clientSite);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	IOleContainer *container;
	result = clientSite->GetContainer(&container);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	IHTMLDocument2 *document;
	result = container->QueryInterface(IID_IHTMLDocument2, (void **)&document);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	return document;

end:
	if (!SUCCEEDED(result)) {
		TRACE("GetDocument() failed: %ls\n", GetMessageForHresult(result));
	}
	return NULL;
}

BOOL CLegacyUpdateCtrl::IsPermitted(void) {
	IHTMLDocument2 *document = GetHTMLDocument();
	if (document == NULL) {
		return FALSE;
	}

	IHTMLLocation *location;
	HRESULT result = document->get_location(&location);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	BSTR host;
	result = location->get_host(&host);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	for (int i = 0; i < permittedHostsMax; i++) {
		if (wcscmp(host, permittedHosts[i]) == 0) {
			SysFreeString(host);
			return TRUE;
		}
	}

	SysFreeString(host);

end:
	if (result != S_OK) {
		TRACE("IsPermitted() failed: %ls\n", GetMessageForHresult(result));
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
		HRESULT regResult = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"BuildLab", NULL, &data, &size);
		if (SUCCEEDED(regResult)) {
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
		HRESULT verResult = GetOwnVersion(&data, &size);
		if (!SUCCEEDED(verResult)) {
			return verResult;
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
			HRESULT regResult = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName", NULL, &data, &size);
			if (SUCCEEDED(regResult)) {
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
		HRESULT regResult = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"DisplayVersion", NULL, &data, &size);
		if (SUCCEEDED(regResult)) {
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

STDMETHODIMP CLegacyUpdateCtrl::CreateObject(BSTR progID, IDispatch **retval) {
	DoIsPermittedCheck();

	if (progID == NULL) {
		return E_INVALIDARG;
	}

	BOOL isPermitted = FALSE;
	if (!isPermitted) {
		for (int i = 0; i < permittedProgIDsMax; i++) {
			if (wcsncmp(progID, permittedProgIDs[i], wcslen(permittedProgIDs[i])) == 0) {
				isPermitted = TRUE;
				break;
			}
		}
	}
	if (!isPermitted) {
		TRACE("CreateObject(%ls) failed: not permitted\n", progID);
		return E_ACCESSDENIED;
	}

	CLSID clsid;
	HRESULT result = CLSIDFromProgID(progID, &clsid);
	if (!SUCCEEDED(result)) {
		TRACE("CreateObject(%ls) failed: %ls\n", progID, GetMessageForHresult(result));
		return result;
	}

	IDispatch *object;
	result = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IDispatch, (void **)&object);
	if (!SUCCEEDED(result)) {
		TRACE("CreateObject(%ls) failed: %ls\n", progID, GetMessageForHresult(result));
		return result;
	}

	*retval = object;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::GetUserType(UserType *retval) {
	DoIsPermittedCheck();
	*retval = IsUserAnAdmin() ? e_admin : e_nonAdmin;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_IsRebootRequired(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	HKEY subkey;
	HRESULT result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired", 0, KEY_READ, &subkey);
	*retval = subkey != NULL;
	if (subkey != NULL) {
		RegCloseKey(subkey);
	}
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_IsWindowsUpdateDisabled(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	DWORD noWU;
	HRESULT result = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", L"NoWindowsUpdate", NULL, &noWU);
	if (SUCCEEDED(result) && noWU == 1) {
		*retval = TRUE;
		return S_OK;
	}

	DWORD disableWUAccess;
	result = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\WindowsUpdate", L"DisableWindowsUpdateAccess", NULL, &disableWUAccess);
	if (SUCCEEDED(result) && disableWUAccess == 1) {
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
	HRESULT result = SHGetFolderPath(0, CSIDL_WINDOWS, NULL, 0, windir);
	if (!SUCCEEDED(result)) {
		TRACE(L"SHGetFolderPath() failed: %ls\n", GetMessageForHresult(result));
		return result;
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

	WCHAR systemDir[MAX_PATH];
	HRESULT result = SHGetFolderPath(0, CSIDL_SYSTEM, NULL, 0, systemDir);
	if (!SUCCEEDED(result)) {
		TRACE(L"SHGetFolderPath() failed: %ls\n", GetMessageForHresult(result));
		return result;
	}

	#pragma warning(disable: 4311 4302)
	DWORD majorVersion = GetVersionInfo()->dwMajorVersion;
	if (majorVersion >= 10) {
		// Windows 10+: Open Settings app
		ShellExecute(NULL, L"open", L"ms-settings:windowsupdate-options", NULL, systemDir, SW_SHOWDEFAULT);
	} else if (majorVersion >= 6) {
		// Windows Vista, 7, 8: Open Windows Update control panel
		ShellExecute(NULL, L"open", L"wuauclt.exe", L"/ShowOptions", systemDir, SW_SHOWDEFAULT);
	} else {
		// Windows 2000, XP: Open Automatic Updates control panel
		ShellExecute(NULL, L"cplopen", L"wuaucpl.cpl", NULL, systemDir, SW_SHOWDEFAULT);
	}
	#pragma warning(default: 4311 4302)
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_IsUsingWsusServer(VARIANT_BOOL *retval) {
	DoIsPermittedCheck();

	DWORD useWUServer;
	HRESULT result = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU", L"UseWUServer", NULL, &useWUServer);
	*retval = SUCCEEDED(result) && useWUServer == 1;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_WsusServerUrl(BSTR *retval) {
	DoIsPermittedCheck();

	LPWSTR data;
	DWORD size;
	HRESULT result = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", L"WUServer", NULL, &data, &size);
	*retval = SUCCEEDED(result) ? SysAllocStringLen(data, size - 1) : NULL;
	return S_OK;
}

STDMETHODIMP CLegacyUpdateCtrl::get_WsusStatusServerUrl(BSTR *retval) {
	DoIsPermittedCheck();

	LPWSTR data;
	DWORD size;
	HRESULT result = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", L"WUStatusServer", NULL, &data, &size);
	*retval = SUCCEEDED(result) ? SysAllocStringLen(data, size - 1) : NULL;
	return S_OK;
}
