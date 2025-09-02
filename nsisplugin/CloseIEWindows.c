#include <windows.h>
#include <nsis/pluginapi.h>
#include <exdisp.h>
#include <tlhelp32.h>

const LPWSTR LegacyUpdateSiteURLHttp  = L"http://legacyupdate.net/";
const LPWSTR LegacyUpdateSiteURLHttps = L"https://legacyupdate.net/";

PLUGIN_METHOD(CloseIEWindows) {
	PLUGIN_INIT();

	// Find and close IE windows that might have the ActiveX control loaded
	IShellWindows *windows;
	HRESULT hr = CoCreateInstance(&CLSID_ShellWindows, NULL, CLSCTX_ALL, &IID_IShellWindows, (void **)&windows);
	CHECK_HR_OR_GOTO_END(L"CoCreateInstance");

	long count = 0;
	hr = IShellWindows_get_Count(windows, &count);
	CHECK_HR_OR_GOTO_END(L"get_Count");

	VARIANT index = {0};
	index.vt = VT_I4;

	for (long i = 0; i <= count; i++) {
		IDispatch *item = NULL;
		index.lVal = i;

		hr = IShellWindows_Item(windows, index, &item);
		if (!SUCCEEDED(hr) || !item) {
			continue;
		}

		IWebBrowser2 *browser = NULL;
		hr = IDispatch_QueryInterface(item, &IID_IWebBrowser2, (void **)&browser);
		IDispatch_Release(item);
		if (!SUCCEEDED(hr)) {
			continue;
		}

		BSTR location = NULL;
		hr = IWebBrowser2_get_LocationURL(browser, &location);
		if (!SUCCEEDED(hr)) {
			IWebBrowser2_Release(browser);
			continue;
		}

		if (wcsstr(location, LegacyUpdateSiteURLHttp) != NULL || wcsstr(location, LegacyUpdateSiteURLHttps) != NULL) {
			hr = IWebBrowser2_Quit(browser);
			if (!SUCCEEDED(hr)) {
				TRACE(L"Quit failed: %08x", hr);
			}

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

	// Find and exit all dllhosts
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		goto end;
	}

	PROCESSENTRY32W entry = {0};
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szExeFile, L"dllhost.exe") != 0) {
				continue;
			}

			// Loop over loaded modules to determine if ours is loaded
			HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
			if (process) {
				HANDLE module = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, entry.th32ProcessID);
				if (module == INVALID_HANDLE_VALUE) {
					CloseHandle(process);
					continue;
				}

				MODULEENTRY32W moduleEntry = {0};
				moduleEntry.dwSize = sizeof(MODULEENTRY32W);

				if (Module32FirstW(module, &moduleEntry)) {
					do {
						LPWSTR dllName = wcsrchr(moduleEntry.szExePath, L'\\');
						if (!dllName) {
							continue;
						}

						dllName += 1;
						if (_wcsicmp(dllName, L"LegacyUpdate.dll") == 0 || _wcsicmp(dllName, L"LegacyUpdate32.dll") == 0) {
							TerminateProcess(process, 0);

							// Wait up to 5 seconds for the dllhost to close
							DWORD start = GetTickCount();
							while (GetTickCount() - start < 5000) {
								Sleep(100);

								DWORD exitCode = 0;
								if (GetExitCodeProcess(process, &exitCode) && exitCode != STILL_ACTIVE) {
									break;
								}
							}

							break;
						}
					} while (Module32NextW(module, &moduleEntry));
				}

				CloseHandle(module);
				CloseHandle(process);
			}
		} while (Process32NextW(snapshot, &entry));
	}

	CloseHandle(snapshot);

end:
	if (windows) {
		IShellWindows_Release(windows);
	}

	pushint(hr);
}
