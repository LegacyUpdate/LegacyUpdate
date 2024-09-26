#include "stdafx.h"
#include "WMI.h"
#include <wbemcli.h>

#pragma comment(lib, "wbemuuid.lib")

#ifdef __cplusplus
	#define our_CLSID_WbemLocator CLSID_WbemLocator
	#define our_IID_IWbemLocator  IID_IWbemLocator
	#define IWbemLocator_ConnectServer(obj, ...) obj->ConnectServer(__VA_ARGS__)
	#define IWbemServices_ExecQuery(obj, ...)    obj->ExecQuery(__VA_ARGS__)
	#define IEnumWbemClassObject_Next(obj, ...)  obj->Next(__VA_ARGS__)
	#define IWbemClassObject_Get(obj, ...)       obj->Get(__VA_ARGS__)
	#define IWbemClassObject_Release(obj)        obj->Release()
	#define IEnumWbemClassObject_Release(obj)    obj->Release()
	#define IWbemServices_Release(obj)           obj->Release()
	#define IWbemLocator_Release(obj)            obj->Release()
#else
	#define our_CLSID_WbemLocator &CLSID_WbemLocator
	#define our_IID_IWbemLocator  &IID_IWbemLocator
#endif

HRESULT QueryWMIProperty(LPWSTR query, LPWSTR property, LPVARIANT value) {
	IWbemLocator *locator = NULL;
	IWbemServices *services = NULL;
	IEnumWbemClassObject *enumerator = NULL;
	IWbemClassObject *object = NULL;
	HRESULT hr = CoCreateInstance(our_CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, our_IID_IWbemLocator, (void **)&locator);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	BSTR server = SysAllocString(L"ROOT\\CIMV2");
	hr = IWbemLocator_ConnectServer(locator, server, NULL, NULL, NULL, 0, NULL, NULL, &services);
	SysFreeString(server);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	hr = CoSetProxyBlanket((IUnknown *)services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	BSTR wql = SysAllocString(L"WQL");
	BSTR queryBstr = SysAllocString(query);
	hr = IWbemServices_ExecQuery(services, wql, queryBstr, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &enumerator);
	SysFreeString(wql);
	SysFreeString(queryBstr);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	ULONG uReturn = 0;
	hr = IEnumWbemClassObject_Next(enumerator, WBEM_INFINITE, 1, &object, &uReturn);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	BSTR propBstr = SysAllocString(property);
	hr = IWbemClassObject_Get(object, propBstr, 0, value, NULL, NULL);
	SysFreeString(propBstr);

end:
	if (object) {
		IWbemClassObject_Release(object);
	}
	if (enumerator) {
		IEnumWbemClassObject_Release(enumerator);
	}
	if (services) {
		IWbemServices_Release(services);
	}
	if (locator) {
		IWbemLocator_Release(locator);
	}

	return hr;
}
