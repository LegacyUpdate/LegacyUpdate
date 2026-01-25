#include <windows.h>
#include <nsis/pluginapi.h>
#include "Registry.h"
#include "VersionInfo.h"

#define ROOTS_UPDATE_THRESHOLD (30LL * 24LL * 60LL * 60LL * 10000000LL)

PLUGIN_METHOD(NeedsRootsUpdate) {
	PLUGIN_INIT();

	FILETIME currentTime = {0};
	GetSystemTimeAsFileTime(&currentTime);

	HKEY key = NULL;
	HRESULT hr = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGPATH_LEGACYUPDATE_SETUP, 0, GetRegistryWow64Flag(KEY_READ | KEY_WOW64_64KEY), &key));
	if (!SUCCEEDED(hr)) {
		pushint(1);
		return;
	}

	FILETIME lastUpdateTime = {0};
	DWORD type = REG_QWORD;
	DWORD size = sizeof(FILETIME);
	hr = HRESULT_FROM_WIN32(RegQueryValueEx(key, L"LastRootsUpdateTime", NULL, &type, (LPBYTE)&lastUpdateTime, &size));
	RegCloseKey(key);

	if (!SUCCEEDED(hr) || type != REG_QWORD) {
		pushint(1);
		return;
	}

	ULONGLONG difference = *(ULONGLONG *)&currentTime - *(ULONGLONG *)&lastUpdateTime;
	pushint(difference > ROOTS_UPDATE_THRESHOLD ? 1 : 0);
}

PLUGIN_METHOD(SetRootsUpdateTime) {
	PLUGIN_INIT();

	FILETIME currentTime = {0};
	GetSystemTimeAsFileTime(&currentTime);

	HKEY key = NULL;
	HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGPATH_LEGACYUPDATE_SETUP, 0, NULL, 0, GetRegistryWow64Flag(KEY_WRITE | KEY_WOW64_64KEY), NULL, &key, NULL));
	if (SUCCEEDED(hr)) {
		RegSetValueEx(key, L"LastRootsUpdateTime", 0, REG_QWORD, (LPBYTE)&currentTime, sizeof(FILETIME));
		RegCloseKey(key);
	}
}
