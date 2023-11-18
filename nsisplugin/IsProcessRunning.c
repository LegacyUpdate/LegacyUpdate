#include <windows.h>
#include <nsis/pluginapi.h>
#include <psapi.h>
#include "main.h"

EXTERN_C __declspec(dllexport)
void __cdecl IsProcessRunning(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra) {
	EXDLL_INIT();
	g_hwndParent = hwndParent;

	LPTSTR process = (LPTSTR)malloc(string_size * sizeof(TCHAR));
	popstring(process);
	CharLowerBuffW(process, wcslen(process));

	// Yes, eat all of our SHRTs!
	DWORD pids[SHRT_MAX];
	DWORD bytesReturned;
	if (!EnumProcesses(pids, sizeof(pids), &bytesReturned)) {
		pushstring(L"0");
		return;
	}

	DWORD count = bytesReturned / sizeof(DWORD);
	for (DWORD i = 0; i < count; ++i) {
		HANDLE handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pids[i]);
		if (handle) {
			WCHAR path[MAX_PATH];
			if (GetProcessImageFileName(handle, path, ARRAYSIZE(path))) {
				LPWSTR basename = wcsrchr(path, L'\\');
				if (basename != NULL) {
					basename += 1;
					CharLowerBuffW(basename, wcslen(basename));
					if (_wcsicmp(process, basename) == 0) {
						pushstring(L"1");
						CloseHandle(handle);
						return;
					}
				}
			}
			CloseHandle(handle);
		}
	}

	pushstring(L"0");
}
