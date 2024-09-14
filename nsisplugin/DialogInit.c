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

#define WM_DWMCOMPOSITIONCHANGED 0x031e

#define IDI_APPICON              103
#define IDI_BANNER_WORDMARK_AERO 1337

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
static Theme g_theme = ThemeUnknown;
static WNDPROC g_dialogOrigWndProc;
static WNDPROC g_bannerOrigWndProc;

static void DrawGradient(HDC hdc, RECT rect, COLORREF startColor, COLORREF endColor) {
	TRIVERTEX vertex[2];

	vertex[0].x     = rect.left;
	vertex[0].y     = rect.top;
	vertex[0].Red   = GetRValue(startColor) << 8;
	vertex[0].Green = GetGValue(startColor) << 8;
	vertex[0].Blue  = GetBValue(startColor) << 8;
	vertex[0].Alpha = 0;

	vertex[1].x     = rect.right;
	vertex[1].y     = rect.bottom;
	vertex[1].Red   = GetRValue(endColor) << 8;
	vertex[1].Green = GetGValue(endColor) << 8;
	vertex[1].Blue  = GetBValue(endColor) << 8;
	vertex[1].Alpha = 0;

	GRADIENT_RECT gradientRect = {0, 1};
	GdiGradientFill(hdc, vertex, 2, &gradientRect, 1, GRADIENT_FILL_RECT_H);
}

static Theme GetTheme() {
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
	if (!bannerWindow || !bannerDivider) {
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

		ShowWindow(bannerDivider, theme == ThemeClassic ? SW_SHOW : SW_HIDE);

		if (g_theme == ThemeAero) {
			g_bannerWordmark = LoadPNGResource(NULL, MAKEINTRESOURCE(IDI_BANNER_WORDMARK_AERO), L"PNG");
		} else {
			LPWSTR logoPath;
			DWORD logoPathSize;
			GetOwnFileName(&logoPath, &logoPathSize);
			wcsrchr(logoPath, L'\\')[1] = L'\0';
			wcsncat(logoPath, L"banner.bmp", ARRAYSIZE(logoPath) - wcslen(logoPath) - 1);
			g_bannerWordmark = LoadBitmapFromPath(logoPath);
		}
	}
}

static LRESULT CALLBACK BannerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rect;
		GetClientRect(hwnd, &rect);

		// Draw background
		if (g_theme != ThemeAero) {
			HIGHCONTRAST highContrast;
			highContrast.cbSize = sizeof(highContrast);
			SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, 0);

			if (GetDeviceCaps(hdc, BITSPIXEL) <= 4 || (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0) {
				// Fallback for low color and high contrast modes
				HBRUSH brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
				FillRect(hdc, &ps.rcPaint, brush);
				DeleteBrush(brush);
			} else {
				DrawGradient(hdc, rect, RGB(255, 255, 255), RGB(182, 197, 238));
			}
		}

		// Draw wordmark with alpha blending
		if (g_bannerWordmark) {
			HDC hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, g_bannerWordmark);

			BITMAP bitmap;
			GetObject(g_bannerWordmark, sizeof(bitmap), &bitmap);

			int x = (rect.right - rect.left - bitmap.bmWidth) / 2;
			int y = (rect.bottom - rect.top - bitmap.bmHeight) / 2;

			BLENDFUNCTION blendFunc;
			blendFunc.BlendOp = AC_SRC_OVER;
			blendFunc.BlendFlags = 0;
			blendFunc.SourceConstantAlpha = 0xFF;
			blendFunc.AlphaFormat = AC_SRC_ALPHA;

			AlphaBlend(hdc,
				x, y, bitmap.bmWidth, bitmap.bmHeight, hdcMem,
				0, 0, bitmap.bmWidth, bitmap.bmHeight, blendFunc);

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

static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (!g_dialogOrigWndProc) {
		return 0;
	}

	LRESULT lRet = 0;
	if ($DwmDefWindowProc && $DwmDefWindowProc(hwnd, uMsg, wParam, lParam, &lRet)) {
		return lRet;
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

EXTERN_C __declspec(dllexport)
void __cdecl DialogInit(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra) {
	EXDLL_INIT();
	g_hwndParent = hwndParent;

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
	g_bannerOrigWndProc = (WNDPROC)SetWindowLongPtr(bannerWindow, GWLP_WNDPROC, (LONG_PTR)BannerWndProc);
	g_dialogOrigWndProc = (WNDPROC)SetWindowLongPtr(g_hwndParent, GWLP_WNDPROC, (LONG_PTR)MainWndProc);
}
