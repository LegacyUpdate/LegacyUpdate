#include <windows.h>
#include <wincodec.h>
#include <gdiplus.h>

// Adapted from https://faithlife.codes/blog/2008/09/displaying_a_splash_screen_with_c_part_i/
//  1. Get resource as an IStream
//  2. Use Windows Imaging Component to load the PNG
//  3. Convert the WIC bitmap to an HBITMAP
// Why does loading a PNG need to be so difficult?

typedef HRESULT (WINAPI *_WICConvertBitmapSource)(REFWICPixelFormatGUID dstFormat, IWICBitmapSource *pISrc, IWICBitmapSource **ppIDst);

static _WICConvertBitmapSource $WICConvertBitmapSource;

static IStream *GetResourceStream(HINSTANCE hInstance, LPWSTR name, LPWSTR type) {
	IStream *stream;
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
	if (!LockResource) {
		TRACE(L"FindResource failed: %d", GetLastError());
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

	if (SUCCEEDED(CreateStreamOnHGlobal(resourceDataHandle, TRUE, &stream))) {
		return stream;
	}

	GlobalFree(resourceDataHandle);
	return NULL;
}

static IWICBitmapSource *GetWICBitmap(IStream *imageStream) {
	IWICBitmapSource *bitmap;
	IWICBitmapDecoder *decoder;
	UINT frameCount;
	IWICBitmapFrameDecode *frame;

	if (!SUCCEEDED(CoCreateInstance(&CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, &IID_IWICBitmapDecoder, (LPVOID *)&decoder))) {
		return NULL;
	}

	if (!SUCCEEDED(IWICBitmapDecoder_Initialize(decoder, imageStream, WICDecodeMetadataCacheOnLoad))) {
		goto end;
	}

	if (!SUCCEEDED(IWICBitmapDecoder_GetFrameCount(decoder, &frameCount)) || frameCount != 1) {
		goto end;
	}

	if (!SUCCEEDED(IWICBitmapDecoder_GetFrame(decoder, 0, &frame))) {
		goto end;
	}

	$WICConvertBitmapSource(&GUID_WICPixelFormat32bppPBGRA, (IWICBitmapSource *)frame, &bitmap);
	IWICBitmapFrameDecode_Release(frame);

end:
	IWICBitmapDecoder_Release(decoder);
	return bitmap;
}

static HBITMAP GetHBitmapForWICBitmap(IWICBitmapSource *bitmap) {
	HBITMAP hBitmap;
	UINT width, height;
	if (!SUCCEEDED(IWICBitmapSource_GetSize(bitmap, &width, &height)) || width == 0 || height == 0) {
		return NULL;
	}

	BITMAPINFO bminfo;
	ZeroMemory(&bminfo, sizeof(bminfo));
	bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bminfo.bmiHeader.biWidth = width;
	bminfo.bmiHeader.biHeight = -(LONG)height;
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;

	void *imageBits;
	HDC screenDC = GetDC(NULL);
	hBitmap = CreateDIBSection(screenDC, &bminfo, DIB_RGB_COLORS, &imageBits, NULL, 0);
	ReleaseDC(NULL, screenDC);
	if (!hBitmap) {
		return NULL;
	}

	UINT stride = width * 4;
	UINT imageSize = stride * height;
	if (!SUCCEEDED(IWICBitmapSource_CopyPixels(bitmap, NULL, stride, imageSize, (BYTE*)imageBits))) {
		DeleteObject(hBitmap);
		return NULL;
	}

	return hBitmap;
}

HBITMAP LoadPNGResource(HINSTANCE hInstance, LPWSTR resourceName, LPWSTR resourceType) {
	if (!$WICConvertBitmapSource) {
		$WICConvertBitmapSource = (_WICConvertBitmapSource)GetProcAddress(LoadLibrary(L"windowscodecs.dll"), "WICConvertBitmapSource");
		if (!$WICConvertBitmapSource) {
			return NULL;
		}
	}

	IStream *imageStream = GetResourceStream(hInstance, resourceName, resourceType);
	if (!imageStream) {
		TRACE(L"GetResourceStream failed: %d", GetLastError());
		return NULL;
	}

	IWICBitmapSource *bitmap = GetWICBitmap(imageStream);
	if (!bitmap) {
		TRACE(L"GetWICBitmap failed: %d", GetLastError());
		IStream_Release(imageStream);
		return NULL;
	}

	HBITMAP result = GetHBitmapForWICBitmap(bitmap);
	IWICBitmapSource_Release(bitmap);
	IStream_Release(imageStream);
	return result;
}
