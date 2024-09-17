#pragma once

#include <windows.h>
#include <shellapi.h>

extern HRESULT Exec(LPWSTR verb, LPWSTR file, LPWSTR params, LPWSTR workingDir, WORD show, BOOL wait, LPDWORD exitCode);
extern HRESULT ExecEx(LPSHELLEXECUTEINFO execInfo, BOOL wait, LPDWORD exitCode);
