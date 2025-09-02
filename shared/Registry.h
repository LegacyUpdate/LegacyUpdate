#pragma once

#include <windows.h>

EXTERN_C LPVOID DELETE_KEY;
EXTERN_C LPVOID DELETE_VALUE;

typedef struct {
	HKEY hKey;
	LPCTSTR lpSubKey;
	LPCWSTR lpValueName;
	DWORD dwType;
	union {
		LPVOID lpData;
		DWORD dwData;
	} uData;
	DWORD dataSize;
	REGSAM samDesired;
} RegistryEntry;

EXTERN_C HRESULT GetRegistryString(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, REGSAM options, LPWSTR *data, LPDWORD size);
EXTERN_C HRESULT GetRegistryDword(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, REGSAM options, LPDWORD data);
EXTERN_C HRESULT SetRegistryEntries(RegistryEntry entries[]);
