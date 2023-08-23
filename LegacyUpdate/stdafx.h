#pragma once

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // some CString constructors will be explicit

#define ISOLATION_AWARE_ENABLED 1 // Enable comctl 6.0 (visual styles)

#include "resource.h"
#include <atltrace.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

using namespace ATL;

#define TRACE ATLTRACE
