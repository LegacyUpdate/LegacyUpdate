#pragma once

#ifndef STRICT
#define STRICT
#endif

#define WINVER       _WIN32_WINNT_WIN2K
#define _WIN32_WINNT _WIN32_WINNT_WIN2K

// Use msvcrt stdio functions
#define __USE_MINGW_ANSI_STDIO 0

// Enable COM C interfaces
#define CINTERFACE
#define COBJMACROS
#define INITGUID

#include "resource.h"
#include <windows.h>
#include "Trace.h"
