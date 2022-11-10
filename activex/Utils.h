#pragma once

OSVERSIONINFOEX *GetVersionInfo();

void GetOwnFileName(LPWSTR *filename, LPDWORD size);
HRESULT GetOwnVersion(LPWSTR *version, LPDWORD size);

HRESULT GetRegistryString(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, LPDWORD type, LPWSTR *data, LPDWORD size);
HRESULT GetRegistryDword(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, LPDWORD type, LPDWORD data);

LPWSTR GetMessageForHresult(HRESULT hresult);