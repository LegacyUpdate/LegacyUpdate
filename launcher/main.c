#include "stdafx.h"
#include "main.h"
#include "resource.h"
#include <windows.h>
#include <commctrl.h>
#include "MsgBox.h"
#include "RegisterServer.h"

HINSTANCE g_hInstance;

extern void LaunchUpdateSite(int argc, LPWSTR *argv, int nCmdShow);
extern void RunOnce();

EXTERN_C __declspec(dllexport)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	g_hInstance = hInstance;

	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	LPWSTR action = L"/launch";
	if (argc > 1) {
		action = argv[1];
	}

	// All remaining args past the action
	LPWSTR *flags = {0};
	int flagsCount = 0;
	if (argc > 2) {
		flags = &argv[2];
		flagsCount = argc - 2;
	}

	if (wcscmp(action, L"/launch") == 0) {
		LaunchUpdateSite(flagsCount, flags, nCmdShow);
	} else if (wcscmp(action, L"/runonce") == 0) {
		RunOnce();
	} else if (wcscmp(action, L"/regserver") == 0 || wcscmp(action, L"/unregserver") == 0) {
		BOOL state = wcscmp(action, L"/regserver") == 0;
		HWND hwnd = flagsCount > 0 ? (HWND)(intptr_t)wcstol(flags[0], NULL, 10) : 0;
		RegisterServer(hwnd, state, FALSE);
	} else {
		const LPWSTR usage = L""
			L"LegacyUpdate.exe [/launch|/regserver|/unregserver]\n"
			L"\n"
			L"/launch\n"
			L"    Launch Legacy Update website in Internet Explorer\n"
			L"\n"
			L"/regserver\n"
			L"    Register ActiveX control\n"
			L"\n"
			L"/unregserver\n"
			L"    Unregister ActiveX control\n"
			L"\n"
			L"If no parameters are provided, /launch is assumed.";
		MsgBox(NULL, L"LegacyUpdate.exe usage", usage, MB_OK);
		PostQuitMessage(1);
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) == 1) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		switch (msg.message) {
		case WM_QUIT:
		case WM_DESTROY:
			break;
		}
	}

	ExitProcess(msg.wParam);
	return msg.wParam;
}
