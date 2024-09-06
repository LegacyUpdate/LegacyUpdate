#include "stdafx.h"
#include "HResult.h"
#include "Registry.h"
#include <shellapi.h>

// Function signature required by Rundll32.exe.
void CALLBACK LaunchUpdateSite(HWND hwnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	LPWSTR path;
	DWORD pathSize;

	if (!SUCCEEDED(hr)) {
		goto end;
	}

	// Run LegacyUpdate.exe from native Program Files directory.
	hr = GetRegistryString(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion", L"ProgramFilesDir", KEY_WOW64_64KEY, &path, &pathSize);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	PathAppend(path, L"Legacy Update\\LegacyUpdate.exe");

	if ((INT_PTR)ShellExecute(NULL, NULL, path, L"", NULL, nCmdShow) <= 32) {
		hr = HRESULT_FROM_WIN32(GetLastError());
	}

end:
	if (!SUCCEEDED(hr)) {
		MessageBox(NULL, GetMessageForHresult(hr), L"Legacy Update", MB_OK | MB_ICONERROR);
	}

	CoUninitialize();
}
