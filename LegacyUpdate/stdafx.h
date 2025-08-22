#pragma once

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#define STRICT

#define WINVER       _WIN32_WINNT_WIN2K
#define _WIN32_WINNT _WIN32_WINNT_WIN2K

// Use msvcrt stdio functions
#define __USE_MINGW_ANSI_STDIO 0

#include "resource.h"
#include "Trace.h"
#include <windows.h>
