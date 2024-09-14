#include <windows.h>
#include <wincodec.h>
#include <gdiplus.h>

// Load bitmap from disk, ensuring to not lose 32-bit alpha channel
// HBITMAP LoadBitmapFromPath(LPWSTR path) {
// 	return (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
// }

HBITMAP LoadBitmapFromPath(LPWSTR filename) {
	HBITMAP hBitmap;
	BYTE *fileData;
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		goto end;
	}

	DWORD fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE) {
		goto end;
	}

	fileData = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fileSize);
	if (!fileData) {
		goto end;
	}

	DWORD bytesRead = 0;
	if (!ReadFile(hFile, fileData, fileSize, &bytesRead, NULL) || bytesRead != fileSize) {
		goto end;
	}

	BITMAPFILEHEADER *bmfHeader = (BITMAPFILEHEADER *)fileData;

	// Is it actually a bitmap?
	if (bmfHeader->bfType != 0x4D42) {
		goto end;
	}

	BITMAPINFOHEADER *bmiHeader = (BITMAPINFOHEADER *)(fileData + sizeof(BITMAPFILEHEADER));

	// Is it 32-bit?
	if (bmiHeader->biBitCount != 32) {
		goto end;
	}

	BITMAPINFO bmi;
	bmi.bmiHeader = *bmiHeader;

	void *pBits = NULL;
	hBitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

	if (!hBitmap) {
		goto end;
	}

	BYTE *pixelData = fileData + bmfHeader->bfOffBits;
	DWORD imageSize = bmiHeader->biWidth * abs(bmiHeader->biHeight) * 4;
	CopyMemory(pBits, pixelData, imageSize);

end:
	if (hFile) {
		CloseHandle(hFile);
	}
	if (fileData) {
		HeapFree(GetProcessHeap(), 0, fileData);
	}
	return hBitmap;
}

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
		return NULL;
	}

	DWORD resourceSize = SizeofResource(hInstance, resource);
	HGLOBAL imageHandle = LoadResource(hInstance, resource);
	if (!imageHandle) {
		return NULL;
	}

	LPVOID sourceResourceData = LockResource(imageHandle);
	if (!sourceResourceData) {
		return NULL;
	}

	HGLOBAL resourceDataHandle = GlobalAlloc(GMEM_MOVEABLE, resourceSize);
	if (!resourceDataHandle) {
		return NULL;
	}

	LPVOID resourceData = GlobalLock(resourceDataHandle);
	if (!resourceData) {
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

	if (!SUCCEEDED(CoCreateInstance(&CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, &IID_IWICBitmapDecoder, (LPVOID*)&decoder))) {
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
		return NULL;
	}

	IWICBitmapSource *bitmap = GetWICBitmap(imageStream);
	if (!bitmap) {
		IStream_Release(imageStream);
		return NULL;
	}

	HBITMAP result = GetHBitmapForWICBitmap(bitmap);
	IWICBitmapSource_Release(bitmap);
	IStream_Release(imageStream);
	return result;
}
