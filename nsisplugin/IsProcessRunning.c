#include <windows.h>
#include <nsis/pluginapi.h>
#include "main.h"
#include "GetPidForName.h"

PLUGIN_METHOD(IsProcessRunning) {
	PLUGIN_INIT();

	LPTSTR process = (LPTSTR)LocalAlloc(LPTR, string_size * sizeof(TCHAR));
	popstring(process);
	CharLowerBuffW(process, wcslen(process));

	int pid = GetPidForName(process);
	WCHAR buffer[SHRT_MAX];
	wsprintf(buffer, L"%d", pid);
	pushstring(buffer);

	LocalFree(process);
}
