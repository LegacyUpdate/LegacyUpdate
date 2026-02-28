#pragma once

#include "Log.h"

#ifdef _DEBUG
	// Yeah, sue me lol
	#define TRACE(...) { \
		LPWSTR __traceMsg = (LPWSTR)LocalAlloc(LPTR, 4096 * sizeof(WCHAR)); \
		wsprintf(__traceMsg, L"%hs(%d): %hs: ", __FILE__, __LINE__, __func__); \
		wsprintf(__traceMsg + wcslen(__traceMsg), __VA_ARGS__); \
		MessageBox(NULL, __traceMsg, L"Debug", MB_OK); \
		LocalFree(__traceMsg); \
		LOG(__VA_ARGS__); \
	}
#else
	#define TRACE(...) LOG(__VA_ARGS__)
#endif

#define CHECK_HR_OR_RETURN(thing) \
	if (!SUCCEEDED(hr)) { \
		TRACE(thing L" failed: %08x", hr); \
		return hr; \
	}

#define CHECK_HR_OR_GOTO_END(thing) \
	if (!SUCCEEDED(hr)) { \
		TRACE(thing L" failed: %08x", hr); \
		goto end; \
	}
