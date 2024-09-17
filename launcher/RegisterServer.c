#include <windows.h>
#include <shlobj.h>
#include "Exec.h"
#include "LegacyUpdate.h"
#include "MsgBox.h"
#include "SelfElevate.h"
#include "User.h"
#include "VersionInfo.h"
#include "Wow64.h"

HRESULT RegisterDll(LPWSTR path, BOOL state) {
	WCHAR windir[MAX_PATH];
	HRESULT hr = SHGetFolderPath(0, CSIDL_WINDOWS, NULL, 0, windir);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	LPWSTR system32 = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
	wsprintf(system32, L"%ls\\System32", windir);

	LPWSTR args = (LPWSTR)LocalAlloc(LPTR, (lstrlen(path) + 6) * sizeof(WCHAR));
	wsprintf(args, L"/s %ls\"%ls\"", state ? L"" : L"/u ", path);

	DWORD status;
	hr = Exec(NULL, L"regsvr32.exe", args, system32, SW_HIDE, TRUE, &status);
	if (!SUCCEEDED(hr)) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	if (status != 0) {
		// Run again without /s, so the user can see the error
		wsprintf(args, L"%ls\"%ls\"", state ? L"" : L"/u ", path);
		hr = Exec(NULL, L"regsvr32.exe", args, system32, SW_SHOWDEFAULT, TRUE, &status);
	}

	return status == 0 ? S_OK : E_FAIL;
}

void RegisterServer(BOOL state) {
	// Ensure elevation
	HRESULT hr = S_OK;
	LPWSTR installPath;
	LPWSTR dllPath;

	if (!IsUserAdmin()) {
		DWORD code;
		hr = SelfElevate(state ? L"/regserver" : L"/unregserver", &code);
		if (!SUCCEEDED(hr)) {
			// Ignore error on cancelling UAC dialog
			if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
				hr = S_OK;
			}
			goto end;
		}

		hr = HRESULT_FROM_WIN32(code);
		goto end;
	}

	hr = GetInstallPath(&installPath);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	dllPath = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
	wsprintf(dllPath, L"%ls\\LegacyUpdate.dll", installPath);

	hr = RegisterDll(dllPath, state);

#if _WIN64
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	wsprintf(dllPath, L"%ls\\LegacyUpdate32.dll", installPath);
	hr = RegisterDll(dllPath, state);
#endif

end:
	if (installPath) {
		LocalFree(installPath);
	}

	if (dllPath) {
		LocalFree(dllPath);
	}

	if (!SUCCEEDED(hr)) {
		LPWSTR title = state
			? L"Failed to register Legacy Update ActiveX control"
			: L"Failed to unregister Legacy Update ActiveX control";
		// TODO: Fix GetMessageForHresult()
		LPWSTR text;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&text, 0, NULL);
		MsgBox(NULL, title, /*GetMessageForHresult(hr)*/ text, MB_OK | MB_ICONERROR);
	}

	PostQuitMessage(hr);
}
