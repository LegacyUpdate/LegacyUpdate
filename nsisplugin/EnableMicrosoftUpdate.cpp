#include <windows.h>
#include <nsis/pluginapi.h>
#include <atlcomcli.h>
#include <wuapi.h>
#include "main.h"
#include <strsafe.h>

static const GUID our_CLSID_UpdateServiceManager = { 0xf8d253d9, 0x89a4, 0x4daa, { 0x87, 0xb6, 0x11, 0x68, 0x36, 0x9f, 0x0b, 0x21 } };
static const GUID our_IID_IUpdateServiceManager2 = { 0x0bb8531d, 0x7e8d, 0x424f, { 0x98, 0x6c, 0xa0, 0xb8, 0xf6, 0x0a, 0x3e, 0x7b } };

static const LPWSTR MicrosoftUpdateServiceID = L"7971f918-a847-4430-9279-4a52d1efe18d";

EXTERN_C __declspec(dllexport)
void __cdecl EnableMicrosoftUpdate(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra) {
	EXDLL_INIT();
	g_hwndParent = hwndParent;

	CComPtr<IUpdateServiceManager2> serviceManager;
	CComPtr<IUpdateServiceRegistration> registration;

	HRESULT hr = CoCreateInstance(our_CLSID_UpdateServiceManager, NULL, CLSCTX_INPROC_SERVER, our_IID_IUpdateServiceManager2, (void **)&serviceManager);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	hr = serviceManager->AddService2(SysAllocString(MicrosoftUpdateServiceID), 0, SysAllocString(L""), &registration);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

end:
	pushint(hr);
}
