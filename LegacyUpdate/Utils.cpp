#include "stdafx.h"
#include <comdef.h>
#include <atlstr.h>
#include <Wbemidl.h>

#pragma comment(lib, "version.lib")
#pragma comment(lib, "wbemuuid.lib")

extern "C" IMAGE_DOS_HEADER __ImageBase;

#define OwnModule ((HMODULE)&__ImageBase)

static BOOL _loadedVersionInfo = FALSE;
static OSVERSIONINFOEX _versionInfo;

static BOOL _loadedProductName = FALSE;
static VARIANT _productName;

static BOOL _loadedOwnVersion = FALSE;
static LPWSTR _version;
static UINT _versionSize;

OSVERSIONINFOEX *GetVersionInfo() {
	if (!_loadedVersionInfo) {
		_loadedVersionInfo = true;
		ZeroMemory(&_versionInfo, sizeof(OSVERSIONINFOEX));
		_versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((LPOSVERSIONINFO)&_versionInfo);
	}
	return &_versionInfo;
}

void GetOwnFileName(LPWSTR *filename, LPDWORD size) {
	*filename = (LPWSTR)malloc(MAX_PATH);
	*size = GetModuleFileName(OwnModule, *filename, MAX_PATH);
}

HRESULT GetOwnVersion(LPWSTR *version, LPDWORD size) {
	if (!_loadedOwnVersion) {
		LPWSTR filename;
		DWORD filenameSize;
		GetOwnFileName(&filename, &filenameSize);

		DWORD verHandle;
		DWORD verInfoSize = GetFileVersionInfoSize(filename, &verHandle);
		if (verInfoSize == 0) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		LPVOID verInfo = new BYTE[verInfoSize];
		if (!GetFileVersionInfo(filename, verHandle, verInfoSize, verInfo)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (!VerQueryValue(verInfo, L"\\StringFileInfo\\040904B0\\ProductVersion", (LPVOID *)&_version, &_versionSize)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}
	}

	*version = _version;
	*size = _versionSize;
	return _version == NULL ? E_FAIL : NOERROR;
}

HRESULT GetRegistryString(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, LPDWORD type, LPWSTR *data, LPDWORD size) {
	HKEY subkey;
	HRESULT result = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, KEY_READ, &subkey));
	if (!SUCCEEDED(result)) {
		goto end;
	}

	if (data != NULL && size != NULL) {
		DWORD length = 512 * sizeof(WCHAR);
		LPWSTR buffer = (LPWSTR)malloc(length);
		LSTATUS status;
		do {
			status = RegQueryValueEx(subkey, valueName, NULL, type, (BYTE *)buffer, &length);
			if (status == ERROR_MORE_DATA) {
				length += 256 * sizeof(WCHAR);
				buffer = (LPWSTR)realloc(buffer, length);
			} else if (status != ERROR_SUCCESS) {
				result = HRESULT_FROM_WIN32(status);
				goto end;
			}
		} while (status == ERROR_MORE_DATA);

		*data = buffer;
		*size = length / sizeof(WCHAR);
	}

end:
	if (subkey != NULL) {
		RegCloseKey(subkey);
	}
	if (!SUCCEEDED(result)) {
		if (data != NULL) {
			*data = NULL;
		}
		if (size != NULL) {
			*size = 0;
		}
	}
	return result;
}

HRESULT GetRegistryDword(HKEY key, LPCWSTR subkeyPath, LPCWSTR valueName, LPDWORD type, LPDWORD data) {
	HKEY subkey;
	HRESULT result = HRESULT_FROM_WIN32(RegOpenKeyEx(key, subkeyPath, 0, KEY_READ, &subkey));
	if (!SUCCEEDED(result)) {
		goto end;
	}

	if (data != NULL) {
		DWORD length = sizeof(DWORD);
		result = HRESULT_FROM_WIN32(RegQueryValueEx(subkey, valueName, NULL, type, (LPBYTE)data, &length));
		if (!SUCCEEDED(result)) {
			goto end;
		}
	}

end:
	if (subkey != NULL) {
		RegCloseKey(subkey);
	}
	return result;
}

LPWSTR GetMessageForHresult(HRESULT result) {
	_com_error *error = new _com_error(result);
	CString message = error->ErrorMessage();
	BSTR outMessage = message.AllocSysString();
	return outMessage;
}

HRESULT GetOSProductName(VARIANT *pProductName) {
	if (_loadedProductName) {
		VariantCopy(pProductName, &_productName);
		return S_OK;
	}

	VariantInit(&_productName);
	_loadedProductName = true;

	IWbemLocator *locator;
	IWbemServices *services;
	IEnumWbemClassObject *enumerator;
	IWbemClassObject *object;

	HRESULT hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&locator);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	hr = locator->ConnectServer(L"ROOT\\CIMV2", NULL, NULL, 0, NULL, 0, 0, &services);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	hr = CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	hr = services->ExecQuery(L"WQL", L"SELECT Caption FROM Win32_OperatingSystem", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &enumerator);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	while (enumerator) {
		ULONG uReturn = 0;
		hr = enumerator->Next(WBEM_INFINITE, 1, &object, &uReturn);
		if (!SUCCEEDED(hr)) {
			goto end;
		}
		if (uReturn == 0) {
			break;
		}

		hr = object->Get(L"Caption", 0, &_productName, NULL, NULL);
		object->Release();
		if (!SUCCEEDED(hr)) {
			goto end;
		}

		VariantCopy(pProductName, &_productName);
	}

end:
	if (locator != NULL) {
		locator->Release();
	}
	if (services != NULL) {
		services->Release();
	}
	if (enumerator != NULL) {
		enumerator->Release();
	}
	return hr;
}
