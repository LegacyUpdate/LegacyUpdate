#pragma once

#include "Log.h"

#ifdef _DEBUG
	// Yeah, sue me lol
	#define TRACE(...) { \
		LPWSTR __traceMsg = (LPWSTR)LocalAlloc(LPTR, LOG_LINE_MAXLEN * sizeof(WCHAR)); \
		StringCchPrintf(__traceMsg, LOG_LINE_MAXLEN, L"%hs(%d): %hs: ", __FILE__, __LINE__, __func__); \
		int __tracePrefix = lstrlenW(__traceMsg); \
		StringCchPrintf(__traceMsg + __tracePrefix, LOG_LINE_MAXLEN - __tracePrefix, __VA_ARGS__); \
		MessageBox(NULL, __traceMsg, L"Debug", MB_OK); \
		LocalFree(__traceMsg); \
		LOG(__VA_ARGS__); \
	}
#else
	#define TRACE(...) LOG(__VA_ARGS__)
#endif

#define CHECK_HR(thing) \
	if (!SUCCEEDED(hr)) { \
		TRACE(thing L" failed: %08x", hr); \
	}

#define CHECK_HR_OR_RETURN(thing) \
	CHECK_HR(thing); \
	if (!SUCCEEDED(hr)) { \
		return hr; \
	}

#define CHECK_HR_OR_GOTO_END(thing) \
	CHECK_HR(thing); \
	if (!SUCCEEDED(hr)) { \
		goto end; \
	}
