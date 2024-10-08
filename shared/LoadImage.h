#include <windows.h>

HBITMAP LoadPNGResource(HINSTANCE hInstance, LPWSTR resourceName, LPWSTR resourceType);
BOOL WritePNGResourceToBMP(HINSTANCE hInstance, LPWSTR resourceName, LPWSTR resourceType, LPWSTR outputPath);
