#include "stdafx.h"
#include <comdef.h>
#include <ShellAPI.h>
#include <ShlObj.h>
#include <ExDisp.h>
#include <strsafe.h>
#include "Utils.h"

const LPCSTR UpdateSiteHostname     = "legacyupdate.net";
const LPWSTR UpdateSiteURLHttp      = L"http://legacyupdate.net/windowsupdate/v6/";
const LPWSTR UpdateSiteURLHttps     = L"https://legacyupdate.net/windowsupdate/v6/";
const LPWSTR UpdateSiteFirstRunFlag = L"?firstrun=true";

static BOOL CanUseSSLConnection() {
	// Get the Windows Update website URL set by Legacy Update setup
	LPWSTR data;
	DWORD size;
	HRESULT result = GetRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate", L"URL", NULL, &data, &size);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	// Return based on the URL value
	if (StrCmpW(data, UpdateSiteURLHttps) == 0) {
		return TRUE;
	} else if (StrCmpW(data, UpdateSiteURLHttp) == 0) {
		return FALSE;
	}

end:
	// Fallback: Use SSL only on Vista and up
	OSVERSIONINFOEX *versionInfo = GetVersionInfo();
	return versionInfo->dwMajorVersion > 5;
}

// Function signature required by Rundll32.exe.
void CALLBACK LaunchUpdateSite(HWND hwnd, HINSTANCE hinstance, LPSTR lpszCmdLine, int nCmdShow) {
	HRESULT result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	// Make sure we're elevated. If not, show UAC prompt (Vista+) or Run As prompt (XP/2k).
	if (!IsUserAnAdmin()) {
		LPWSTR filename;
		DWORD filenameSize;
		GetOwnFileName(&filename, &filenameSize);
		WCHAR args[MAX_PATH + 20];
		StringCchPrintfW(args, filenameSize + 20, L"\"%ls\",LaunchUpdateSite", filename);

		// Ignore C4311 and C4302, which is for typecasts. It is due to ShellExec and should be safe to bypass.
		#pragma warning(disable: 4311 4302)
		int execResult = (int)ShellExecute(NULL, L"runas", L"rundll32.exe", args, NULL, SW_SHOWDEFAULT);
		#pragma warning(default: 4311 4302)

		// Access denied happens when the user clicks No/Cancel.
		if (execResult <= 32 && execResult != SE_ERR_ACCESSDENIED) {
			result = E_FAIL;
		}
		goto end;
	}

	// Spawn an IE window via the COM interface. This ensures the page opens in IE (ShellExecute uses
	// default browser), and avoids hardcoding a path to iexplore.exe. Also conveniently allows testing
	// on Windows 11 (iexplore.exe redirects to Edge, but COM still works). Same strategy as used by
	// Wupdmgr.exe and Muweb.dll,LaunchMUSite.
	IWebBrowser2 *browser;
	result = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void **)&browser);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	// Can we connect with https? WinInet will throw an error if not.
	LPWSTR siteURL = CanUseSSLConnection() ? UpdateSiteURLHttps : UpdateSiteURLHttp;

	// Is this a first run launch? Append first run flag if so.
	if (strcmp(lpszCmdLine, "firstrun") == 0) {
		WCHAR newSiteURL[256];
		StringCchPrintfW(newSiteURL, 256, L"%s%s", siteURL, UpdateSiteFirstRunFlag);
		siteURL = newSiteURL;
	}

	VARIANT url;
	VariantInit(&url);
	url.vt = VT_BSTR;
	url.bstrVal = SysAllocString(siteURL);

	VARIANT flags;
	VariantInit(&flags);
	flags.vt = VT_I4;
	flags.lVal = 0;

	VARIANT nullVariant;
	VariantInit(&nullVariant);

	result = browser->Navigate2(&url, &flags, &nullVariant, &nullVariant, &nullVariant);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	HWND ieHwnd;
	result = browser->get_HWND((SHANDLE_PTR *)&ieHwnd);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	// Are we on a small display? If so, resize and maximise the window.
	HMONITOR monitor = MonitorFromWindow(ieHwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);

	if (GetMonitorInfo(monitor, &monitorInfo) > 0) {
		LONG workWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
		LONG workHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		LONG width, height;
		browser->get_Width(&width);
		browser->get_Height(&height);

		if (width < 800) {
			width = min(800, workWidth);
			browser->put_Width(width);
		}
		if (height < 600) {
			height = min(600, workHeight);
			browser->put_Height(height);
		}

		LONG left, top;
		browser->get_Left(&left);
		browser->get_Top(&top);

		if (left + width > workWidth) {
			browser->put_Left(0);
		}
		if (top + height > workHeight) {
			browser->put_Top(0);
		}

		if (workWidth <= 1152) {
			ShowWindow(ieHwnd, SW_MAXIMIZE);
		}
	}

	browser->put_Visible(TRUE);
	browser->Release();

	// Focus the window, since it seems to not always get focus as it should.
	SetForegroundWindow(ieHwnd);

end:
	if (!SUCCEEDED(result)) {
		MessageBox(NULL, GetMessageForHresult(result), L"Legacy Update", MB_OK | MB_ICONEXCLAMATION);
	}

	CoUninitialize();
}
