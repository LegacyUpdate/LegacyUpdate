#include "stdafx.h"
#include "Exec.h"
#include "HResult.h"
#include "Utils.h"

// Function signature required by Rundll32.exe.
EXTERN_C __declspec(dllexport)
void CALLBACK LaunchUpdateSite(HWND hwnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	// This just calls LegacyUpdate.exe now for backwards compatibility.
	hr = StartLauncher(L"/launch", TRUE);

end:
	if (!SUCCEEDED(hr)) {
		MessageBox(NULL, GetMessageForHresult(hr), L"Legacy Update", MB_OK | MB_ICONERROR);
	}

	CoUninitialize();
}
