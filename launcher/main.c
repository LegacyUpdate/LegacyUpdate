#include "stdafx.h"
#include "main.h"
#include "resource.h"
#include <windows.h>
#include <commctrl.h>
#include "MsgBox.h"
#include "RegisterServer.h"
#include "Startup.h"

HINSTANCE g_hInstance;

extern void LaunchUpdateSite(int argc, LPWSTR *argv, int nCmdShow);
extern void LaunchOptions(int nCmdShow);
extern void LaunchLog(int nCmdShow);
extern void RunOnce();

typedef enum Action {
	ActionLaunch,
	ActionOptions,
	ActionLog,
	ActionRunOnce,
	ActionRegServer,
	ActionUnregServer
} Action;

static const LPWSTR actions[] = {
	L"/launch",
	L"/options",
	L"/log",
	L"/runonce",
	L"/regserver",
	L"/unregserver"
};

EXTERN_C __declspec(dllexport)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	g_hInstance = hInstance;
	Startup();

	// nCmdShow seems to give us garbage values. Get it from the startup info struct.
	STARTUPINFO startupInfo;
	GetStartupInfo(&startupInfo);
	nCmdShow = (startupInfo.dwFlags & STARTF_USESHOWWINDOW) ? startupInfo.wShowWindow : SW_SHOWDEFAULT;

	// Init COM
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (!SUCCEEDED(hr)) {
		ExitProcess(hr);
		return hr;
	}

	// Init common controls
	INITCOMMONCONTROLSEX initComctl = {0};
	initComctl.dwSize = sizeof(initComctl);
	initComctl.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&initComctl);

	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	LPWSTR actionFlag = L"/launch";
	if (argc > 1) {
		actionFlag = argv[1];
	}

	// All remaining args past the action
	LPWSTR *flags = NULL;
	int flagsCount = 0;
	if (argc > 2) {
		flags = &argv[2];
		flagsCount = argc - 2;
	}

	Action action = -1;

	for (DWORD i = 0; i < ARRAYSIZE(actions); i++) {
		if (wcscmp(actionFlag, actions[i]) == 0) {
			action = i;
			break;
		}
	}

	switch (action) {
	case ActionLaunch:
		LaunchUpdateSite(flagsCount, flags, nCmdShow);
		break;

	case ActionOptions:
		LaunchOptions(nCmdShow);
		break;

	case ActionLog:
		LaunchLog(nCmdShow);
		break;

	case ActionRunOnce:
		RunOnce();
		break;

	case ActionRegServer:
	case ActionUnregServer: {
		BOOL state = action == ActionRegServer;
		HWND hwnd = flagsCount > 0 ? (HWND)(intptr_t)wcstol(flags[0], NULL, 10) : 0;
		RegisterServer(hwnd, state, FALSE);
		break;
	}

	default: {
		const LPWSTR usage = L""
			L"LegacyUpdate.exe [/launch|/regserver|/unregserver]\n"
			L"\n"
			L"/launch\n"
			L"    Launch Legacy Update website in Internet Explorer\n"
			L"\n"
			L"/options\n"
			L"    Open the Windows Update Options control panel\n"
			L"\n"
			L"/log\n"
			L"    Open the Windows Update log file\n"
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
		break;
	}
	}

	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0) == 1) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		switch (msg.message) {
		case WM_QUIT:
		case WM_DESTROY:
			break;
		}
	}

	CoUninitialize();
	ExitProcess(msg.wParam);
	return msg.wParam;
}
