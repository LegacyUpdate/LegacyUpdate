#include "stdafx.h"
#include "Gdip.h"

static BOOL g_gdiplusLoaded = FALSE;
static ULONG_PTR g_gdiplusToken;

typedef struct {
	LPCSTR name;
	FARPROC symbol;
} GdiplusSymbol;

static GdiplusSymbol symbols[] = {
	{"GdiplusStartup"},
	{"GdipCreateBitmapFromScan0"},
	{"GdipCreateBitmapFromStream"},
	{"GdipCreateFont"},
	{"GdipCreateFontFamilyFromName"},
	{"GdipCreateFontFromDC"},
	{"GdipCreateFontFromLogfontW"},
	{"GdipCreateFromHDC"},
	{"GdipCreateFromHDC2"},
	{"GdipCreateHBITMAPFromBitmap"},
	{"GdipCreateSolidFill"},
	{"GdipDeleteBrush"},
	{"GdipDeleteFont"},
	{"GdipDeleteFontFamily"},
	{"GdipDeleteGraphics"},
	{"GdipDeleteGraphics"},
	{"GdipDisposeImage"},
	{"GdipDrawImageRectRectI"},
	{"GdipDrawString"},
	{"GdipGetImageGraphicsContext"},
	{"GdipGetImageHeight"},
	{"GdipGetImageWidth"},
	{"GdipLoadImageFromFile"},
	{"GdipSaveImageToFile"},
	{"GdipSetSmoothingMode"},
};

static GpStatus LoadGdiplus() {
	if (g_gdiplusLoaded) {
		return Ok;
	}

	g_gdiplusLoaded = TRUE;

	HMODULE gdiplus = LoadLibrary(L"gdiplus.dll");
	if (!gdiplus) {
		return GetLastError();
	}

	for (int i = 0; i < ARRAYSIZE(symbols); i++) {
		symbols[i].symbol = GetProcAddress(gdiplus, symbols[i].name);
	}

	GdiplusStartupInput startup = {1};
	_GdiplusStartup $GdiplusStartup = (_GdiplusStartup)symbols[0].symbol;
	if (!$GdiplusStartup) {
		return GetLastError();
	}

	GpStatus status = $GdiplusStartup(&g_gdiplusToken, &startup, NULL);
	if (status != Ok) {
		TRACE(L"GdiplusStartup failed: %d", status);
		return status;
	}
}

FARPROC GetGdiplusSymbol(LPCSTR name) {
	if (LoadGdiplus() != Ok) {
		return NULL;
	}

	for (int i = 0; i < ARRAYSIZE(symbols); i++) {
		if (strcmp(symbols[i].name, name) == 0) {
			return symbols[i].symbol;
		}
	}

	return NULL;
}
