#pragma once

#include <windows.h>
#include <shellapi.h>

EXTERN_C HRESULT Exec(LPWSTR verb, LPWSTR file, LPWSTR params, LPWSTR workingDir, WORD show, BOOL wait, LPDWORD exitCode);
EXTERN_C HRESULT ExecEx(LPSHELLEXECUTEINFO execInfo, BOOL wait, LPDWORD exitCode);
