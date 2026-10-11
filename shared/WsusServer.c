#include "WsusServer.h"
#include <windows.h>
#include "Registry.h"
#include "VersionInfo.h"

static const LPCWSTR ProxyUpdateServerURLHttp   = L"http://legacyupdate.net/v6";
static const LPCWSTR ProxyUpdateServerURLHttps  = L"https://legacyupdate.net/v6";

static const LPCWSTR CustomUpdateServerURLHttp  = L"http://wu.legacyupdate.net/v6";
static const LPCWSTR CustomUpdateServerURLHttps = L"https://wu.legacyupdate.net/v6";

static const LPCWSTR UpdateSiteURLHttp  = L"http://legacyupdate.net/windowsupdate/v6/";
static const LPCWSTR UpdateSiteURLHttps = L"https://legacyupdate.net/windowsupdate/v6/";

// Matches with or without a trailing slash
static BOOL IsServerUrl(LPCWSTR url, LPCWSTR serverUrl) {
	int length = lstrlen(serverUrl);
	if (wcsncmp(url, serverUrl, length) != 0) {
		return FALSE;
	}

	return url[length] == L'\0' || (url[length] == L'/' && url[length + 1] == L'\0');
}

HRESULT IsUsingWsusServer(BOOL *retval) {
	if (retval == NULL) {
		return E_POINTER;
	}

	DWORD value = 0;
	HRESULT hr = GetRegistryDword(HKEY_LOCAL_MACHINE, REGPATH_POLICIES_WU_AU, L"UseWUServer", KEY_WOW64_64KEY, &value);
	*retval = SUCCEEDED(hr) && value == 1;
	return S_OK;
}

HRESULT GetWsusServerUrl(BSTR *retval) {
	if (retval == NULL) {
		return E_POINTER;
	}

	LPWSTR data = NULL;
	DWORD size = 0;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, REGPATH_POLICIES_WU, L"WUServer", KEY_WOW64_64KEY, &data, &size);
	*retval = SUCCEEDED(hr) ? SysAllocStringLen(data, size - 1) : NULL;

	if (data) {
		LocalFree(data);
	}

	return S_OK;
}

HRESULT GetWsusStatusServerUrl(BSTR *retval) {
	if (retval == NULL) {
		return E_POINTER;
	}

	LPWSTR data = NULL;
	DWORD size = 0;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, REGPATH_POLICIES_WU, L"WUStatusServer", KEY_WOW64_64KEY, &data, &size);
	*retval = SUCCEEDED(hr) ? SysAllocStringLen(data, size - 1) : NULL;

	if (data) {
		LocalFree(data);
	}

	return S_OK;
}

HRESULT GetWsusServer(WsusServer *retval) {
	if (retval == NULL) {
		return E_POINTER;
	}

	BSTR serverUrl = NULL;
	HRESULT hr = GetWsusServerUrl(&serverUrl);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	if (serverUrl == NULL) {
		*retval = e_none;
		return S_OK;
	}

	if (IsServerUrl(serverUrl, ProxyUpdateServerURLHttp) || IsServerUrl(serverUrl, ProxyUpdateServerURLHttps)) {
		*retval = e_legacyUpdateProxy;
	} else if (IsServerUrl(serverUrl, CustomUpdateServerURLHttp) || IsServerUrl(serverUrl, CustomUpdateServerURLHttps)) {
		*retval = e_legacyUpdateCustom;
	} else {
		*retval = e_other;
	}

	SysFreeString(serverUrl);
	return hr;
}

BOOL IsUpdateSiteHttps(void) {
	// Fallback: Use SSL only on Vista and up
	BOOL useHTTPS = AtLeastWinVista();

	// Get the Windows Update website URL set by Legacy Update setup
	LPWSTR data = NULL;
	DWORD size = 0;
	HRESULT hr = GetRegistryString(HKEY_LOCAL_MACHINE, REGPATH_WU, L"URL", KEY_WOW64_64KEY, &data, &size);
	if (SUCCEEDED(hr)) {
		// Return based on the URL value
		if (wcscmp(data, UpdateSiteURLHttps) == 0) {
			useHTTPS = TRUE;
		} else if (wcscmp(data, UpdateSiteURLHttp) == 0) {
			useHTTPS = FALSE;
		}

		LocalFree(data);
	}

	return useHTTPS;
}

static HRESULT StopWuService(void) {
	HRESULT hr = E_FAIL;
	SC_HANDLE manager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (manager) {
		SC_HANDLE service = OpenService(manager, L"wuauserv", SERVICE_STOP | SERVICE_START | SERVICE_QUERY_STATUS);
		if (service) {
			SERVICE_STATUS status;
			if (ControlService(service, SERVICE_CONTROL_STOP, &status)) {
				while (QueryServiceStatus(service, &status) && status.dwCurrentState != SERVICE_STOPPED) {
					Sleep(100);
				}
			}

			if (StartService(service, 0, NULL)) {
				hr = S_OK;
			}

			CloseServiceHandle(service);
		}

		CloseServiceHandle(manager);
	}

	CHECK_HR_OR_RETURN(L"StopWuService");
	return hr;
}

HRESULT SetWsusServer(WsusServer server) {
	LPCWSTR serverUrl = NULL;

	switch (server) {
	case e_none:
		break;

	case e_legacyUpdateProxy:
		serverUrl = IsUpdateSiteHttps() ? ProxyUpdateServerURLHttps : ProxyUpdateServerURLHttp;
		break;

	case e_legacyUpdateCustom:
		serverUrl = IsUpdateSiteHttps() ? CustomUpdateServerURLHttps : CustomUpdateServerURLHttp;
		break;

	default:
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;
	if (serverUrl == NULL) {
		RegistryEntry entries[] = {
			{HKEY_LOCAL_MACHINE, REGPATH_POLICIES_WU, L"WUServer", 0, {DELETE_VALUE}, 0, KEY_WOW64_64KEY},
			{HKEY_LOCAL_MACHINE, REGPATH_POLICIES_WU, L"WUStatusServer", 0, {DELETE_VALUE}, 0, KEY_WOW64_64KEY},
			{HKEY_LOCAL_MACHINE, REGPATH_POLICIES_WU_AU, L"UseWUServer", 0, {DELETE_VALUE}, 0, KEY_WOW64_64KEY},
			{NULL}
		};
		hr = SetRegistryEntries(entries);
	} else {
		RegistryEntry entries[] = {
			{HKEY_LOCAL_MACHINE, REGPATH_POLICIES_WU, L"WUServer", REG_SZ, {(LPVOID)serverUrl}, 0, KEY_WOW64_64KEY},
			{HKEY_LOCAL_MACHINE, REGPATH_POLICIES_WU, L"WUStatusServer", REG_SZ, {(LPVOID)serverUrl}, 0, KEY_WOW64_64KEY},
			{HKEY_LOCAL_MACHINE, REGPATH_POLICIES_WU_AU, L"UseWUServer", REG_DWORD, {(LPVOID)1}, 0, KEY_WOW64_64KEY},
			{NULL}
		};
		hr = SetRegistryEntries(entries);
	}

	CHECK_HR_OR_RETURN(L"SetRegistryEntries");
	return StopWuService();
}
