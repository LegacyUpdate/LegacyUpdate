#include "stdafx.h"

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <windows.h>
#include <nsis/pluginapi.h>
#include <shlwapi.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <windowsx.h>
#include "main.h"
#include "LoadImage.h"
#include "Registry.h"
#include "VersionInfo.h"

#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031e
#endif

#define DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 19
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#define DWMWA_CAPTION_COLOR       35
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#define DWMWA_COLOR_NONE          0xFFFFFFFE
#define DWMSBT_MAINWINDOW         2

#define IDI_BANNER_WORDMARK_LIGHT 1337
#define IDI_BANNER_WORDMARK_DARK  1338
#define IDI_BANNER_WORDMARK_GLOW  1339

typedef HRESULT (WINAPI *_DwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS *pMarInset);
typedef HRESULT (WINAPI *_DwmIsCompositionEnabled)(BOOL *pfEnabled);
typedef HRESULT (WINAPI *_DwmDefWindowProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
typedef HRESULT (WINAPI *_DwmSetWindowAttribute)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
typedef HRESULT (WINAPI *_SetWindowThemeAttribute)(HWND hwnd, enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute);
typedef BOOL (WINAPI *_IsThemeActive)();
typedef HTHEME (WINAPI *_OpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (WINAPI *_CloseThemeData)(HTHEME hTheme);
typedef HRESULT (WINAPI *_DrawThemeBackground)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect);

static _DwmExtendFrameIntoClientArea $DwmExtendFrameIntoClientArea;
static _DwmIsCompositionEnabled $DwmIsCompositionEnabled;
static _DwmDefWindowProc $DwmDefWindowProc;
static _DwmSetWindowAttribute $DwmSetWindowAttribute;
static _SetWindowThemeAttribute $SetWindowThemeAttribute;
static _IsThemeActive $IsThemeActive;
static _OpenThemeData $OpenThemeData;
static _CloseThemeData $CloseThemeData;
static _DrawThemeBackground $DrawThemeBackground;

typedef enum Theme {
	ThemeUnknown = -1,
	ThemeClassic,
	ThemeBasic,
	ThemeAeroLight,
	ThemeAeroDark
} Theme;

static HBITMAP g_bannerWordmark;
static HBITMAP g_bannerWordmarkGlow;
static HTHEME g_aeroTheme;
static Theme g_theme = ThemeUnknown;

static WNDPROC g_dialogOrigWndProc;
static WNDPROC g_bannerOrigWndProc;
static WNDPROC g_bottomOrigWndProc;

static Theme GetTheme() {
	BOOL enabled;
	if (!$DwmIsCompositionEnabled || !$IsThemeActive || !SUCCEEDED($DwmIsCompositionEnabled(&enabled))) {
		return ThemeClassic;
	}

	if (enabled) {
		DWORD light = 1;
		if (AtLeastWin10_1809()) {
			GetRegistryDword(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", 0, &light);
		}
		return light ? ThemeAeroLight : ThemeAeroDark;
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

		if (theme >= ThemeAeroLight) {
			// Set glass area
			RECT rect;
			GetWindowRect(bannerWindow, &rect);
			margins.cyTopHeight = rect.bottom - rect.top;
		}

		if ($DwmExtendFrameIntoClientArea) {
			$DwmExtendFrameIntoClientArea(g_hwndParent, &margins);
		}

		ShowWindow(bannerDivider, theme >= ThemeBasic ? SW_HIDE : SW_SHOW);
		ShowWindow(bottomDivider, theme >= ThemeBasic ? SW_HIDE : SW_SHOW);

		if (g_theme >= ThemeBasic) {
			DWORD wordmark = g_theme == ThemeAeroDark ? IDI_BANNER_WORDMARK_DARK : IDI_BANNER_WORDMARK_LIGHT;
			g_bannerWordmark = LoadPNGResource(NULL, MAKEINTRESOURCE(wordmark), L"PNG");
			if (g_theme >= ThemeAeroLight && AtMostWin7()) {
				g_bannerWordmarkGlow = LoadPNGResource(NULL, MAKEINTRESOURCE(IDI_BANNER_WORDMARK_GLOW), L"PNG");
			}
		} else {
			DeleteObject(g_bannerWordmark);
			DeleteObject(g_bannerWordmarkGlow);
			g_bannerWordmark = NULL;
			g_bannerWordmarkGlow = NULL;
		}

		// Set dark mode state
		if (AtLeastWin10_1809() && $DwmSetWindowAttribute) {
			DWORD attr = AtLeastWin10_2004() ? DWMWA_USE_IMMERSIVE_DARK_MODE : DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1;
			DWORD value = g_theme == ThemeAeroDark;
			$DwmSetWindowAttribute(g_hwndParent, attr, &value, sizeof(value));
		}
	}
}

static LRESULT CALLBACK BannerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (g_theme < ThemeBasic) {
		return CallWindowProc(g_bannerOrigWndProc, hwnd, uMsg, wParam, lParam);
	}

	switch (uMsg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rect;
		GetClientRect(hwnd, &rect);

		// Draw base color for glass area
		FillRect(hdc, &rect, GetStockObject(BLACK_BRUSH));

		// Draw Aero Basic titlebar
		if (g_theme == ThemeBasic && g_aeroTheme && $DrawThemeBackground) {
			int state = GetActiveWindow() == g_hwndParent ? AW_S_TITLEBAR_ACTIVE : AW_S_TITLEBAR_INACTIVE;
			$DrawThemeBackground(g_aeroTheme, hdc, AW_TITLEBAR, state, &rect, NULL);
		}

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
	}

	if (!g_bannerOrigWndProc) {
		return 0;
	}

	return CallWindowProc(g_bannerOrigWndProc, hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK BottomWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT: {
		// Draw command area background (grey with divider line)
		if (g_theme < ThemeBasic || !g_aeroTheme || !$DrawThemeBackground) {
			break;
		}

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT rect;
		GetClientRect(hwnd, &rect);
		$DrawThemeBackground(g_aeroTheme, hdc, AW_COMMANDAREA, 0, &rect, NULL);

		EndPaint(hwnd, &ps);
		return 0;
	}
	}

	if (!g_bottomOrigWndProc) {
		return 0;
	}

	return CallWindowProc(g_bottomOrigWndProc, hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (g_theme >= ThemeAeroLight && $DwmDefWindowProc) {
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

	case WM_ACTIVATE:
	case WM_NCACTIVATE:
		// Redraw banner on activation
		if (g_theme == ThemeBasic) {
			InvalidateRect(GetDlgItem(hwnd, 1046), NULL, FALSE);
		}
		break;

	case WM_NCHITTEST: {
		if (g_theme < ThemeBasic) {
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
	}

	if (!g_dialogOrigWndProc) {
		return 0;
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

	// Get symbols
	HMODULE dwmapi = LoadLibrary(L"dwmapi.dll");
	if (dwmapi) {
		$DwmExtendFrameIntoClientArea = (_DwmExtendFrameIntoClientArea)GetProcAddress(dwmapi, "DwmExtendFrameIntoClientArea");
		$DwmIsCompositionEnabled = (_DwmIsCompositionEnabled)GetProcAddress(dwmapi, "DwmIsCompositionEnabled");
		$DwmDefWindowProc = (_DwmDefWindowProc)GetProcAddress(dwmapi, "DwmDefWindowProc");
		$DwmSetWindowAttribute = (_DwmSetWindowAttribute)GetProcAddress(dwmapi, "DwmSetWindowAttribute");
	}

	HMODULE uxtheme = LoadLibrary(L"uxtheme.dll");
	if (uxtheme) {
		$SetWindowThemeAttribute = (_SetWindowThemeAttribute)GetProcAddress(uxtheme, "SetWindowThemeAttribute");
		$IsThemeActive = (_IsThemeActive)GetProcAddress(uxtheme, "IsThemeActive");
		$OpenThemeData = (_OpenThemeData)GetProcAddress(uxtheme, "OpenThemeData");
		$CloseThemeData = (_CloseThemeData)GetProcAddress(uxtheme, "CloseThemeData");
		$DrawThemeBackground = (_DrawThemeBackground)GetProcAddress(uxtheme, "DrawThemeBackground");
	}

	// Get AeroWizard theme
	if ($OpenThemeData) {
		g_aeroTheme = $OpenThemeData(g_hwndParent, L"AeroWizard");
	}

	// Hide title caption/icon
	if ($SetWindowThemeAttribute) {
		WTA_OPTIONS options;
		options.dwFlags = WTNCA_NODRAWCAPTION | WTNCA_NODRAWICON;
		options.dwMask = WTNCA_NODRAWCAPTION | WTNCA_NODRAWICON;
		$SetWindowThemeAttribute(g_hwndParent, WTA_NONCLIENT, &options, sizeof(options));
	}

	// Enable Acrylic blur
	if (AtLeastWin11_21H1() && $DwmSetWindowAttribute) {
		// I stole this undocumented 1029 attr from Microsoft Store's StoreInstaller.exe
		BOOL modern = AtLeastWin11_22H2();
		DWORD attr = modern ? DWMWA_SYSTEMBACKDROP_TYPE : 1029;
		DWORD value = modern ? DWMSBT_MAINWINDOW : 1;
		$DwmSetWindowAttribute(g_hwndParent, attr, &value, sizeof(value));

		// Hide caption background
		value = DWMWA_COLOR_NONE;
		$DwmSetWindowAttribute(g_hwndParent, DWMWA_CAPTION_COLOR, &value, sizeof(value));
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
