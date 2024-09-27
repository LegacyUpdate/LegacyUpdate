#include <windows.h>
#include <nsis/pluginapi.h>
#include "../shared/HResult.h"

PLUGIN_METHOD(MessageForHresult) {
	PLUGIN_INIT();

	HRESULT hr = popint();
	LPWSTR message = GetMessageForHresult(hr);
	pushstring(message);
	LocalFree(message);
}
