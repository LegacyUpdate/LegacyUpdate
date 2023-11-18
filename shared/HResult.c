#include <windows.h>
#include <strsafe.h>
#include "WUErrors.h"

EXTERN_C LPWSTR GetMessageForHresult(HRESULT hr) {
	LPWSTR message;
	for (int i = 0; WUErrorMessages[i].hr != 0; i++) {
		if (WUErrorMessages[i].hr == hr) {
			message = (LPWSTR)LocalAlloc(LPTR, 4096 * sizeof(WCHAR));
			StringCchCopy(message, 4096, WUErrorMessages[i].message);
			return message;
		}
	}

	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&message, 0, NULL) == 0) {
		message = (LPWSTR)LocalAlloc(LPTR, 1024 * sizeof(WCHAR));
		StringCchPrintf(message, 1024, L"Error 0x%08x", hr);
	}

	return message;
}
