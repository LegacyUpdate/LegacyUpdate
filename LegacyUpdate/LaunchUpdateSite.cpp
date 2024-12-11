#include "stdafx.h"
#include "Exec.h"
#include "HResult.h"
#include "LegacyUpdate.h"
#include <shellapi.h>

// Function signature required by Rundll32.exe.
void CALLBACK LaunchUpdateSite(HWND hwnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	LPWSTR path;

	if (!SUCCEEDED(hr)) {
		goto end;
	}

	// This just calls LegacyUpdate.exe now for backwards compatibility.
	hr = GetInstallPath(&path);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	PathAppend(path, L"LegacyUpdate.exe");

	DWORD code;
	hr = Exec(L"open", path, NULL, NULL, nCmdShow, TRUE, &code);
	if (SUCCEEDED(hr)) {
		hr = HRESULT_FROM_WIN32(code);
	}

end:
	if (!SUCCEEDED(hr)) {
		MessageBox(NULL, GetMessageForHresult(hr), L"Legacy Update", MB_OK | MB_ICONERROR);
	}

	CoUninitialize();
}
