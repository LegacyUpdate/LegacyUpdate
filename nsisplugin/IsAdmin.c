#include <windows.h>
#include <nsis/pluginapi.h>
#include "User.h"

PLUGIN_METHOD(IsAdmin) {
	PLUGIN_INIT();

	pushint(IsUserAdmin());
}
