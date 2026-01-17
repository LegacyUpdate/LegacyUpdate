#include <windows.h>
#include <nsis/pluginapi.h>

PLUGIN_METHOD(CheckCCPProgress) {
	PLUGIN_INIT();

	// Get the first two bytes of the 16 byte progress value in the registry, which correspond to the progress of the
	// current CBS operation. Not fully understood what these values mean, see wcp.dll and maybe also poqexec.exe in
	// Vista SP1 for implementation.
	HKEY key;
	LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"COMPONENTS\\CCPInterface", 0, KEY_READ, &key);
	if (result != ERROR_SUCCESS) {
		pushint(-1);
		return;
	}

	BYTE data[16];
	DWORD size = sizeof(data);
	DWORD type = 0;
	result = RegQueryValueEx(key, L"Progress", NULL, &type, data, &size);
	RegCloseKey(key);
	if (result != ERROR_SUCCESS || type != REG_BINARY || size < 2) {
		pushint(-1);
		return;
	}

	WORD value = *((WORD *)data);
	pushint(value);
}
