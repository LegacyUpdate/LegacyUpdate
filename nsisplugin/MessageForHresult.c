#include <windows.h>
#include <nsis/pluginapi.h>
#include "../shared/HResult.h"

PLUGIN_METHOD(MessageForHresult) {
	PLUGIN_INIT();

	HRESULT hr = popint();
	if (hr == 0) {
		pushstring(L"Unknown error");
		return;
	}

	LPWSTR message = GetMessageForHresult(hr);
	pushstring(message);
	LocalFree(message);
}
