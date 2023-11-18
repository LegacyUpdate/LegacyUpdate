#pragma once
#include <windows.h>

OSVERSIONINFOEX *GetVersionInfo();
BOOL IsOSVersionOrLater(DWORD major, DWORD minor);
BOOL IsOSVersionOrEarlier(DWORD major, DWORD minor);
