#include <windows.h>
#include <nsis/pluginapi.h>
#include "main.h"
#include "resource.h"
#include "LoadImage.h"
#include "SafeString.h"

#define INSTALLER_ICON_ID 103

static HWND g_activeXHwnd;
static BOOL g_activeXSelected = FALSE;

static WNDPROC g_dialogOrigWndProc;

static HICON g_yesIcon;
static HICON g_noIcon;

static INT_PTR CALLBACK ActiveXDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORLISTBOX:
		return SendMessage(g_hwndParent, uMsg, wParam, lParam);
	}

	return FALSE;
}

static LRESULT CALLBACK ActiveXMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = CallWindowProc(g_dialogOrigWndProc, hwnd, uMsg, wParam, lParam);

	switch (uMsg) {
	case WM_NOTIFY_OUTER_NEXT:
		g_activeXSelected = IsDlgButtonChecked(g_activeXHwnd, IDC_YESRADIO) == BST_CHECKED;

		DestroyWindow(g_activeXHwnd);
		g_activeXHwnd = NULL;
		break;
	}

	return result;
}

PLUGIN_METHOD(ActiveXPageCreate) {
	PLUGIN_INIT();

	WCHAR introText[NSIS_MAX_STRLEN], yesTitle[NSIS_MAX_STRLEN], yesText[NSIS_MAX_STRLEN], noTitle[NSIS_MAX_STRLEN], noText[NSIS_MAX_STRLEN];
	popstringn(introText, sizeof(introText) / sizeof(WCHAR));
	popstringn(yesTitle, sizeof(yesTitle) / sizeof(WCHAR));
	popstringn(yesText, sizeof(yesText) / sizeof(WCHAR));
	popstringn(noTitle, sizeof(noTitle) / sizeof(WCHAR));
	popstringn(noText, sizeof(noText) / sizeof(WCHAR));
	g_activeXSelected = popint() == 1;

	// Set up child dialog
	RECT childRect;
	HWND childHwnd = GetDlgItem(g_hwndParent, IDC_CHILDRECT);
	GetWindowRect(childHwnd, &childRect);
	MapWindowPoints(NULL, g_hwndParent, (LPPOINT)&childRect, 2);

	// Icons
	g_activeXHwnd = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_ACTIVEX), g_hwndParent, ActiveXDialogProc);
	if (g_activeXHwnd == NULL) {
		return;
	}

	SetWindowPos(g_activeXHwnd, 0,
		childRect.left, childRect.top,
		childRect.right - childRect.left,
		childRect.bottom - childRect.top,
		SWP_NOZORDER | SWP_NOACTIVATE);

	// Icons
	g_yesIcon = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(INSTALLER_ICON_ID), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
	if (g_yesIcon) {
		SendDlgItemMessage(g_activeXHwnd, IDC_YESICON, STM_SETICON, (WPARAM)g_yesIcon, 0);
	}

	g_noIcon = LoadWuIcon();
	if (g_noIcon) {
		SendDlgItemMessage(g_activeXHwnd, IDC_NOICON, STM_SETICON, (WPARAM)g_noIcon, 0);
	}

	// Text
	SetDlgItemText(g_activeXHwnd, IDC_INTROTEXT, introText);
	SetDlgItemText(g_activeXHwnd, IDC_YESRADIO, yesTitle);
	SetDlgItemText(g_activeXHwnd, IDC_YESTEXT, yesText);
	SetDlgItemText(g_activeXHwnd, IDC_NORADIO, noTitle);
	SetDlgItemText(g_activeXHwnd, IDC_NOTEXT, noText);

	// Initial selection
	int selectedRadio = g_activeXSelected ? IDC_YESRADIO : IDC_NORADIO;
	CheckRadioButton(g_activeXHwnd, IDC_YESRADIO, IDC_NORADIO, selectedRadio);

	// Show window
	g_dialogOrigWndProc = (WNDPROC)SetWindowLongPtr(g_hwndParent, GWLP_WNDPROC, (LONG_PTR)ActiveXMainWndProc);
	SendMessage(g_hwndParent, WM_NOTIFY_CUSTOM_READY, (WPARAM)g_activeXHwnd, 0);
	ShowWindow(g_activeXHwnd, SW_SHOWNA);
	SetFocus(GetDlgItem(g_activeXHwnd, selectedRadio));
}

PLUGIN_METHOD(ActiveXPageShow) {
	while (g_activeXHwnd) {
		MSG msg;
		GetMessage(&msg, NULL, 0, 0);
		if (!IsDialogMessage(g_activeXHwnd, &msg) && !IsDialogMessage(g_hwndParent, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	pushint(g_activeXSelected ? 1 : 0);

	// Cleanup
	if (g_dialogOrigWndProc) {
		SetWindowLongPtr(g_hwndParent, GWLP_WNDPROC, (LONG_PTR)g_dialogOrigWndProc);
		g_dialogOrigWndProc = NULL;
	}

	if (g_yesIcon) {
		DestroyIcon(g_yesIcon);
		g_yesIcon = NULL;
	}

	if (g_noIcon) {
		DestroyIcon(g_noIcon);
		g_noIcon = NULL;
	}
}
