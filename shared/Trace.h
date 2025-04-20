#pragma once

#if defined(ATLTRACE)
	#define TRACE ATLTRACE
#elif defined(_DEBUG)
	// Yeah, sue me lol
	#define TRACE(...) { \
		LPWSTR __traceMsg = (LPWSTR)LocalAlloc(LPTR, 4096 * sizeof(WCHAR)); \
		wsprintf(__traceMsg, L"%hs(%d): %hs: ", __FILE__, __LINE__, __func__); \
		wsprintf(__traceMsg + wcslen(__traceMsg), __VA_ARGS__); \
		MessageBox(NULL, __traceMsg, L"Debug", MB_OK); \
		LocalFree(__traceMsg); \
	}
#else
	#define TRACE(...)
#endif
