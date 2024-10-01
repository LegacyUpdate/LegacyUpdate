#include <windows.h>
#include <commctrl.h>
#include "GetPidForName.h"
#include "MsgBox.h"
#include "VersionInfo.h"
#include "LoadImage.h"

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#define HK_RUNCMD 1

typedef DWORD (__fastcall *_ThemeWaitForServiceReady)(DWORD timeout);
typedef DWORD (__fastcall *_ThemeWatchForStart)();

static HBITMAP g_wallpaperBitmap;
static HANDLE g_cmdHandle;

static void StartThemes() {
	// Ask UxInit.dll to ask the Themes service to start a session for this desktop. Themes doesn't
	// automatically start a session for the SYSTEM desktop, so we need to ask it to. This matches
	// what msoobe.exe does, e.g., on first boot.

	// Windows 7 moves this to UxInit.dll
	HMODULE shsvcs = LoadLibrary(L"UxInit.dll");
	if (!shsvcs) {
		shsvcs = LoadLibrary(L"shsvcs.dll");
		if (!shsvcs) {
			return;
		}
	}

	// Get functions by ordinals
	_ThemeWaitForServiceReady $ThemeWaitForServiceReady = (_ThemeWaitForServiceReady)GetProcAddress(shsvcs, MAKEINTRESOURCEA(2));
	_ThemeWatchForStart $ThemeWatchForStart = (_ThemeWatchForStart)GetProcAddress(shsvcs, MAKEINTRESOURCEA(1));

	// 1. Wait up to 1000ms for Themes to start
	if ($ThemeWaitForServiceReady) {
		$ThemeWaitForServiceReady(1000);
	}

	// 2. Prompt Themes to start a session for the SYSTEM desktop
	if ($ThemeWatchForStart) {
		$ThemeWatchForStart();
	}

	FreeLibrary(shsvcs);
}

static BOOL RunCmd(LPPROCESS_INFORMATION processInfo) {
	WCHAR cmd[MAX_PATH];
	ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\cmd.exe", cmd, ARRAYSIZE(cmd));

	STARTUPINFO startupInfo = {0};
	startupInfo.cb = sizeof(startupInfo);

	if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, processInfo)) {
		MsgBox(NULL, L"Launching cmd.exe failed", NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	CloseHandle(processInfo->hThread);
	g_cmdHandle = processInfo->hProcess;
	return TRUE;
}

static const COLORREF WallpaperColorWin10 = RGB(24,   0,  82); // #180052
static const COLORREF WallpaperColorWin8  = RGB(32, 103, 178); // #2067b2

static COLORREF g_wallpaperColor;

static LRESULT CALLBACK WallpaperWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_PAINT: {
		// Set background image on the window
		// 2k/XP: Desktop color
		// Vista/7: OOBE wallpaper
		// 8/10+: Solid color

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		if (g_wallpaperBitmap) {
			HDC memDC = CreateCompatibleDC(hdc);
			HBITMAP oldBitmap = SelectObject(memDC, g_wallpaperBitmap);

			BITMAP bitmap;
			GetObject(g_wallpaperBitmap, sizeof(BITMAP), &bitmap);

			// Get the dimensions of the window
			RECT rect;
			GetClientRect(hwnd, &rect);

			// Stretch the bitmap to fit the entire window
			SetStretchBltMode(hdc, HALFTONE);
			StretchBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
				memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

			SelectObject(memDC, oldBitmap);
			DeleteDC(memDC);
		} else {
			HBRUSH brush;
			if (IsOSVersionOrLater(6, 2)) {
				brush = CreateSolidBrush(IsOSVersionOrLater(10, 0) ? WallpaperColorWin10 : WallpaperColorWin8);
			} else {
				brush = CreateSolidBrush(GetSysColor(COLOR_DESKTOP));
			}

			FillRect(hdc, &ps.rcPaint, brush);
			DeleteObject(brush);
		}

		break;
	}

	case WM_DISPLAYCHANGE:
	case WM_THEMECHANGED:
	case WM_DPICHANGED: {
		// Resize to fit the new screen size
		DWORD width = GetSystemMetrics(SM_CXSCREEN);
		DWORD height = GetSystemMetrics(SM_CYSCREEN);
		SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	}

	case WM_NCHITTEST:
		// Don't accept any mouse input
		return HTNOWHERE;

	case WM_HOTKEY: {
		// Shift-F10 to run cmd
		if (wParam == HK_RUNCMD) {
			DWORD exitCode;
			if (!g_cmdHandle || (GetExitCodeProcess(g_cmdHandle, &exitCode) && exitCode != STILL_ACTIVE)) {
				PROCESS_INFORMATION processInfo;
				RunCmd(&processInfo);
			}
		}

		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);;
}

const WCHAR WallpaperClassName[] = L"LegacyUpdateWallpaper";

static void CreateWallpaperWindow() {
	// Init COM
	CoInitialize(NULL);

	// Init common controls
	INITCOMMONCONTROLSEX initComctl = {0};
	initComctl.dwSize = sizeof(initComctl);
	initComctl.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&initComctl);

	// Create window
	WNDCLASS wndClass = {0};
	wndClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC | CS_NOCLOSE;
	wndClass.lpfnWndProc = WallpaperWndProc;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.lpszClassName = WallpaperClassName;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClass(&wndClass)) {
		TRACE(L"RegisterClass failed: %d", GetLastError());
		return;
	}

	DWORD width = GetSystemMetrics(SM_CXSCREEN);
	DWORD height = GetSystemMetrics(SM_CYSCREEN);
	HWND hwnd = CreateWindowEx(
		WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
		wndClass.lpszClassName, L"Legacy Update",
		WS_POPUP,
		0, 0, width, height,
		NULL, NULL,
		wndClass.hInstance,
		NULL
	);
	if (!hwnd) {
		TRACE(L"CreateWindow failed: %d", GetLastError());
		return;
	}

	// Register hotkey
	RegisterHotKey(hwnd, HK_RUNCMD, MOD_SHIFT, VK_F10);

	if (IsOSVersionEqual(6, 1)) {
		// 7: Bitmap in oobe dir
		WCHAR bmpPath[MAX_PATH];
		ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\oobe\\background.bmp", bmpPath, ARRAYSIZE(bmpPath));
		g_wallpaperBitmap = LoadImage(NULL, bmpPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	} else if (IsOSVersionEqual(6, 0)) {
		// Vista: Resources in ooberesources.dll
		WCHAR ooberesPath[MAX_PATH];
		ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\oobe\\ooberesources.dll", ooberesPath, ARRAYSIZE(ooberesPath));
		HMODULE ooberes = LoadLibrary(ooberesPath);
		if (ooberes) {
			// Width logic is the same used by Vista msoobe.dll
			g_wallpaperBitmap = LoadPNGResource(ooberes, width < 1200 ? L"OOBE_BACKGROUND_0" : L"OOBE_BACKGROUND_LARGE_0", RT_RCDATA);
		}
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

static BOOL IsSystemUser() {
	BOOL result = FALSE;
	PTOKEN_USER tokenInfo;
	PSID systemSid;
	HANDLE token;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
		goto end;
	}

	DWORD tokenInfoLen;
	GetTokenInformation(token, TokenUser, NULL, 0, &tokenInfoLen);
	tokenInfo = (PTOKEN_USER)LocalAlloc(LPTR, tokenInfoLen);
	if (!GetTokenInformation(token, TokenUser, tokenInfo, tokenInfoLen, &tokenInfoLen)) {
		goto end;
	}

	DWORD sidSize = SECURITY_MAX_SID_SIZE;
	systemSid = LocalAlloc(LPTR, sidSize);
	if (!CreateWellKnownSid(WinLocalSystemSid, NULL, systemSid, &sidSize)) {
		goto end;
	}

	result = EqualSid(tokenInfo->User.Sid, systemSid);

end:
	if (tokenInfo) {
		LocalFree(tokenInfo);
	}
	if (systemSid) {
		LocalFree(systemSid);
	}
	if (token) {
		CloseHandle(token);
	}
	return result;
}

void RunOnce() {
#ifndef _DEBUG
	// Only relevant if we're SYSTEM
	if (!IsSystemUser()) {
		PostQuitMessage(1);
		return;
	}
#endif

	// Start Themes on this desktop
	StartThemes();

	// Find and hide the FirstUxWnd window, if it exists (Windows 7+)
	HWND firstUxWnd = FindWindow(L"FirstUxWndClass", NULL);
	if (firstUxWnd) {
		ShowWindow(firstUxWnd, SW_HIDE);
	}

	// Show our wallpaper window
	CreateWallpaperWindow();

	// Construct path to LegacyUpdateSetup.exe
	WCHAR setupPath[MAX_PATH];
	GetModuleFileName(NULL, setupPath, ARRAYSIZE(setupPath));
	wcsrchr(setupPath, L'\\')[1] = L'\0';
	wcsncat(setupPath, L"LegacyUpdateSetup.exe", ARRAYSIZE(setupPath) - wcslen(setupPath) - 1);

	// Execute and wait for completion
	STARTUPINFO startupInfo = {0};
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo = {0};
	LPWSTR cmdLine = (LPWSTR)LocalAlloc(LPTR, 4096 * sizeof(WCHAR));
	wsprintf(cmdLine, L"\"%ls\" /runonce", setupPath);
	if (!CreateProcess(setupPath, cmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo)) {
#ifdef _DEBUG
		// Run cmd.exe instead
		if (!RunCmd(&processInfo)) {
			PostQuitMessage(0);
			return;
		}
#else
		MsgBox(NULL, L"Continuing Legacy Update setup failed", NULL, MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;
#endif
	}

	CloseHandle(processInfo.hThread);

	// Wait for it to finish, while running a message loop
	MSG msg;
	while (WaitForSingleObject(processInfo.hProcess, 100) == WAIT_TIMEOUT) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CloseHandle(processInfo.hProcess);

	// Don't let SYSTEM cmd keep running beyond runonce
	if (g_cmdHandle) {
		TerminateProcess(g_cmdHandle, 0);
		CloseHandle(g_cmdHandle);
	}

	// Show FirstUxWnd again
	if (firstUxWnd) {
		ShowWindow(firstUxWnd, SW_SHOW);
	}

	PostQuitMessage(0);
}
