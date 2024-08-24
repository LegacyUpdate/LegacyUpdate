#pragma once

#include <windows.h>
#include <stdio.h>
#include <comdef.h>

HRESULT QueryWMIProperty(BSTR query, BSTR property, VARIANT *value);
