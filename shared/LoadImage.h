#include <windows.h>

HBITMAP LoadPNGResource(HINSTANCE hInstance, LPWSTR resourceName, LPWSTR resourceType);
HBITMAP LoadJPEGFile(LPWSTR filePath);
BOOL ScaleAndWriteToBMP(HBITMAP hBitmap, DWORD width, DWORD height, LPWSTR outputPath);
