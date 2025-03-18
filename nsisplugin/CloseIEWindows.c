#include <windows.h>
#include <nsis/pluginapi.h>
#include <exdisp.h>

const LPWSTR LegacyUpdateSiteURLHttp  = L"http://legacyupdate.net/";
const LPWSTR LegacyUpdateSiteURLHttps = L"https://legacyupdate.net/";

PLUGIN_METHOD(CloseIEWindows) {
	PLUGIN_INIT();

	// Find and close IE windows that might have the ActiveX control loaded
	IShellWindows *windows;
	HRESULT hr = CoCreateInstance(&CLSID_ShellWindows, NULL, CLSCTX_ALL, &IID_IShellWindows, (void **)&windows);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	long count;
	hr = IShellWindows_get_Count(windows, &count);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	VARIANT index = {0};
	index.vt = VT_I4;

	for (long i = 0; i <= count; i++) {
		IDispatch *item;
		index.lVal = i;

		hr = IShellWindows_Item(windows, index, &item);
		if (!SUCCEEDED(hr) || !item) {
			continue;
		}

		IWebBrowser2 *browser;
		hr = IDispatch_QueryInterface(item, &IID_IWebBrowser2, (void **)&browser);
		IDispatch_Release(item);
		if (!SUCCEEDED(hr)) {
			continue;
		}

		BSTR location;
		hr = IWebBrowser2_get_LocationURL(browser, &location);
		if (!SUCCEEDED(hr)) {
			IWebBrowser2_Release(browser);
			continue;
		}

		if (wcsstr(location, LegacyUpdateSiteURLHttp) != NULL || wcsstr(location, LegacyUpdateSiteURLHttps) != NULL) {
			hr = IWebBrowser2_Quit(browser);

			// Wait up to 5 seconds for the window to close
			HWND hwnd = 0;
			hr = IWebBrowser2_get_HWND(browser, (SHANDLE_PTR *)&hwnd);
			if (SUCCEEDED(hr) && hwnd != 0) {
				DWORD start = GetTickCount();
				while (IsWindow(hwnd) && GetTickCount() - start < 5000) {
					Sleep(100);
				}
			}
		}

		SysFreeString(location);
		IWebBrowser2_Release(browser);
	}

end:
	if (windows) {
		IShellWindows_Release(windows);
	}

	pushint(hr);
}
