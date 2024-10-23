#include <windows.h>
#include <wincodec.h>
#include <gdiplus.h>
#include "Gdip.h"

DEFINE_GUID(CLSID_EncoderBMP, 0x557cf400, 0x1a04, 0x11d3, 0x9a, 0x73, 0x00, 0xc0, 0x4f, 0x8e, 0x38, 0x82);

static HGLOBAL GetRawResource(HINSTANCE hInstance, LPWSTR name, LPWSTR type) {
	HRSRC resource = FindResource(hInstance, name, type);
	if (!resource) {
		TRACE(L"FindResource failed: %d", GetLastError());
		return NULL;
	}

	DWORD resourceSize = SizeofResource(hInstance, resource);
	HGLOBAL imageHandle = LoadResource(hInstance, resource);
	if (!imageHandle) {
		TRACE(L"LoadResource failed: %d", GetLastError());
		return NULL;
	}

	LPVOID sourceResourceData = LockResource(imageHandle);
	if (!sourceResourceData) {
		TRACE(L"LockResource failed: %d", GetLastError());
		return NULL;
	}

	HGLOBAL resourceDataHandle = GlobalAlloc(GMEM_MOVEABLE, resourceSize);
	if (!resourceDataHandle) {
		TRACE(L"GlobalAlloc failed: %d", GetLastError());
		return NULL;
	}

	LPVOID resourceData = GlobalLock(resourceDataHandle);
	if (!resourceData) {
		TRACE(L"GlobalLock failed: %d", GetLastError());
		GlobalFree(resourceDataHandle);
		return NULL;
	}

	CopyMemory(resourceData, sourceResourceData, resourceSize);
	GlobalUnlock(resourceDataHandle);
	return resourceDataHandle;
}

static IStream *GetResourceStream(HINSTANCE hInstance, LPWSTR name, LPWSTR type) {
	IStream *stream;
	HGLOBAL resource = GetRawResource(hInstance, name, type);
	if (!resource) {
		TRACE(L"GetResource failed: %d", GetLastError());
		return NULL;
	}

	if (SUCCEEDED(CreateStreamOnHGlobal(resource, TRUE, &stream))) {
		return stream;
	}

	GlobalFree(resource);
	return NULL;
}

GpBitmap *LoadPNGResource(HINSTANCE hInstance, LPWSTR resourceName, LPWSTR resourceType) {
	IStream *imageStream = GetResourceStream(hInstance, resourceName, resourceType);
	if (!imageStream) {
		TRACE(L"GetResourceStream failed: %d", GetLastError());
		return NULL;
	}

	GpBitmap *bitmap;
	_GdipCreateBitmapFromStream $GdipCreateBitmapFromStream = (_GdipCreateBitmapFromStream)GetGdiplusSymbol("GdipCreateBitmapFromStream");
	if (!$GdipCreateBitmapFromStream) {
		TRACE(L"gdip symbols not available");
		goto end;
	}

	if ($GdipCreateBitmapFromStream(imageStream, &bitmap) != Ok) {
		TRACE(L"GdipCreateBitmapFromStream failed: %d", GetLastError());
		bitmap = NULL;
		goto end;
	}

end:
	IStream_Release(imageStream);
	return bitmap;
}

BOOL ScaleAndWriteToBMP(GpBitmap *bitmap, DWORD width, DWORD height, LPWSTR outputPath) {
	BOOL result = FALSE;
	if (!bitmap) {
		TRACE(L"Null bitmap");
		return FALSE;
	}

	_GdipCreateBitmapFromScan0 $GdipCreateBitmapFromScan0 = (_GdipCreateBitmapFromScan0)GetGdiplusSymbol("GdipCreateBitmapFromScan0");
	_GdipDeleteGraphics $GdipDeleteGraphics = (_GdipDeleteGraphics)GetGdiplusSymbol("GdipDeleteGraphics");
	_GdipDisposeImage $GdipDisposeImage = (_GdipDisposeImage)GetGdiplusSymbol("GdipDisposeImage");
	_GdipDrawImageRectRectI $GdipDrawImageRectRectI = (_GdipDrawImageRectRectI)GetGdiplusSymbol("GdipDrawImageRectRectI");
	_GdipGetImageGraphicsContext $GdipGetImageGraphicsContext = (_GdipGetImageGraphicsContext)GetGdiplusSymbol("GdipGetImageGraphicsContext");
	_GdipGetImageHeight $GdipGetImageHeight = (_GdipGetImageHeight)GetGdiplusSymbol("GdipGetImageHeight");
	_GdipGetImageWidth $GdipGetImageWidth = (_GdipGetImageWidth)GetGdiplusSymbol("GdipGetImageWidth");
	_GdipSaveImageToFile $GdipSaveImageToFile = (_GdipSaveImageToFile)GetGdiplusSymbol("GdipSaveImageToFile");

	if (!$GdipCreateBitmapFromScan0 || !$GdipDeleteGraphics || !$GdipDisposeImage || !$GdipDrawImageRectRectI || !$GdipGetImageGraphicsContext || !$GdipGetImageHeight || !$GdipGetImageWidth || !$GdipSaveImageToFile) {
		TRACE(L"gdip symbols not available");
		return FALSE;
	}

	GpGraphics *graphics;
	GpStatus status;
	status = $GdipGetImageGraphicsContext(bitmap, &graphics);
	if (status != Ok) {
		TRACE(L"GdipGetImageGraphicsContext failed: %d", status);
		goto end;
	}

	UINT originalWidth, originalHeight;
	$GdipGetImageWidth(bitmap, &originalWidth);
	$GdipGetImageHeight(bitmap, &originalHeight);

	GpBitmap *scaledBitmap;
	status = $GdipCreateBitmapFromScan0(width, height, 0, PixelFormat32bppARGB, NULL, &scaledBitmap);
	if (status != Ok) {
		TRACE(L"GdipCreateBitmapFromScan0 failed: %d", status);
		goto end;
	}

	GpGraphics *scaledGraphics;
	status = $GdipGetImageGraphicsContext(scaledBitmap, &scaledGraphics);
	if (status != Ok) {
		TRACE(L"GdipGetImageGraphicsContext for scaledBitmap failed: %d", status);
		goto end;
	}

	status = $GdipDrawImageRectRectI(scaledGraphics, bitmap,
		0, 0, width, height,
		0, 0, originalWidth, originalHeight,
		UnitPixel, NULL, NULL, NULL);
	if (status != Ok) {
		TRACE(L"GdipDrawImageRectRectI failed: %d", status);
		goto end;
	}

	status = $GdipSaveImageToFile(scaledBitmap, outputPath, &CLSID_EncoderBMP, NULL);
	if (status != Ok) {
		TRACE(L"GdipSaveImageToFile failed: %d", status);
		goto end;
	}

	$GdipDeleteGraphics(scaledGraphics);

	result = TRUE;

end:
	if (graphics) {
		$GdipDeleteGraphics(graphics);
	}
	if (scaledBitmap) {
		$GdipDisposeImage(scaledBitmap);
	}
	return result;
}
