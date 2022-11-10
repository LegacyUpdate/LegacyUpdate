// LegacyUpdateCtrl.cpp : Implementation of the CLegacyUpdateCtrl ActiveX Control class.

#include "stdafx.h"
#include "LegacyUpdateOCX.h"
#include "LegacyUpdateCtrl.h"
#include "Utils.h"
#include "Compat.h"
#include <atlbase.h>
#include <atlcom.h>

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

IMPLEMENT_DYNCREATE(CLegacyUpdateCtrl, COleControl)

// Message map

BEGIN_MESSAGE_MAP(CLegacyUpdateCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

// Dispatch map

BEGIN_DISPATCH_MAP(CLegacyUpdateCtrl, COleControl)
	DISP_FUNCTION_ID(CLegacyUpdateCtrl, "CheckControl", dispidCheckControl, CheckControl, VT_I1, VTS_NONE)
	DISP_FUNCTION_ID(CLegacyUpdateCtrl, "MessageForHresult", dispidMessageForHresult, MessageForHresult, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(CLegacyUpdateCtrl, "GetOSVersionInfo", dispidGetOSVersionInfo, GetOSVersionInfo, VT_VARIANT, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CLegacyUpdateCtrl, "CreateObject", dispidCreateObject, CreateComObject, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION_ID(CLegacyUpdateCtrl, "GetUserType", dispidGetUserType, GetUserType, VT_I1, VTS_NONE)
	DISP_PROPERTY_EX_ID(CLegacyUpdateCtrl, "IsRebootRequired", dispidIsRebootRequired, IsRebootRequired, SetNotSupported, VT_BOOL)
	DISP_PROPERTY_EX_ID(CLegacyUpdateCtrl, "IsWindowsUpdateDisabled", dispidIsWindowsUpdateDisabled, IsWindowsUpdateDisabled, SetNotSupported, VT_BOOL)
	DISP_FUNCTION_ID(CLegacyUpdateCtrl, "RebootIfRequired", dispidRebootIfRequired, RebootIfRequired, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CLegacyUpdateCtrl, "ViewWindowsUpdateLog", dispidViewWindowsUpdateLog, ViewWindowsUpdateLog, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX_ID(CLegacyUpdateCtrl, "IsUsingWsusServer", dispidIsUsingWsusServer, IsUsingWsusServer, SetNotSupported, VT_BOOL)
	DISP_PROPERTY_EX_ID(CLegacyUpdateCtrl, "WsusServerUrl", dispidWsusServerUrl, WsusServerUrl, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX_ID(CLegacyUpdateCtrl, "WsusStatusServerUrl", dispidWsusStatusServerUrl, WsusStatusServerUrl, SetNotSupported, VT_BSTR)
END_DISPATCH_MAP()

// Event map

BEGIN_EVENT_MAP(CLegacyUpdateCtrl, COleControl)
END_EVENT_MAP()

// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CLegacyUpdateCtrl, "LegacyUpdate.Control.1", 0xAD28E0DF, 0x5F5A, 0x40B5, 0x94, 0x32, 0x85, 0xEF, 0xD9, 0x7D, 0x1F, 0x9F)

// Type library ID and version

IMPLEMENT_OLETYPELIB(CLegacyUpdateCtrl, _tlid, _wVerMajor, _wVerMinor)

// Interface IDs

const IID IID_IDispatch               = { 0x00020400, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
const IID IID_ILegacyUpdateCtrl       = { 0x3A81AF5D, 0x35C4, 0x441A, { 0x82, 0xBA, 0x7C, 0xA6, 0x18, 0xF1, 0x12, 0xCE } };
const IID IID_ILegacyUpdateCtrlEvents = { 0xBF62027D, 0xE775, 0x49FD, { 0xA7, 0xE2, 0xA8, 0x7B, 0x08, 0xE9, 0x25, 0xC3 } };

// Control type information

static const DWORD _dwLegacyUpdateOCXOleMisc =
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CLegacyUpdateCtrl, IDS_LEGACYUPDATEOCX, _dwLegacyUpdateOCXOleMisc)

// CLegacyUpdateCtrl::CLegacyUpdateCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CLegacyUpdateCtrl

BOOL CLegacyUpdateCtrl::CLegacyUpdateCtrlFactory::UpdateRegistry(BOOL bRegister) {
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister) {
		return AfxOleRegisterControlClass(AfxGetInstanceHandle(), m_clsid, m_lpszProgID, IDS_LEGACYUPDATEOCX, 0, afxRegApartmentThreading, _dwLegacyUpdateOCXOleMisc, _tlid, _wVerMajor, _wVerMinor);
	} else {
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
	}
}


CLegacyUpdateCtrl::CLegacyUpdateCtrl() {
	InitializeIIDs(&IID_ILegacyUpdateCtrl, &IID_ILegacyUpdateCtrlEvents);
}

CLegacyUpdateCtrl::~CLegacyUpdateCtrl() {
	// TODO: Cleanup your control's instance data here.
}

void CLegacyUpdateCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid) {
}

void CLegacyUpdateCtrl::DoPropExchange(CPropExchange* pPX) {
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
	// TODO: Call PX_ functions for each persistent custom property.
}

void CLegacyUpdateCtrl::OnResetState() {
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	// TODO: Reset any other control state here.
}

// CLegacyUpdateCtrl message handlers

IHTMLDocument2 *CLegacyUpdateCtrl::GetHTMLDocument() {
	IOleClientSite *clientSite = GetClientSite();
	if (clientSite == NULL) {
		return FALSE;
	}

	IOleContainer *container;
	HRESULT result = clientSite->GetContainer(&container);
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
	if (result != S_OK) {
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

VARIANT_BOOL CLegacyUpdateCtrl::CheckControl(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Just return true so the site can confirm the control is working.
	return TRUE;
}

BSTR CLegacyUpdateCtrl::MessageForHresult(LONG inHresult) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!IsPermitted()) {
		return NULL;
	}
	return GetMessageForHresult(inHresult);
}

VARIANT CLegacyUpdateCtrl::GetOSVersionInfo(OSVersionField osField, LONG systemMetric) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VARIANT result;
	VariantInit(&result);

	if (!IsPermitted()) {
		return result;
	}

	result.vt = VT_I4;

	OSVERSIONINFOEX *versionInfo = GetVersionInfo();

	switch (osField) {
	case e_majorVer:
		result.lVal = versionInfo->dwMajorVersion;
		break;

	case e_minorVer:
		result.lVal = versionInfo->dwMinorVersion;
		break;

	case e_buildNumber:
		result.lVal = versionInfo->dwBuildNumber;
		break;

	case e_platform:
		result.lVal = versionInfo->dwPlatformId;
		break;

	case e_SPMajor:
		result.lVal = versionInfo->wServicePackMajor;
		break;

	case e_SPMinor:
		result.lVal = versionInfo->wServicePackMinor;
		break;

	case e_productSuite:
		result.lVal = versionInfo->wSuiteMask;
		break;

	case e_productType:
		result.lVal = versionInfo->wProductType;
		break;

	case e_systemMetric:
		result.lVal = GetSystemMetrics(systemMetric);
		break;

	case e_SPVersionString: {
		LPWSTR data;
		DWORD size;
		HRESULT regResult = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"BuildLab", NULL, &data, &size);
		if (!SUCCEEDED(regResult)) {
			VariantClear(&result);
			return result;
		}
		result.bstrVal = SysAllocStringLen(data, size - 1);
		result.vt = VT_BSTR;
		break;
													}

	case e_controlVersionString: {
		LPWSTR data;
		DWORD size;
		HRESULT verResult = GetOwnVersion(&data, &size);
		if (!SUCCEEDED(verResult)) {
			VariantClear(&result);
			return result;
		}
		result.bstrVal = SysAllocStringLen(data, size - 1);
		result.vt = VT_BSTR;
		break;
															 }

	case e_VistaProductType: {
		DWORD productType;
		GetVistaProductInfo(versionInfo->dwMajorVersion, versionInfo->dwMinorVersion, versionInfo->wServicePackMajor, versionInfo->wServicePackMinor, &productType);
		result.lVal = productType;
		break;
													 }
	}

	return result;
}

IDispatch *CLegacyUpdateCtrl::CreateComObject(BSTR progID) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!IsPermitted() || progID == NULL) {
		return NULL;
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
		return NULL;
	}

	CLSID clsid;
	HRESULT result = CLSIDFromProgID(progID, &clsid);
	if (!SUCCEEDED(result)) {
		TRACE("CreateObject(%ls) failed: %ls\n", progID, MessageForHresult(result));
		return NULL;
	}

	IDispatch *object;
	result = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IDispatch, (void **)&object);
	if (!SUCCEEDED(result)) {
		TRACE("CreateObject(%ls) failed: %ls\n", progID, MessageForHresult(result));
		return NULL;
	}

	return object;
}

CLegacyUpdateCtrl::UserType CLegacyUpdateCtrl::GetUserType(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return IsPermitted() && IsUserAnAdmin() ? e_admin : e_nonAdmin;
}

VARIANT_BOOL CLegacyUpdateCtrl::IsRebootRequired(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!IsPermitted()) {
		return FALSE;
	}

	HKEY subkey;
	HRESULT result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\RebootRequired", 0, KEY_READ, &subkey);
	if (subkey != NULL) {
		RegCloseKey(subkey);
		return TRUE;
	}
	return FALSE;
}

VARIANT_BOOL CLegacyUpdateCtrl::IsWindowsUpdateDisabled(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!IsPermitted()) {
		return TRUE;
	}

	DWORD noWU;
	HRESULT result = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", L"NoWindowsUpdate", NULL, &noWU);
	if (SUCCEEDED(result) && noWU == 1) {
		return TRUE;
	}

	DWORD disableWUAccess;
	result = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\WindowsUpdate", L"DisableWindowsUpdateAccess", NULL, &disableWUAccess);
	if (SUCCEEDED(result) && disableWUAccess == 1) {
		return TRUE;
	}

	return FALSE;
}

void CLegacyUpdateCtrl::RebootIfRequired(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (IsPermitted() && IsRebootRequired()) {
		Reboot();
	}
}

void CLegacyUpdateCtrl::ViewWindowsUpdateLog(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!IsPermitted()) {
		return;
	}

	WCHAR windir[MAX_PATH];
	HRESULT result = SHGetFolderPath(0, CSIDL_WINDOWS, NULL, 0, windir);
	if (!SUCCEEDED(result)) {
		TRACE(L"SHGetFolderPath() failed: %ls\n", GetMessageForHresult(result));
		return;
	}

	// Try Windows Server 2003 Resource Kit (or MSYS/Cygwin/etc) tail.exe, falling back to directly
	// opening the file (most likely in Notepad).
	if ((int)ShellExecute(NULL, L"open", L"tail.exe", L"-f WindowsUpdate.log", windir, SW_SHOWDEFAULT) > 32) {
		return;
	}
	ShellExecute(NULL, L"open", L"WindowsUpdate.log", NULL, windir, SW_SHOWDEFAULT);
}

VARIANT_BOOL CLegacyUpdateCtrl::IsUsingWsusServer(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!IsPermitted()) {
		return FALSE;
	}
	
	DWORD useWUServer;
	HRESULT result = GetRegistryDword(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\AU", L"UseWUServer", NULL, &useWUServer);
	return SUCCEEDED(result) && useWUServer == 1;
}

BSTR CLegacyUpdateCtrl::WsusServerUrl(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!IsPermitted()) {
		return NULL;
	}

	LPWSTR data;
	DWORD size;
	HRESULT result = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", L"WUServer", NULL, &data, &size);
	if (!SUCCEEDED(result)) {
		return NULL;
	}
	return SysAllocStringLen(data, size - 1);
}

BSTR CLegacyUpdateCtrl::WsusStatusServerUrl(void) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!IsPermitted()) {
		return NULL;
	}

	LPWSTR data;
	DWORD size;
	HRESULT result = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate", L"WUStatusServer", NULL, &data, &size);
	if (!SUCCEEDED(result)) {
		return NULL;
	}
	return SysAllocStringLen(data, size - 1);
}
