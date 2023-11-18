#include <windows.h>
#include <nsis/pluginapi.h>

HMODULE g_hInstance;
HWND g_hwndParent;

EXTERN_C BOOL WINAPI DllMain(HMODULE hInstance, UINT iReason, LPVOID lpReserved) {
	g_hInstance = hInstance;
	return TRUE;
}
