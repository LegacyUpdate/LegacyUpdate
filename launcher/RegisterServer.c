#include <windows.h>
#include "Exec.h"
#include "HResult.h"
#include "LegacyUpdate.h"
#include "MsgBox.h"
#include "SelfElevate.h"
#include "User.h"
#include "VersionInfo.h"
#include "Wow64.h"

static HRESULT RegisterDllInternal(LPWSTR path, BOOL state) {
	HMODULE module = LoadLibrary(path);
	if (!module) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	HRESULT hr = S_OK;
	FARPROC proc = GetProcAddress(module, state ? "DllRegisterServer" : "DllUnregisterServer");
	if (!proc) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto end;
	}

	hr = ((HRESULT (WINAPI *)())proc)();

end:
	if (module) {
		FreeLibrary(module);
	}

	return hr;
}

#if _WIN64
static HRESULT RegisterDllExternal(LPWSTR path, BOOL state) {
	WCHAR regsvr32[MAX_PATH];
	ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\regsvr32.exe", regsvr32, ARRAYSIZE(regsvr32));

	LPWSTR args = (LPWSTR)LocalAlloc(LPTR, (lstrlen(path) + 6) * sizeof(WCHAR));
	wsprintf(args, L"/s %ls\"%ls\"", state ? L"" : L"/u ", path);

	DWORD status;
	HRESULT hr = Exec(NULL, regsvr32, args, NULL, SW_HIDE, TRUE, &status);
	if (!SUCCEEDED(hr)) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	if (status != 0) {
		// Run again without /s, so the user can see the error
		wsprintf(args, L"%ls\"%ls\"", state ? L"" : L"/u ", path);
		hr = Exec(NULL, regsvr32, args, NULL, SW_SHOWDEFAULT, TRUE, &status);
	}

	return status == 0 ? S_OK : E_FAIL;
}
#endif

HRESULT RegisterServer(HWND hwnd, BOOL state, BOOL forLaunch) {
	// Ensure elevation
	HRESULT hr = S_OK;
	LPWSTR installPath;
	LPWSTR dllPath;

	if (!IsUserAdmin()) {
		LPWSTR args = (LPWSTR)LocalAlloc(LPTR, 512 * sizeof(WCHAR));
		wsprintf(args, L"%ls %i", state ? L"/regserver" : L"/unregserver", hwnd);
		DWORD code;
		hr = SelfElevate(args, &code);
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

#if _WIN64
	PVOID oldValue;
	DisableWow64FsRedirection(&oldValue);
#endif

	hr = GetInstallPath(&installPath);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	dllPath = (LPWSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(WCHAR));
	wsprintf(dllPath, L"%ls\\LegacyUpdate.dll", installPath);

	hr = RegisterDllInternal(dllPath, state);

#if _WIN64
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	wsprintf(dllPath, L"%ls\\LegacyUpdate32.dll", installPath);
	hr = RegisterDllExternal(dllPath, state);

	RevertWow64FsRedirection(oldValue);
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
		MsgBox(hwnd, title, GetMessageForHresult(hr), MB_ICONERROR);
	}

	if (!forLaunch) {
		PostQuitMessage(hr);
	}

	return hr;
}
