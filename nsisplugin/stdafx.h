#pragma once

#ifndef STRICT
#define STRICT
#endif

#define WINVER       _WIN32_WINNT_NT4
#define _WIN32_WINNT _WIN32_WINNT_NT4

// Use msvcrt stdio functions
#define __USE_MINGW_ANSI_STDIO 0

// Enable COM C interfaces
#define CINTERFACE
#define COBJMACROS
#define INITGUID

#include "resource.h"
#include <windows.h>
#include "Trace.h"

EXTERN_C HWND g_hwndParent;

#define NSIS_MAX_STRLEN 8192

#define PLUGIN_METHOD(name) \
	EXTERN_C __declspec(dllexport) \
	void __cdecl name(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra)

#define PLUGIN_INIT() \
	if (extra && extra->exec_flags && (extra->exec_flags->plugin_api_version != NSISPIAPIVER_CURR)) { \
		return; \
	} \
	EXDLL_INIT(); \
	g_hwndParent = hwndParent;
