#pragma once

#include <windows.h>

// Same as NSIS_MAX_STRLEN
#define LOG_LINE_MAXLEN 4096

EXTERN_C HRESULT OpenLog();
EXTERN_C void CloseLog(void);
EXTERN_C void LogInternal(LPCWSTR text);

#define LOG(...) { \
	LPWSTR _logMsg = (LPWSTR)LocalAlloc(LPTR, LOG_LINE_MAXLEN * sizeof(WCHAR)); \
	wsprintf(_logMsg, __VA_ARGS__); \
	LogInternal(_logMsg); \
	LocalFree(_logMsg); \
}
