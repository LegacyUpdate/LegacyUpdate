#include "stdafx.h"
#include <comdef.h>
#include <ExDisp.h>
#include <strsafe.h>
#include "Utils.h"

const LPWSTR UpdateSiteURL = L"http://legacyupdate.net/windowsupdate/v6/";

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
	// default browser), and avoids hardcoding a path to iexplore.exe. Same strategy as used by
	// Wupdmgr.exe and Muweb.dll,LaunchMUSite.
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

	browser->put_Visible(TRUE);
	browser->Release();

end:
	if (!SUCCEEDED(result)) {
		MessageBox(NULL, GetMessageForHresult(result), L"Legacy Update", MB_OK | MB_ICONEXCLAMATION);
	}

	CoUninitialize();

	if (!SUCCEEDED(result)) {
		exit(1);
	}
}
