#include "stdafx.h"
#include "WMI.h"
#include <atlcomcli.h>
#include <wbemcli.h>

#pragma comment(lib, "wbemuuid.lib")

HRESULT QueryWMIProperty(BSTR query, BSTR property, VARIANT *value) {
	CComPtr<IWbemLocator> locator;
	HRESULT hr = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (void **)&locator);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	CComPtr<IWbemServices> services;
	hr = locator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, NULL, 0, NULL, NULL, &services);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	CComPtr<IEnumWbemClassObject> enumerator;
	hr = services->ExecQuery(L"WQL", query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &enumerator);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	CComPtr<IWbemClassObject> object;
	ULONG uReturn = 0;
	hr = enumerator->Next(WBEM_INFINITE, 1, &object, &uReturn);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = object->Get(property, 0, value, NULL, NULL);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	return S_OK;
}
