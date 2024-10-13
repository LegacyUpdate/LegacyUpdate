#include <windows.h>
#include <nsis/pluginapi.h>
#include "../shared/HResult.h"

PLUGIN_METHOD(MessageForHresult) {
	PLUGIN_INIT();

	LPWSTR str;
	popstring(str);

	// Handle non-numeric inputs
	for (int i = 0; str[i] != L'\0'; i++) {
		if (str[i] < L'0' || str[i] > L'9') {
			pushstring(L"Unknown error");
			return;
		}
	}

	HRESULT hr = nsishelper_str_to_ptr(str);
	LPWSTR message = GetMessageForHresult(hr);
	pushstring(message);
	LocalFree(message);
}
