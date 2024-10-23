#include <windows.h>
#include <gdiplus.h>

GpBitmap *LoadPNGResource(HINSTANCE hInstance, LPWSTR resourceName, LPWSTR resourceType);
BOOL ScaleAndWriteToBMP(GpBitmap *bitmap, DWORD width, DWORD height, LPWSTR outputPath);
