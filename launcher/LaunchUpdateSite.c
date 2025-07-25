#include "stdafx.h"
#include "main.h"
#include "resource.h"
#include <exdisp.h>
#include "Exec.h"
#include "HResult.h"
#include "MsgBox.h"
#include "RegisterServer.h"
#include "Registry.h"
#include "SelfElevate.h"
#include "User.h"
#include "VersionInfo.h"
#include "Wow64.h"

const LPWSTR UpdateSiteURLHttp      = L"http://legacyupdate.net/windowsupdate/v6/";
const LPWSTR UpdateSiteURLHttps     = L"https://legacyupdate.net/windowsupdate/v6/";
const LPWSTR UpdateSiteFirstRunFlag = L"?firstrun=true";

DEFINE_GUID(IID_ILegacyUpdateCtrl,  0xC33085BB, 0xC3E1, 0x4D27, 0xA2, 0x14, 0xAF, 0x01, 0x95, 0x3D, 0xF5, 0xE5);
DEFINE_GUID(CLSID_LegacyUpdateCtrl, 0xAD28E0DF, 0x5F5A, 0x40B5, 0x94, 0x32, 0x85, 0xEF, 0xD9, 0x7D, 0x1F, 0x9F);

static LPWSTR GetUpdateSiteURL() {
	// Fallback: Use SSL only on Vista and up
	BOOL useHTTPS = AtLeastWinVista();

	// Get the Windows Update website URL set by Legacy Update setup
	LPWSTR data = NULL;
	DWORD size = 0;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate", L"URL", KEY_WOW64_64KEY, &data, &size);
	if (SUCCEEDED(hr)) {
		// Return based on the URL value
		if (wcscmp(data, UpdateSiteURLHttps) == 0) {
			useHTTPS = TRUE;
		} else if (wcscmp(data, UpdateSiteURLHttp) == 0) {
			useHTTPS = FALSE;
		}
		LocalFree(data);
	}

	return useHTTPS ? UpdateSiteURLHttps : UpdateSiteURLHttp;
}

HRESULT HandleIENotInstalled() {
	// Handle case where the user has uninstalled Internet Explorer using Programs and Features.
	if (AtLeastWin8() && !AtLeastWin10_1703()) {
		// Windows 8 - 10 1607: Directly prompt to reinstall IE using Fondue.exe.
		SYSTEM_INFO systemInfo = {0};
		OurGetNativeSystemInfo(&systemInfo);
		LPCTSTR archSuffix = systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? L"amd64" : L"x86";

		WCHAR fondue[MAX_PATH];
		ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\fondue.exe", fondue, ARRAYSIZE(fondue));

		WCHAR fondueArgs[256];
		wsprintf(fondueArgs, L"/enable-feature:Internet-Explorer-Optional-%ls", archSuffix);

		HRESULT hr = Exec(NULL, fondue, fondueArgs, NULL, SW_SHOWDEFAULT, FALSE, NULL);
		if (SUCCEEDED(hr)) {
			return S_OK;
		}
	}

	// Tell the user what they need to do, then open the Optional Features dialog.
	WCHAR message[4096];
	LoadString(GetModuleHandle(NULL), IDS_IENOTINSTALLED, message, ARRAYSIZE(message));
	MsgBox(NULL, message, NULL, MB_OK | MB_ICONEXCLAMATION);

	if (AtLeastWin10_1703()) {
		// Windows 10 1703+: IE is moved to a WindowsCapability, handled by the Settings app.
		Exec(NULL, L"ms-settings:optionalfeatures", NULL, NULL, SW_SHOWDEFAULT, FALSE, NULL);
	} else if (AtLeastWin7()) {
		// Windows 7: Optional Features dialog
		WCHAR optionalFeatures[MAX_PATH];
		ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\OptionalFeatures.exe", optionalFeatures, ARRAYSIZE(optionalFeatures));
		Exec(NULL, optionalFeatures, NULL, NULL, SW_SHOWDEFAULT, FALSE, NULL);
	}

	return S_OK;
}

void LaunchUpdateSite(int argc, LPWSTR *argv, int nCmdShow) {
	HRESULT hr = S_OK;
	IWebBrowser2 *browser = NULL;
	VARIANT url = {0};
	VARIANT flags = {0};
	VARIANT nullVariant = {0};
	LPTSTR siteURL = NULL;
	HMONITOR monitor = NULL;

	// If running on 2k/XP, make sure we're elevated. If not, show Run As prompt.
	if (!AtLeastWinVista() && !IsUserAdmin()) {
		LPWSTR args = (LPWSTR)LocalAlloc(LPTR, 512 * sizeof(WCHAR));
		wsprintf(args, L"/launch %ls", argc > 0 ? argv[0] : L"");
		hr = SelfElevate(args, NULL);

		// Access denied happens when the user clicks No/Cancel.
		if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
			hr = S_OK;
		}

		goto end;
	}

	// Can we instantiate our own ActiveX control? If not, try to register it.
	hr = CoCreateInstance(&CLSID_LegacyUpdateCtrl, NULL, CLSCTX_LOCAL_SERVER, &IID_ILegacyUpdateCtrl, (void **)&browser);
	if (hr == REGDB_E_CLASSNOTREG || hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) {
		TRACE(L"LegacyUpdateCtrl not registered");
		hr = RegisterServer(0, TRUE, TRUE);
		if (!SUCCEEDED(hr)) {
			goto end;
		}

		hr = CoCreateInstance(&CLSID_LegacyUpdateCtrl, NULL, CLSCTX_LOCAL_SERVER, &IID_ILegacyUpdateCtrl, (void **)&browser);
		if (!SUCCEEDED(hr)) {
			TRACE(L"Still failed to load LegacyUpdateCtrl");
			goto end;
		}

		IUnknown_Release(browser);
	} else if (!SUCCEEDED(hr)) {
		TRACE(L"Create ILegacyUpdateCtrl failed (%8x)", hr);
		goto end;
	}

	// Spawn an IE window via the COM interface. This ensures the page opens in IE (ShellExecute uses
	// default browser), and avoids hardcoding a path to iexplore.exe. Also conveniently allows testing
	// on Windows 11 (iexplore.exe redirects to Edge, but COM still works). Same strategy as used by
	// Wupdmgr.exe and Muweb.dll,LaunchMUSite.
	hr = CoCreateInstance(&CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, &IID_IWebBrowser2, (void **)&browser);

	// An install of IE can be "broken" in two ways:
	//  - Class not registered: mshtml.dll unregistered, deleted, or uninstalled in Optional Features.
	//  - Path not found: iexplore.exe is not present.
	if (hr == REGDB_E_CLASSNOTREG || hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) {
		TRACE(L"IE not installed (%8x)", hr);
		hr = HandleIENotInstalled();
		goto end;
	} else if (!SUCCEEDED(hr)) {
		TRACE(L"Create IWebBrowser2 failed (%8x)", hr);
		goto end;
	}

	// Get the URL we want to launch
	siteURL = GetUpdateSiteURL();

	// Is this a first run launch? Append first run flag if so.
	if (argc > 0 && lstrcmpi(argv[0], L"/firstrun") == 0) {
		WCHAR newSiteURL[256];
		wsprintf(newSiteURL, L"%ls%ls", siteURL, UpdateSiteFirstRunFlag);
		siteURL = newSiteURL;
	}

	VariantInit(&url);
	url.vt = VT_BSTR;
	url.bstrVal = SysAllocString(siteURL);

	VariantInit(&flags);
	flags.vt = VT_I4;
	flags.lVal = 0;

	VariantInit(&nullVariant);

	hr = IWebBrowser2_Navigate2(browser, &url, &flags, &nullVariant, &nullVariant, &nullVariant);
	if (!SUCCEEDED(hr)) {
		TRACE(L"Navigate2 failed (%8x)", hr);
		goto end;
	}

	HWND ieHwnd = NULL;
	hr = IWebBrowser2_get_HWND(browser, (SHANDLE_PTR *)&ieHwnd);
	if (!SUCCEEDED(hr)) {
		TRACE(L"get_HWND failed (%8x)", hr);
		goto end;
	}

	// Pass through our nCmdShow flag
	ShowWindow(ieHwnd, nCmdShow);

	// Are we on a small display? If so, resize and maximise the window.
	monitor = MonitorFromWindow(ieHwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo = {0};
	monitorInfo.cbSize = sizeof(MONITORINFO);

	if (GetMonitorInfo(monitor, &monitorInfo) > 0) {
		LONG workWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
		LONG workHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		LONG width = 0, height = 0;
		IWebBrowser2_get_Width(browser, &width);
		IWebBrowser2_get_Height(browser, &height);

		if (width < 800) {
			width = workWidth < 800 ? workWidth : 800;
			IWebBrowser2_put_Width(browser, width);
		}
		if (height < 600) {
			height = workHeight < 600 ? workHeight : 600;
			IWebBrowser2_put_Height(browser, height);
		}

		LONG left = 0, top = 0;
		IWebBrowser2_get_Left(browser, &left);
		IWebBrowser2_get_Top(browser, &top);

		if (left + width > workWidth) {
			IWebBrowser2_put_Left(browser, 0);
		}
		if (top + height > workHeight) {
			IWebBrowser2_put_Top(browser, 0);
		}

		// Maximize if below 1152x864, if not already overridden by user
		if (workWidth <= 1152 && nCmdShow == SW_SHOWDEFAULT) {
			ShowWindow(ieHwnd, SW_MAXIMIZE);
		}
	}

	// IE window won't be fully initialized until we make it visible.
	IWebBrowser2_put_Visible(browser, TRUE);

	// Focus the window, since it seems to not always get focus as it should.
	SetForegroundWindow(ieHwnd);

end:
	if (!SUCCEEDED(hr)) {
		MsgBox(NULL, GetMessageForHresult(hr), NULL, MB_ICONEXCLAMATION);
	}

	browser = NULL;
	CoUninitialize();
	PostQuitMessage(0);
}
