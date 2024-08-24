#include "stdafx.h"
#include <windows.h>
#include "WUErrors.h"

LPWSTR GetMessageForHresult(HRESULT hr) {
	LPWSTR message;
	for (int i = 0; WUErrorMessages[i].hr != 0; i++) {
		if (WUErrorMessages[i].hr == hr) {
			message = (LPWSTR)LocalAlloc(LPTR, 4096 * sizeof(WCHAR));
			lstrcpy(message, WUErrorMessages[i].message);
			return message;
		}
	}

	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&message, 0, NULL) == 0) {
		message = (LPWSTR)LocalAlloc(LPTR, 1024 * sizeof(WCHAR));
		wsprintf(message, L"Error 0x%08x", hr);
	}

	// Truncate trailing \r\n if any
	int len = lstrlen(message);
	if (len >= 2 && message[len - 2] == '\r' && message[len - 1] == '\n') {
		message[len - 2] = 0;
	}

	return message;
}
