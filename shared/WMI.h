#pragma once

#include <windows.h>
#include <oaidl.h>

EXTERN_C HRESULT QueryWMIProperty(LPCWSTR query, LPCWSTR property, LPVARIANT value);
