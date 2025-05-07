#include <windows.h>
#include <nsis/pluginapi.h>
#include "User.h"

PLUGIN_METHOD(IsMultiCPU) {
	PLUGIN_INIT();

	SYSTEM_INFO systemInfo = {0};
	GetSystemInfo(&systemInfo);
	pushint(systemInfo.dwNumberOfProcessors > 1 ? 1 : 0);
}
