#include "stdafx.h"
#include <psapi.h>
#include "GetPidForName.h"

#pragma comment(lib, "psapi.lib")

int GetPidForName(const LPTSTR process) {
	// Yes, eat all of our SHRTs!
	DWORD pids[SHRT_MAX];
	DWORD bytesReturned;
	if (!EnumProcesses(pids, sizeof(pids), &bytesReturned)) {
		return 0;
	}

	DWORD count = bytesReturned / sizeof(DWORD);
	for (DWORD i = 0; i < count; ++i) {
		HANDLE handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pids[i]);
		if (handle) {
			WCHAR path[MAX_PATH];
			if (GetModuleBaseName(handle, NULL, path, ARRAYSIZE(path))) {
				LPWSTR basename = wcsrchr(path, L'\\');
				if (basename != NULL) {
					basename += 1;
					CharLowerBuffW(basename, (DWORD)wcslen(basename));
					if (_wcsicmp(process, basename) == 0) {
						CloseHandle(handle);
						return pids[i];
					}
				}
			}
			CloseHandle(handle);
		}
	}

	return 0;
}
