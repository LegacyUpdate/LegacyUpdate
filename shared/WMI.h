#pragma once

#include <windows.h>
#include <oaidl.h>

EXTERN_C HRESULT QueryWMIProperty(LPWSTR query, LPWSTR property, LPVARIANT value);
