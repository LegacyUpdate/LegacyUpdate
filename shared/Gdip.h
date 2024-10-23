#pragma once

#include <windows.h>
#include <gdiplus.h>

typedef GpStatus (WINAPI *_GdiplusStartup)(ULONG_PTR *, GDIPCONST GdiplusStartupInput *, GdiplusStartupOutput *);
typedef GpStatus (WINAPI *_GdipCreateBitmapFromScan0)(INT, INT, INT, PixelFormat, BYTE *, GpBitmap **);
typedef GpStatus (WINAPI *_GdipCreateBitmapFromStream)(IStream *, GpBitmap **);
typedef GpStatus (WINAPI *_GdipCreateFont)(GDIPCONST GpFontFamily *, REAL, INT, Unit, GpFont **);
typedef GpStatus (WINAPI *_GdipCreateFontFamilyFromName)(GDIPCONST WCHAR *, GpFontCollection *, GpFontFamily **);
typedef GpStatus (WINAPI *_GdipCreateFontFromDC)(HDC, GpFont **);
typedef GpStatus (WINAPI *_GdipCreateFontFromLogfontW)(HDC, GDIPCONST LOGFONTW *, GpFont **);
typedef GpStatus (WINAPI *_GdipCreateFromHDC)(HDC, GpGraphics **);
typedef GpStatus (WINAPI *_GdipCreateFromHDC2)(HDC, HANDLE, GpGraphics **);
typedef GpStatus (WINAPI *_GdipCreateHBITMAPFromBitmap)(GpBitmap *, HBITMAP *, ARGB);
typedef GpStatus (WINAPI *_GdipCreateSolidFill)(ARGB, GpSolidFill **);
typedef GpStatus (WINAPI *_GdipDeleteBrush)(GpBrush *);
typedef GpStatus (WINAPI *_GdipDeleteFont)(GpFont *);
typedef GpStatus (WINAPI *_GdipDeleteFontFamily)(GpFontFamily *);
typedef GpStatus (WINAPI *_GdipDeleteGraphics)(GpGraphics *);
typedef GpStatus (WINAPI *_GdipDeleteGraphics)(GpGraphics *);
typedef GpStatus (WINAPI *_GdipDisposeImage)(GpImage *);
typedef GpStatus (WINAPI *_GdipDrawImageRectRectI)(GpGraphics *, GpImage *, INT, INT, INT, INT, INT, INT, INT, INT, GpUnit, GDIPCONST GpImageAttributes *, DrawImageAbort, VOID *);
typedef GpStatus (WINAPI *_GdipDrawString)(GpGraphics *, GDIPCONST WCHAR *, INT, GDIPCONST GpFont *, GDIPCONST RectF *, GDIPCONST GpStringFormat *, GDIPCONST GpBrush *);
typedef GpStatus (WINAPI *_GdipGetImageGraphicsContext)(GpImage *, GpGraphics **);
typedef GpStatus (WINAPI *_GdipGetImageHeight)(GpImage *, UINT *);
typedef GpStatus (WINAPI *_GdipGetImageWidth)(GpImage *, UINT *);
typedef GpStatus (WINAPI *_GdipLoadImageFromFile)(GDIPCONST WCHAR*, GpImage **);
typedef GpStatus (WINAPI *_GdipSaveImageToFile)(GpImage *, GDIPCONST WCHAR *, GDIPCONST CLSID *, GDIPCONST EncoderParameters *);
typedef GpStatus (WINAPI *_GdipSetSmoothingMode)(GpGraphics *, SmoothingMode);

FARPROC GetGdiplusSymbol(LPCSTR name);
