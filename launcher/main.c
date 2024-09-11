#include "stdafx.h"
#include "main.h"
#include "resource.h"
#include <windows.h>
#include <commctrl.h>
#include "MsgBox.h"

HINSTANCE g_hInstance;

extern void LaunchUpdateSite(HWND hwnd, int nCmdShow);
extern void RunOnce();

EXTERN_C __declspec(dllexport)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	g_hInstance = hInstance;

	MsgBox(NULL, L"LegacyUpdate.exe", L"Legacy Update", MB_OK | MB_ICONINFORMATION);

	int argc;
	LPWSTR *argv = CommandLineToArgvW(pCmdLine, &argc);

	LPWSTR action = L"launch";
	if (argc > 1) {
		action = argv[1];
	}

	if (wcscmp(action, L"launch") == 0) {
		LaunchUpdateSite(NULL, nCmdShow);
	} else if (wcscmp(action, L"/runonce") == 0) {
		RunOnce();
	} else {
		MsgBox(NULL, L"Unknown LegacyUpdate.exe usage", NULL, MB_OK);
		return 1;
	}

	return 0;
}
