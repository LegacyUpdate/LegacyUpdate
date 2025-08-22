#pragma once

#include <windows.h>
#include <shellapi.h>

EXTERN_C HRESULT Exec(LPCWSTR verb, LPCWSTR file, LPCWSTR params, LPCWSTR workingDir, int show, BOOL wait, LPDWORD exitCode);
EXTERN_C HRESULT ExecEx(LPSHELLEXECUTEINFO execInfo, BOOL wait, LPDWORD exitCode);
