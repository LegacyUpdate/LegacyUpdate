#include "stdafx.h"
#include "main.h"
#include "resource.h"
#include <windows.h>
#include <commctrl.h>
#include "MsgBox.h"

HINSTANCE g_hInstance;

extern void LaunchUpdateSite(HWND hwnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);
extern void RunOnce();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	g_hInstance = hInstance;

	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	LPWSTR action = L"launch";
	if (argc > 1) {
		action = argv[1];
	}

	if (wcscmp(action, L"launch") == 0) {
		LaunchUpdateSite(NULL, hInstance, lpCmdLine, nCmdShow);
	} else if (wcscmp(action, L"/runonce") == 0) {
		RunOnce();
	} else {
		MsgBox(NULL, hInstance, L"Unknown LegacyUpdate.exe usage", NULL, MB_OK);
		return 1;
	}

	return 0;
}
