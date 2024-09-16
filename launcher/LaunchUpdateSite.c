#include "stdafx.h"
#include "main.h"
#include "resource.h"
#include <exdisp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <wchar.h>
#include "HResult.h"
#include "Registry.h"
#include "User.h"
#include "VersionInfo.h"
#include "MsgBox.h"

const LPTSTR UpdateSiteURLHttp      = L"http://legacyupdate.net/windowsupdate/v6/";
const LPTSTR UpdateSiteURLHttps     = L"https://legacyupdate.net/windowsupdate/v6/";
const LPTSTR UpdateSiteFirstRunFlag = L"?firstrun=true";

static BOOL CanUseSSLConnection() {
	// Get the Windows Update website URL set by Legacy Update setup
	LPWSTR data;
	DWORD size;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate", L"URL", KEY_WOW64_64KEY, &data, &size);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	// Return based on the URL value
	if (wcscmp(data, UpdateSiteURLHttps) == 0) {
		return TRUE;
	} else if (wcscmp(data, UpdateSiteURLHttp) == 0) {
		return FALSE;
	}

end:
	// Fallback: Use SSL only on Vista and up
	OSVERSIONINFOEX *versionInfo = GetVersionInfo();
	return IsOSVersionOrLater(6, 0);
}

void LaunchUpdateSite(int argc, LPWSTR *argv, int nCmdShow) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	IWebBrowser2 *browser;
	VARIANT url;
	VARIANT flags;
	VARIANT nullVariant;
	LPTSTR siteURL;
	HMONITOR monitor;

	if (!SUCCEEDED(hr)) {
		goto end;
	}

	// If running on 2k/XP, make sure we're elevated. If not, show Run As prompt.
	if (!IsOSVersionOrLater(6, 0) && !IsUserAdmin()) {
		LPWSTR filename;
		DWORD filenameSize;
		GetOwnFileName(&filename, &filenameSize);

		INT_PTR execResult = (INT_PTR)ShellExecute(NULL, L"runas", filename, GetCommandLineW(), NULL, nCmdShow);

		// Access denied happens when the user clicks No/Cancel.
		if (execResult <= 32 && execResult != SE_ERR_ACCESSDENIED) {
			hr = HRESULT_FROM_WIN32(GetLastError());
		}
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
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		LPCTSTR archSuffix = systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? L"amd64" : L"x86";

		WCHAR fondueArgs[256];
		wsprintf(fondueArgs, L"/enable-feature:Internet-Explorer-Optional-%ls", archSuffix);
		INT_PTR execResult = (INT_PTR)ShellExecute(NULL, L"open", L"fondue.exe", fondueArgs, NULL, SW_SHOWDEFAULT);

		if (execResult <= 32) {
			// Tell the user what they need to do, then open the Optional Features dialog.
			WCHAR message[4096];
			LoadString(g_hInstance, IDS_IENOTINSTALLED, message, ARRAYSIZE(message));
			MsgBox(NULL, message, NULL, MB_OK | MB_ICONEXCLAMATION);
			ShellExecute(NULL, L"open", L"OptionalFeatures.exe", NULL, NULL, SW_SHOWDEFAULT);
		}
		hr = S_OK;
		goto end;
	} else if (!SUCCEEDED(hr)) {
		goto end;
	}

	// Can we connect with https? WinInet will throw an error if not.
	siteURL = CanUseSSLConnection() ? UpdateSiteURLHttps : UpdateSiteURLHttp;

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
		MsgBox(NULL, NULL, L"", MB_ICONEXCLAMATION);
	}

	browser = NULL;
	CoUninitialize();
	PostQuitMessage(0);
}
