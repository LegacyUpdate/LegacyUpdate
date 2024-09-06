#pragma once

#include <windows.h>

HRESULT GetRegistryString(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, DWORD options, LPWSTR *data, LPDWORD size);
HRESULT GetRegistryDword(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, DWORD options, LPDWORD data);
