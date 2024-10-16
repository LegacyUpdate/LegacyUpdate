#include "stdafx.h"

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <windows.h>
#include <nsis/pluginapi.h>
#include <shlwapi.h>
#include <uxtheme.h>
#include <windowsx.h>
#include "main.h"
#include "LoadImage.h"
#include "VersionInfo.h"

#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031e
#endif

#define IDI_BANNER_WORDMARK      1337
#define IDI_BANNER_WORDMARK_GLOW 1338

typedef HRESULT (WINAPI *_DwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS *pMarInset);
typedef HRESULT (WINAPI *_DwmIsCompositionEnabled)(BOOL *pfEnabled);
typedef HRESULT (WINAPI *_DwmDefWindowProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
typedef HRESULT (WINAPI *_SetWindowThemeAttribute)(HWND hwnd, enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute);
typedef BOOL (WINAPI *_IsThemeActive)();

static _DwmExtendFrameIntoClientArea $DwmExtendFrameIntoClientArea;
static _DwmIsCompositionEnabled $DwmIsCompositionEnabled;
static _DwmDefWindowProc $DwmDefWindowProc;
static _SetWindowThemeAttribute $SetWindowThemeAttribute;
static _IsThemeActive $IsThemeActive;

typedef enum Theme {
	ThemeUnknown = -1,
	ThemeClassic,
	ThemeBasic,
	ThemeAero
} Theme;

static HBITMAP g_bannerWordmark;
static HBITMAP g_bannerWordmarkGlow;
static Theme g_theme = ThemeUnknown;
static WNDPROC g_dialogOrigWndProc;
static WNDPROC g_bannerOrigWndProc;
static WNDPROC g_bottomOrigWndProc;

static Theme GetTheme() {
	// return ThemeClassic;
	BOOL enabled;
	if (!$DwmIsCompositionEnabled || !$IsThemeActive || !SUCCEEDED($DwmIsCompositionEnabled(&enabled))) {
		return ThemeClassic;
	}

	if (enabled) {
		return ThemeAero;
	}

	return $IsThemeActive() ? ThemeBasic : ThemeClassic;
}

static void ConfigureWindow() {
	HWND bannerWindow = GetDlgItem(g_hwndParent, 1046);
	HWND bannerDivider = GetDlgItem(g_hwndParent, 1047);
	HWND bottomDivider = GetDlgItem(g_hwndParent, 6900);
	if (!bannerWindow || !bannerDivider || !bottomDivider) {
		return;
	}

	Theme theme = GetTheme();

	if (g_theme != theme) {
		g_theme = theme;
		MARGINS margins = {0};

		if (theme == ThemeAero) {
			// Set black brush for DWM
			SetClassLongPtr(bannerWindow, GCLP_HBRBACKGROUND, (LONG_PTR)GetStockObject(BLACK_BRUSH));

			// Set glass area
			RECT rect;
			GetWindowRect(bannerWindow, &rect);
			margins.cyTopHeight = rect.bottom - rect.top;
		} else {
			// Remove black brush
			SetClassLongPtr(bannerWindow, GCLP_HBRBACKGROUND, (LONG_PTR)GetSysColorBrush(COLOR_BTNFACE));
		}

		if ($DwmExtendFrameIntoClientArea) {
			$DwmExtendFrameIntoClientArea(g_hwndParent, &margins);
		}

		ShowWindow(bannerDivider, theme >= ThemeBasic ? SW_HIDE : SW_SHOW);
		ShowWindow(bottomDivider, theme >= ThemeBasic ? SW_HIDE : SW_SHOW);

		if (g_theme == ThemeAero) {
			g_bannerWordmark = LoadPNGResource(NULL, MAKEINTRESOURCE(IDI_BANNER_WORDMARK), L"PNG");
			if (AtMostWin7()) {
				g_bannerWordmarkGlow = LoadPNGResource(NULL, MAKEINTRESOURCE(IDI_BANNER_WORDMARK_GLOW), L"PNG");
			}
		} else {
			DeleteObject(g_bannerWordmark);
			DeleteObject(g_bannerWordmarkGlow);
			g_bannerWordmark = NULL;
			g_bannerWordmarkGlow = NULL;
		}
	}
}

static LRESULT CALLBACK BannerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (g_bannerOrigWndProc == NULL) {
		return 0;
	}

	if (g_theme != ThemeAero) {
		return CallWindowProc(g_bannerOrigWndProc, hwnd, uMsg, wParam, lParam);
	}

	switch (uMsg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rect;
		GetClientRect(hwnd, &rect);

		float scale = (float)GetDeviceCaps(hdc, LOGPIXELSX) / 96.0f;

		BLENDFUNCTION blendFunc;
		blendFunc.BlendOp = AC_SRC_OVER;
		blendFunc.BlendFlags = 0;
		blendFunc.SourceConstantAlpha = 0xFF;
		blendFunc.AlphaFormat = AC_SRC_ALPHA;

		// Draw wordmark with alpha blending
		if (g_bannerWordmarkGlow) {
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_bannerWordmarkGlow);

			BITMAP bitmap;
			GetObject(g_bannerWordmarkGlow, sizeof(bitmap), &bitmap);

			LONG width = bitmap.bmWidth * scale;
			LONG height = bitmap.bmHeight * scale;
			LONG x = (rect.right - rect.left - width) / 2;
			LONG y = (rect.bottom - rect.top - height) / 2;

			SetStretchBltMode(hdc, HALFTONE);
			AlphaBlend(hdc,
				x, y, width, height, hdcMem,
				0, 0, bitmap.bmWidth, bitmap.bmHeight, blendFunc);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
		}

		if (g_bannerWordmark) {
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_bannerWordmark);

			BITMAP bitmap;
			GetObject(g_bannerWordmark, sizeof(bitmap), &bitmap);

			LONG width = bitmap.bmWidth * scale;
			LONG height = bitmap.bmHeight * scale;
			LONG x = (rect.right - rect.left - width) / 2;
			LONG y = ((rect.bottom - rect.top - height) / 2) - (1 * scale);

			SetStretchBltMode(hdc, HALFTONE);
			AlphaBlend(hdc,
				x, y, width, height, hdcMem,
				0, 0, bitmap.bmWidth, bitmap.bmHeight, blendFunc);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
		}

		EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_ERASEBKGND:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);

	case WM_NCHITTEST:
		// Pass through to parent
		return HTTRANSPARENT;

	case WM_DESTROY:
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)g_bannerOrigWndProc);
		g_bannerOrigWndProc = NULL;
		break;
	}

	return CallWindowProc(g_bannerOrigWndProc, hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK BottomWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (!g_bottomOrigWndProc) {
		return 0;
	}

	switch (uMsg) {
	case WM_PAINT: {
		// Draw subtle 1px divider line for Aero
		if (g_theme < ThemeBasic) {
			break;
		}

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT rect;
		GetClientRect(hwnd, &rect);

		// TODO: Can we get this color from uxtheme?
		HBRUSH brush = CreateSolidBrush(RGB(0xDF, 0xDF, 0xDF));
		RECT lineRect = {rect.left, rect.top, rect.right, rect.top + 1};
		FillRect(hdc, &lineRect, brush);
		DeleteObject(brush);

		brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		lineRect = (RECT){rect.left, rect.top + 1, rect.right, rect.bottom};
		FillRect(hdc, &lineRect, brush);
		DeleteObject(brush);

		EndPaint(hwnd, &ps);
		return 0;
	}
	}

	return CallWindowProc(g_bottomOrigWndProc, hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (!g_dialogOrigWndProc) {
		return 0;
	}

	if (g_theme == ThemeAero && $DwmDefWindowProc) {
		LRESULT lRet = 0;
		if ($DwmDefWindowProc(hwnd, uMsg, wParam, lParam, &lRet)) {
			return lRet;
		}
	}

	switch (uMsg) {
	case WM_THEMECHANGED:
	case WM_DWMCOMPOSITIONCHANGED:
		ConfigureWindow();
		break;

	case WM_NCHITTEST: {
		if (g_theme != ThemeAero) {
			break;
		}

		// Allow drag in the header area
		HWND bannerWindow = GetDlgItem(hwnd, 1046);
		if (!bannerWindow) {
			break;
		}

		POINT hit = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		ScreenToClient(hwnd, &hit);

		RECT rect;
		GetWindowRect(bannerWindow, &rect);
		rect.right -= rect.left;
		rect.bottom -= rect.top;
		rect.left = 0;
		rect.top = 0;

		if (PtInRect(&rect, hit)) {
			return HTCAPTION;
		}
		break;
	}

	case WM_DESTROY:
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)g_dialogOrigWndProc);
		g_dialogOrigWndProc = NULL;
		break;
	}

	return CallWindowProc(g_dialogOrigWndProc, hwnd, uMsg, wParam, lParam);
}

static UINT_PTR NSISPluginCallback(enum NSPIM event) {
	// Does nothing, but keeping a callback registered prevents NSIS from unloading the plugin
	return 0;
}

PLUGIN_METHOD(DialogInit) {
	PLUGIN_INIT();

	if (g_dialogOrigWndProc) {
		return;
	}

	extra->RegisterPluginCallback(g_hInstance, NSISPluginCallback);

	// Get DWM symbols
	HMODULE dwmapi = LoadLibrary(L"dwmapi.dll");
	if (dwmapi) {
		$DwmExtendFrameIntoClientArea = (_DwmExtendFrameIntoClientArea)GetProcAddress(dwmapi, "DwmExtendFrameIntoClientArea");
		$DwmIsCompositionEnabled = (_DwmIsCompositionEnabled)GetProcAddress(dwmapi, "DwmIsCompositionEnabled");
		$DwmDefWindowProc = (_DwmDefWindowProc)GetProcAddress(dwmapi, "DwmDefWindowProc");
	}

	// Get UxTheme symbols
	HMODULE uxtheme = LoadLibrary(L"uxtheme.dll");
	if (uxtheme) {
		$SetWindowThemeAttribute = (_SetWindowThemeAttribute)GetProcAddress(uxtheme, "SetWindowThemeAttribute");
		$IsThemeActive = (_IsThemeActive)GetProcAddress(uxtheme, "IsThemeActive");
	}

	// Hide title caption/icon
	if ($SetWindowThemeAttribute) {
		WTA_OPTIONS options;
		options.dwFlags = WTNCA_NODRAWCAPTION | WTNCA_NODRAWICON;
		options.dwMask = WTNCA_NODRAWCAPTION | WTNCA_NODRAWICON;
		$SetWindowThemeAttribute(g_hwndParent, WTA_NONCLIENT, &options, sizeof(options));
	}

	// Set up extended client frame
	ConfigureWindow();

	// Set up window procedures
	HWND bannerWindow = GetDlgItem(g_hwndParent, 1046);
	HWND bottomWindow = GetDlgItem(g_hwndParent, 6901);
	g_dialogOrigWndProc = (WNDPROC)SetWindowLongPtr(g_hwndParent, GWLP_WNDPROC, (LONG_PTR)MainWndProc);
	g_bannerOrigWndProc = (WNDPROC)SetWindowLongPtr(bannerWindow, GWLP_WNDPROC, (LONG_PTR)BannerWndProc);
	g_bottomOrigWndProc = (WNDPROC)SetWindowLongPtr(bottomWindow, GWLP_WNDPROC, (LONG_PTR)BottomWndProc);
}
