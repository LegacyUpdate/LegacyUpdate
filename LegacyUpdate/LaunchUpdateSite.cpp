#include "stdafx.h"
#include "HResult.h"
#include "Registry.h"
#include "LegacyUpdate.h"
#include <shellapi.h>

// Function signature required by Rundll32.exe.
void CALLBACK LaunchUpdateSite(HWND hwnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	LPWSTR path;
	SHELLEXECUTEINFO execInfo = {0};

	if (!SUCCEEDED(hr)) {
		goto end;
	}

	// Run LegacyUpdate.exe from native Program Files directory.
	hr = GetInstallPath(&path);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	PathAppend(path, L"LegacyUpdate.exe");

	execInfo.cbSize = sizeof(execInfo);
	execInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	execInfo.hwnd = hwnd;
	execInfo.lpFile = path;
	execInfo.nShow = nCmdShow;
	if (!ShellExecuteEx(&execInfo)) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto end;
	}

	// Wait for it to finish and return its exit code
	WaitForSingleObject(execInfo.hProcess, INFINITE);

	if (GetExitCodeProcess(execInfo.hProcess, (DWORD *)&hr) == 0) {
		hr = HRESULT_FROM_WIN32(GetLastError());
	}

end:
	if (!SUCCEEDED(hr)) {
		MessageBox(NULL, GetMessageForHresult(hr), L"Legacy Update", MB_OK | MB_ICONERROR);
	}

	CoUninitialize();
}
