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
