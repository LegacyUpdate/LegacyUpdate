#include <windows.h>
#include <nsis/pluginapi.h>
#include "main.h"
#include "GetPidForName.h"

EXTERN_C __declspec(dllexport)
void __cdecl IsProcessRunning(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra) {
	EXDLL_INIT();
	g_hwndParent = hwndParent;

	LPTSTR process = (LPTSTR)malloc(string_size * sizeof(TCHAR));
	popstring(process);
	CharLowerBuffW(process, wcslen(process));

	int pid = GetPidForName(process);
	WCHAR buffer[SHRT_MAX];
	wsprintf(buffer, L"%d", pid);
	pushstring(buffer);

	free(process);
}
