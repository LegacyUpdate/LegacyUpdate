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

const LPWSTR UpdateSiteURLHttp      = L"http://legacyupdate.net/windowsupdate/v6/";
const LPWSTR UpdateSiteURLHttps     = L"https://legacyupdate.net/windowsupdate/v6/";
const LPWSTR UpdateSiteFirstRunFlag = L"?firstrun=true";

DEFINE_GUID(IID_ILegacyUpdateCtrl,  0xC33085BB, 0xC3E1, 0x4D27, 0xA2, 0x14, 0xAF, 0x01, 0x95, 0x3D, 0xF5, 0xE5);
DEFINE_GUID(CLSID_LegacyUpdateCtrl, 0xAD28E0DF, 0x5F5A, 0x40B5, 0x94, 0x32, 0x85, 0xEF, 0xD9, 0x7D, 0x1F, 0x9F);

static const LPWSTR GetUpdateSiteURL() {
	// Fallback: Use SSL only on Vista and up
	BOOL useHTTPS = AtLeastWinVista();

	// Get the Windows Update website URL set by Legacy Update setup
	LPWSTR data;
	DWORD size;
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

void LaunchUpdateSite(int argc, LPWSTR *argv, int nCmdShow) {
	HRESULT hr = S_OK;
	IWebBrowser2 *browser;
	VARIANT url;
	VARIANT flags;
	VARIANT nullVariant;
	LPTSTR siteURL;
	HMONITOR monitor;

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
	if (hr == REGDB_E_CLASSNOTREG) {
		hr = RegisterServer(0, TRUE, TRUE);
		if (!SUCCEEDED(hr)) {
			goto end;
		}

		hr = CoCreateInstance(&CLSID_LegacyUpdateCtrl, NULL, CLSCTX_LOCAL_SERVER, &IID_ILegacyUpdateCtrl, (void **)&browser);
		if (!SUCCEEDED(hr)) {
			goto end;
		}

		IUnknown_Release(browser);
	} else if (!SUCCEEDED(hr)) {
		goto end;
	}

	// Spawn an IE window via the COM interface. This ensures the page opens in IE (ShellExecute uses
	// default browser), and avoids hardcoding a path to iexplore.exe. Also conveniently allows testing
	// on Windows 11 (iexplore.exe redirects to Edge, but COM still works). Same strategy as used by
	// Wupdmgr.exe and Muweb.dll,LaunchMUSite.
	hr = CoCreateInstance(&CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, &IID_IWebBrowser2, (void **)&browser);
	if (hr == REGDB_E_CLASSNOTREG) {
		// Handle case where the user has uninstalled Internet Explorer using Programs and Features.
		OSVERSIONINFOEX *versionInfo = GetVersionInfo();

		// Windows 8+: Directly prompt to reinstall IE using Fondue.exe.
		if (AtLeastWin8()) {
			SYSTEM_INFO systemInfo;
			GetSystemInfo(&systemInfo);
			LPCTSTR archSuffix = systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? L"amd64" : L"x86";

			WCHAR fondue[MAX_PATH];
			ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\fondue.exe", fondue, ARRAYSIZE(fondue));

			WCHAR fondueArgs[256];
			wsprintf(fondueArgs, L"/enable-feature:Internet-Explorer-Optional-%ls", archSuffix);
			hr = Exec(NULL, fondue, fondueArgs, NULL, SW_SHOWDEFAULT, FALSE, NULL);
			if (SUCCEEDED(hr)) {
				goto end;
			}
		}

		// Tell the user what they need to do, then open the Optional Features dialog.
		WCHAR message[4096];
		LoadString(GetModuleHandle(NULL), IDS_IENOTINSTALLED, message, ARRAYSIZE(message));
		MsgBox(NULL, message, NULL, MB_OK | MB_ICONEXCLAMATION);

		WCHAR optionalFeatures[MAX_PATH];
		ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\OptionalFeatures.exe", optionalFeatures, ARRAYSIZE(optionalFeatures));
		Exec(NULL, optionalFeatures, NULL, NULL, SW_SHOWDEFAULT, FALSE, NULL);
		hr = S_OK;
		goto end;
	} else if (!SUCCEEDED(hr)) {
		goto end;
	}

	// Can we connect with https? WinInet will throw an error if not.
	siteURL = GetUpdateSiteURL();

	// Is this a first run launch? Append first run flag if so.
	if (argc > 0 && lstrcmpi(argv[0], L"/firstrun") == 0) {
		WCHAR newSiteURL[256];
		wsprintf(newSiteURL, L"%s%s", siteURL, UpdateSiteFirstRunFlag);
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
		goto end;
	}

	HWND ieHwnd;
	hr = IWebBrowser2_get_HWND(browser, (SHANDLE_PTR *)&ieHwnd);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	// Are we on a small display? If so, resize and maximise the window.
	monitor = MonitorFromWindow(ieHwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);

	if (GetMonitorInfo(monitor, &monitorInfo) > 0) {
		LONG workWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
		LONG workHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		LONG width, height;
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

		LONG left, top;
		IWebBrowser2_get_Left(browser, &left);
		IWebBrowser2_get_Top(browser, &top);

		if (left + width > workWidth) {
			IWebBrowser2_put_Left(browser, 0);
		}
		if (top + height > workHeight) {
			IWebBrowser2_put_Top(browser, 0);
		}

		if (workWidth <= 1152) {
			ShowWindow(ieHwnd, SW_MAXIMIZE);
		}
	}

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
