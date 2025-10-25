#include <windows.h>
#include <commctrl.h>
#include <nsis/pluginapi.h>
#include "main.h"
#include "resource.h"
#include "VersionInfo.h"

#define COUNTDOWN_ID     1234
#define COUNTDOWN_LENGTH 180

static HWND g_rebootHwnd;
static WPARAM g_rebootParam = 0;

static WNDPROC g_dialogOrigWndProc;

static DWORD g_timerStart = 0;
static DWORD g_timerInterval = 0;
static DWORD g_timerMultiplier = 0;
static UINT_PTR g_timer = 0;
static WCHAR g_timerPrefix[1024] = L"";

static INT_PTR CALLBACK RebootDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_TIMER:
		if (wParam == COUNTDOWN_ID) {
			DWORD elapsed = GetTickCount() - g_timerStart;
			DWORD elapsedSecs = elapsed / 1000;
			DWORD progress = min((elapsed * g_timerMultiplier) / 1000, COUNTDOWN_LENGTH * g_timerMultiplier);
			SendDlgItemMessage(hwnd, IDC_PROGRESS, PBM_SETPOS, progress, 0);

			DWORD remaining = max(COUNTDOWN_LENGTH - elapsedSecs, 0);
			WCHAR timerDisplay[1024];
			wsprintf(timerDisplay, L"%s%02d:%02d", g_timerPrefix, remaining / 60, remaining % 60);
			SetDlgItemText(hwnd, IDC_TEXT1, timerDisplay);

			if (elapsedSecs >= COUNTDOWN_LENGTH) {
				KillTimer(hwnd, COUNTDOWN_ID);
				g_timer = 0;
				PostMessage(hwnd, WM_NOTIFY_OUTER_NEXT, 0, 0);
			}
		}
		return TRUE;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORLISTBOX:
		return SendMessage(g_hwndParent, uMsg, wParam, lParam);
	}

	return FALSE;
}

static LRESULT CALLBACK RebootMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = CallWindowProc(g_dialogOrigWndProc, hwnd, uMsg, wParam, lParam);

	switch (uMsg) {
	case WM_NOTIFY_OUTER_NEXT:
		g_rebootParam = wParam;

		if (g_timer) {
			KillTimer(g_rebootHwnd, COUNTDOWN_ID);
			g_timer = 0;
		}

		DestroyWindow(g_rebootHwnd);
		g_rebootHwnd = NULL;
		break;
	}

	return result;
}

PLUGIN_METHOD(RebootPageCreate) {
	PLUGIN_INIT();

	WCHAR introText[1024], nextText[1024], cancelText[1024];
	popstringn(introText, sizeof(introText) / sizeof(WCHAR));
	popstringn(g_timerPrefix, sizeof(g_timerPrefix) / sizeof(WCHAR));
	popstringn(nextText, sizeof(nextText) / sizeof(WCHAR));
	popstringn(cancelText, sizeof(cancelText) / sizeof(WCHAR));

	// Set up child dialog
	RECT childRect;
	HWND childHwnd = GetDlgItem(g_hwndParent, IDC_CHILDRECT);
	GetWindowRect(childHwnd, &childRect);
	MapWindowPoints(NULL, g_hwndParent, (LPPOINT)&childRect, 2);

	g_rebootHwnd = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_REBOOT), g_hwndParent, RebootDialogProc);
	if (g_rebootHwnd == NULL) {
		return;
	}

	SetWindowPos(g_rebootHwnd, 0,
		childRect.left, childRect.top,
		childRect.right - childRect.left,
		childRect.bottom - childRect.top,
		SWP_NOZORDER | SWP_NOACTIVATE);

	// Icon
	HICON hIcon = LoadImage(NULL, MAKEINTRESOURCE(IDI_WARNING), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_SHARED | LR_DEFAULTSIZE);
	if (hIcon) {
		SendDlgItemMessage(g_rebootHwnd, IDI_ICON2, STM_SETICON, (WPARAM)hIcon, 0);
	}

	// Intro text
	SetDlgItemText(g_rebootHwnd, IDC_INTROTEXT, introText);

	// Smoother animation for Aero progress bar
	g_timerMultiplier = AtLeastWinVista() ? 10 : 1;

	// Progress bar/timer
	SendDlgItemMessage(g_rebootHwnd, IDC_PROGRESS, PBM_SETRANGE, 0, MAKELPARAM(0, COUNTDOWN_LENGTH * g_timerMultiplier));
	SendDlgItemMessage(g_rebootHwnd, IDC_PROGRESS, PBM_SETPOS, 0, 0);
	g_timerStart = GetTickCount();
	g_timer = SetTimer(g_rebootHwnd, COUNTDOWN_ID, 1000 / g_timerMultiplier, NULL);

	// Force initial timer callback to fire now
	SendMessage(g_rebootHwnd, WM_TIMER, COUNTDOWN_ID, 0);

	// Show window
	g_dialogOrigWndProc = (WNDPROC)SetWindowLongPtr(g_hwndParent, GWLP_WNDPROC, (LONG_PTR)RebootMainWndProc);
	SendMessage(g_hwndParent, WM_NOTIFY_CUSTOM_READY, (WPARAM)g_rebootHwnd, 0);
	ShowWindow(g_rebootHwnd, SW_SHOWNA);

	// Set Next button to Reboot
	HWND nextButton = GetDlgItem(g_hwndParent, IDOK);
	if (nextButton) {
		SetWindowText(nextButton, nextText);
	}

	// Set Cancel button to Later
	HWND cancelButton = GetDlgItem(g_hwndParent, IDCANCEL);
	if (cancelButton) {
		SetWindowText(cancelButton, cancelText);
		EnableWindow(cancelButton, TRUE);
	}

	// Bring attention back if the user switched away
	SetForegroundWindow(g_hwndParent);
	BringWindowToTop(g_hwndParent);
	SetFocus(nextButton);
	MessageBeep(AtLeastWinVista() ? MB_ICONWARNING : MB_ICONINFORMATION);

	// Flash taskbar button
	FLASHWINFO fInfo = {0};
	fInfo.cbSize = sizeof(fInfo);
	fInfo.hwnd = g_rebootHwnd;
	fInfo.dwFlags = FLASHW_ALL | FLASHW_TIMER;
	fInfo.uCount = COUNTDOWN_LENGTH;
	fInfo.dwTimeout = 0;
	FlashWindowEx(&fInfo);
}

PLUGIN_METHOD(RebootPageShow) {
	while (g_rebootHwnd) {
		MSG msg;
		GetMessage(&msg, NULL, 0, 0);
		if (!IsDialogMessage(g_rebootHwnd, &msg) && !IsDialogMessage(g_hwndParent, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Break out if timeout reached
		if (msg.message == WM_NOTIFY_OUTER_NEXT) {
			g_rebootParam = 1;
			break;
		}
	}

	pushint(g_rebootParam);

	// Cleanup
	if (g_timer) {
		KillTimer(g_rebootHwnd, COUNTDOWN_ID);
		g_timer = 0;
	}
	SetWindowLongPtr(g_hwndParent, GWLP_WNDPROC, (LONG_PTR)g_dialogOrigWndProc);
	g_dialogOrigWndProc = NULL;
}
