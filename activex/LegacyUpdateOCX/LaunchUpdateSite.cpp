#include "stdafx.h"
#include <comdef.h>
#include <ExDisp.h>
#include "Utils.h"

const LPWSTR UpdateSiteURL = L"http://legacyupdate.net/windowsupdate/v6/";

// Function signature required by Rundll32.exe.
void CALLBACK LaunchUpdateSite(HWND hwnd, HINSTANCE hinstance, LPSTR lpszCmdLine, int nCmdShow) {
	// Spawn an IE window via the COM interface. This ensures the page opens in IE (ShellExecute uses
	// default browser), and avoids hardcoding a path to iexplore.exe. Same strategy as used by
	// Wupdmgr.exe and Muweb.dll,LaunchMUSite.
	HRESULT result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	IWebBrowser2 *browser;
	result = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void **)&browser);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	VARIANT url;
	VariantInit(&url);
	url.vt = VT_BSTR;
	url.bstrVal = SysAllocString(UpdateSiteURL);

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

	// IE window isn't visible by default, so we need to make it visible and foreground.
	HWND browserHwnd;
	result = browser->get_HWND((SHANDLE_PTR *)&browserHwnd);
	if (!SUCCEEDED(result)) {
		goto end;
	}

	SetForegroundWindow(browserHwnd);
	browser->put_Visible(TRUE);
	browser->Release();

end:
	if (!SUCCEEDED(result)) {
		MessageBox(hwnd, GetMessageForHresult(result), L"Legacy Update", MB_OK | MB_ICONEXCLAMATION);
	}

	CoUninitialize();
}