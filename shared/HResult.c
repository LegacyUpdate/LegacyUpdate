#include "stdafx.h"
#include <windows.h>
#include "HResult.h"
#include "LegacyUpdate.h"
#include "VersionInfo.h"

static BOOL g_loadedHModule;
static HMODULE g_messagesHModule;

EXTERN_C LPWSTR GetMessageForHresult(HRESULT hr) {
	if (!g_loadedHModule) {
		g_loadedHModule = TRUE;

		// Load messages table from main dll
		LPWSTR installPath;
		if (GetInstallPath(&installPath)) {
			WCHAR path[MAX_PATH];
			wsprintf(path, L"%ls\\LegacyUpdate.dll", installPath);
			g_messagesHModule = LoadLibrary(path);
			LocalFree(installPath);
		}

		if (!g_messagesHModule) {
			// Try the current module
			g_messagesHModule = (HMODULE)&__ImageBase;
		}
	}

	LPWSTR message;
	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, g_messagesHModule, hr, LANG_NEUTRAL, (LPWSTR)&message, 0, NULL) == 0) {
		message = (LPWSTR)LocalAlloc(LPTR, 1024 * sizeof(WCHAR));
		wsprintf(message, L"Error 0x%08X", hr);
		return message;
	}

	// Truncate trailing \r\n if any
	int len = lstrlen(message);
	if (len >= 2 && message[len - 2] == '\r' && message[len - 1] == '\n') {
		message[len - 2] = 0;
	}

	// Remove title part in braces
	if (len > 1 && message[0] == '{') {
		LPWSTR closeBrace = wcsstr(message, L"}\r\n");
		if (closeBrace) {
			message = closeBrace + 3;
		}
	}

	return message;
}
