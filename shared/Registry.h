#pragma once

#include <windows.h>

#define REGPATH_SETUP              L"System\\Setup"

#define REGPATH_CPL_DESKTOP        L"Control Panel\\Desktop"

#define REGPATH_WIN                L"Software\\Microsoft\\Windows\\CurrentVersion"
#define REGPATH_WIN_PERSONALIZE    REGPATH_WIN L"\\Themes\\Personalize"

#define REGPATH_WINNT              L"Software\\Microsoft\\Windows NT\\CurrentVersion"
#define REGPATH_WINNT_SERVERLEVELS REGPATH_WINNT L"\\Server\\ServerLevels"

#define REGPATH_WU                 L"Software\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate"
#define REGPATH_WU_REBOOTREQUIRED  REGPATH_WU L"\\Auto Update\\RebootRequired"

#define REGPATH_POLICIES_WINDOWS   L"Software\\Policies\\Microsoft\\Windows"
#define REGPATH_POLICIES_EXPLORER  REGPATH_POLICIES_WINDOWS L"\\Explorer"
#define REGPATH_POLICIES_WU        REGPATH_POLICIES_WINDOWS L"\\WindowsUpdate"
#define REGPATH_POLICIES_WU_AU     REGPATH_POLICIES_WU L"\\AU"

#define REGPATH_LEGACYUPDATE       L"Software\\Hashbang Productions\\Legacy Update"
#define REGPATH_LEGACYUPDATE_SETUP REGPATH_LEGACYUPDATE L"\\Setup"
#define REGPATH_LEGACYUPDATE_UNINSTALL REGPATH_WIN L"\\Uninstall\\Legacy Update"

// Filter out WOW64 keys that are not supported on Windows 2000
#ifdef _WIN64
	#define GetRegistryWow64Flag(options) (options)
#else
	#define GetRegistryWow64Flag(options) ((options) & ~(AtLeastWinXP2002() ? 0 : KEY_WOW64_64KEY | KEY_WOW64_32KEY))
#endif

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
