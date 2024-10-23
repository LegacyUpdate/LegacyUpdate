#include <windows.h>
#include <nsis/pluginapi.h>
#include "Startup.h"

HINSTANCE g_hInstance;
HWND g_hwndParent;

EXTERN_C __declspec(dllexport)
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		g_hInstance = hInstance;
		Startup();
		break;

	case DLL_PROCESS_DETACH:
		g_hInstance = NULL;
		break;
	}

	return TRUE;
}
