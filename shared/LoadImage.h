#include <windows.h>

HBITMAP LoadPNGResource(HINSTANCE hInstance, LPCWSTR resourceName, LPCWSTR resourceType);
HBITMAP LoadJPEGFile(LPCWSTR filePath);
BOOL ScaleAndWriteToBMP(HBITMAP hBitmap, DWORD width, DWORD height, LPCWSTR outputPath);
