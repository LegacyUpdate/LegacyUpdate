#include <windows.h>
#include <wincodec.h>
#include <gdiplus.h>

// Adapted from https://faithlife.codes/blog/2008/09/displaying_a_splash_screen_with_c_part_i/
//  1. Get resource as an IStream
//  2. Use Windows Imaging Component to load the PNG
//  3. Convert the WIC bitmap to an HBITMAP
// Why does loading a PNG need to be so difficult?

typedef HRESULT (WINAPI *_WICConvertBitmapSource)(REFWICPixelFormatGUID dstFormat, IWICBitmapSource *pISrc, IWICBitmapSource **ppIDst);
typedef HRESULT (WINAPI *_SHCreateStreamOnFileEx)(LPCWSTR pszFile, DWORD grfMode, DWORD dwAttributes, BOOL fCreate, IStream *pstmTemplate, IStream **ppstm);

static _WICConvertBitmapSource $WICConvertBitmapSource;
static _SHCreateStreamOnFileEx $SHCreateStreamOnFileEx;

static HGLOBAL GetRawResource(HINSTANCE hInstance, LPCWSTR name, LPCWSTR type) {
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

static IStream *GetResourceStream(HINSTANCE hInstance, LPCWSTR name, LPCWSTR type) {
	IStream *stream = NULL;
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

static IWICBitmapSource *GetWICBitmap(IStream *imageStream, REFCLSID rclsid) {
	IWICBitmapSource *bitmap = NULL;
	IWICBitmapDecoder *decoder = NULL;
	UINT frameCount = 0;
	IWICBitmapFrameDecode *frame = NULL;

	if (!SUCCEEDED(CoCreateInstance(rclsid, NULL, CLSCTX_INPROC_SERVER, &IID_IWICBitmapDecoder, (LPVOID *)&decoder))) {
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
	HBITMAP hBitmap = NULL;
	UINT width = 0, height = 0;
	if (!SUCCEEDED(IWICBitmapSource_GetSize(bitmap, &width, &height)) || width == 0 || height == 0) {
		return NULL;
	}

	BITMAPINFO bminfo = {0};
	ZeroMemory(&bminfo, sizeof(bminfo));
	bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bminfo.bmiHeader.biWidth = width;
	bminfo.bmiHeader.biHeight = -(LONG)height;
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;

	void *imageBits = NULL;
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

HBITMAP LoadPNGResource(HINSTANCE hInstance, LPCWSTR resourceName, LPCWSTR resourceType) {
	if (!$WICConvertBitmapSource) {
		HMODULE windowscodecs = LoadLibrary(L"windowscodecs.dll");
		if (windowscodecs) {
			$WICConvertBitmapSource = (_WICConvertBitmapSource)GetProcAddress(windowscodecs, "WICConvertBitmapSource");
		}
		if (!$WICConvertBitmapSource) {
			return NULL;
		}
	}

	IStream *imageStream = GetResourceStream(hInstance, resourceName, resourceType);
	if (!imageStream) {
		TRACE(L"GetResourceStream failed: %d", GetLastError());
		return NULL;
	}

	IWICBitmapSource *bitmap = GetWICBitmap(imageStream, &CLSID_WICPngDecoder);
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

HBITMAP LoadJPEGFile(LPCWSTR filePath) {
	if (!$WICConvertBitmapSource) {
		HMODULE windowscodecs = LoadLibrary(L"windowscodecs.dll");
		if (windowscodecs) {
			$WICConvertBitmapSource = (_WICConvertBitmapSource)GetProcAddress(windowscodecs, "WICConvertBitmapSource");
		}
		if (!$WICConvertBitmapSource) {
			return NULL;
		}
	}

	if (!$SHCreateStreamOnFileEx) {
		HMODULE shlwapi = LoadLibrary(L"shlwapi.dll");
		if (shlwapi) {
			$SHCreateStreamOnFileEx = (_SHCreateStreamOnFileEx)GetProcAddress(shlwapi, "SHCreateStreamOnFileEx");
		}
		if (!$SHCreateStreamOnFileEx) {
			return NULL;
		}
	}

	IStream *imageStream = NULL;
	HRESULT hr = $SHCreateStreamOnFileEx(filePath, STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &imageStream);
	if (!SUCCEEDED(hr)) {
		TRACE(L"SHCreateStreamOnFileEx failed: 0x%08x", hr);
		return NULL;
	}

	IWICBitmapSource *bitmap = GetWICBitmap(imageStream, &CLSID_WICJpegDecoder);
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

BOOL ScaleAndWriteToBMP(HBITMAP hBitmap, DWORD width, DWORD height, LPCWSTR outputPath) {
	BOOL result = FALSE;
	if (!hBitmap) {
		TRACE(L"Null bitmap");
		return FALSE;
	}

	HDC hdc = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdc);
	HDC hdcMemScaled = NULL;
	HGLOBAL handle = NULL;
	HANDLE file = INVALID_HANDLE_VALUE;

	HBITMAP scaledBitmap = CreateCompatibleBitmap(hdc, width, height);
	if (!scaledBitmap) {
		TRACE(L"CreateCompatibleBitmap failed: %d", GetLastError());
		goto end;
	}

	BITMAP bmp = {0};
	if (!GetObject(hBitmap, sizeof(BITMAP), &bmp)) {
		TRACE(L"GetObject failed: %d", GetLastError());
		goto end;
	}

	hdcMemScaled = CreateCompatibleDC(hdc);
	SetStretchBltMode(hdcMemScaled, HALFTONE);

	if (!StretchBlt(hdcMemScaled,
		0, 0, width, height, hdcMem,
		0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY)) {
		TRACE(L"StretchBlt failed: %d", GetLastError());
		goto end;
	}

	BITMAPINFOHEADER bmih = {0};
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biPlanes = 1;
	bmih.biBitCount = bmp.bmBitsPixel;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = ((width * bmp.bmBitsPixel + 31) / 32) * 4 * height;

	BITMAPFILEHEADER bmfh = {0};
	bmfh.bfType = 0x4D42;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfh.bfSize = bmfh.bfOffBits + bmih.biSizeImage;

	handle = GlobalAlloc(GMEM_MOVEABLE, bmih.biSizeImage);
	if (!handle) {
		TRACE(L"GlobalAlloc failed: %d", GetLastError());
		goto end;
	}

	BYTE *bitmapData = (BYTE *)GlobalLock(handle);
	if (!bitmapData) {
		TRACE(L"GlobalLock failed: %d", GetLastError());
		goto end;
	}

	if (!GetDIBits(hdcMemScaled, scaledBitmap, 0, height, bitmapData, (BITMAPINFO *)&bmih, DIB_RGB_COLORS)) {
		TRACE(L"GetDIBits failed: %d", GetLastError());
		goto end;
	}

	file = CreateFile(outputPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		TRACE(L"CreateFile failed: %d", GetLastError());
		goto end;
	}

	DWORD written = 0;
	if (!WriteFile(file, &bmfh, sizeof(BITMAPFILEHEADER), &written, NULL) ||
		!WriteFile(file, &bmih, sizeof(BITMAPINFOHEADER), &written, NULL) ||
		!WriteFile(file, bitmapData, bmih.biSizeImage, &written, NULL)) {
		TRACE(L"WriteFile failed: %d", GetLastError());
		goto end;
	}

	result = TRUE;

end:
	if (file != INVALID_HANDLE_VALUE) {
		CloseHandle(file);
	}
	if (scaledBitmap) {
		DeleteObject(scaledBitmap);
	}
	if (handle) {
		GlobalUnlock(handle);
		GlobalFree(handle);
	}
	if (hdc) {
		ReleaseDC(NULL, hdc);
	}
	if (hdcMem) {
		DeleteDC(hdcMem);
	}
	if (hdcMemScaled) {
		DeleteDC(hdcMemScaled);
	}

	return result;
}
