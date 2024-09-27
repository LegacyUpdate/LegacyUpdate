#pragma once

#ifndef STRICT
#define STRICT
#endif

#define WINVER       0x0500
#define _WIN32_WINNT 0x0500

// Enable comctl 6.0 (visual styles)
#define ISOLATION_AWARE_ENABLED 1

// Enable COM C interfaces
#define CINTERFACE
#define COBJMACROS
#define INITGUID

#include "resource.h"
#include <windows.h>
#include "Trace.h"

EXTERN_C HWND g_hwndParent;

#define PLUGIN_METHOD(name) \
	EXTERN_C __declspec(dllexport) \
	void __cdecl name(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra)

#define PLUGIN_INIT() \
	EXDLL_INIT(); \
	g_hwndParent = hwndParent;
