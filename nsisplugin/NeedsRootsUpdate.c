#include <windows.h>
#include <nsis/pluginapi.h>
#include "Registry.h"
#include "VersionInfo.h"

#define ROOTS_UPDATE_THRESHOLD (30LL * 24LL * 60LL * 60LL * 10000000LL)

// 2026-10-20T18:02:59Z, when disallowedcert.sst was updated to revoke leaked eMudhra certs
#define CERT_REVOKE_EMUDHRA (134369929790000000ULL)

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

	ULONGLONG currentTime64 = 0, lastUpdateTime64 = 0;
	memcpy(&currentTime64, &currentTime, sizeof(currentTime64));
	memcpy(&lastUpdateTime64, &lastUpdateTime, sizeof(lastUpdateTime64));

	if (lastUpdateTime64 < CERT_REVOKE_EMUDHRA) {
		pushint(1);
		return;
	}

	ULONGLONG difference = currentTime64 - lastUpdateTime64;
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
