#include "stdafx.h"
#include <comdef.h>
#include <ExDisp.h>
#include <strsafe.h>
#include "Utils.h"
#include <WinInet.h>

#pragma comment(lib, "wininet.lib")

const LPCSTR UpdateSiteHostname    = "legacyupdate.net";
const LPWSTR UpdateSiteURLHttp     = L"http://legacyupdate.net/windowsupdate/v6/";
const LPWSTR UpdateSiteURLHttps    = L"https://legacyupdate.net/windowsupdate/v6/";
const LPWSTR UpdateSitePingTestURL = L"https://legacyupdate.net/v6/ClientWebService/ping.bin";

static HRESULT AttemptSSLConnection() {
	// We know it won't work prior to XP SP3, so just fail immediately on XP RTM-SP2 and any Win2k.
	OSVERSIONINFOEX* versionInfo = GetVersionInfo();
	if (versionInfo->dwMajorVersion == 5) {
		switch (versionInfo->dwMinorVersion) {
		case 0:
			return E_FAIL;

		case 1:
			if (versionInfo->wServicePackMajor < 3) {
				return E_FAIL;
			}
		}
	}

	HINTERNET internet, request;
	LPWSTR version;
	DWORD size;
	HRESULT result = GetOwnVersion(&version, &size);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	WCHAR userAgent[1024];
	StringCchPrintfW(userAgent, 1024, L"Mozilla/4.0 (Legacy Update %ls; Windows NT %d.%d SP%d)",
		version,
		versionInfo->dwMajorVersion,
		versionInfo->dwMinorVersion,
		versionInfo->wServicePackMajor);

	DWORD connectResult = InternetAttemptConnect(0);
	if (connectResult != ERROR_SUCCESS) {
		result = HRESULT_FROM_WIN32(connectResult);
		goto end;
	}

	internet = InternetOpen(userAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (internet == NULL) {
		result = HRESULT_FROM_WIN32(GetLastError());
		goto end;
	}

	request = InternetOpenUrl(internet, UpdateSitePingTestURL, NULL, 0, 0, NULL);
	if (request == NULL) {
		result = HRESULT_FROM_WIN32(GetLastError());
		goto end;
	}

end:
	if (request != NULL) {
		HttpEndRequest(request, NULL, 0, 0);
		InternetCloseHandle(request);
	}
	if (internet != NULL) {
		InternetCloseHandle(internet);
	}
	return result;
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
		int execResult = (int)ShellExecute(NULL, L"runas", L"rundll32.exe", args, NULL, SW_SHOWDEFAULT);
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
	result = AttemptSSLConnection();
	LPWSTR siteURL = SUCCEEDED(result) ? UpdateSiteURLHttps : UpdateSiteURLHttp;

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
