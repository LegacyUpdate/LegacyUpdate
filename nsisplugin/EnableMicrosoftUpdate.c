#include <windows.h>
#include <nsis/pluginapi.h>
#include <wuapi.h>
#include "main.h"

static const LPWSTR MicrosoftUpdateServiceID = L"7971f918-a847-4430-9279-4a52d1efe18d";

EXTERN_C __declspec(dllexport)
void __cdecl EnableMicrosoftUpdate(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra) {
	EXDLL_INIT();
	g_hwndParent = hwndParent;

	IUpdateServiceManager2 *serviceManager;
	IUpdateServiceRegistration *registration;

	HRESULT hr = CoCreateInstance(&CLSID_UpdateServiceManager, NULL, CLSCTX_INPROC_SERVER, &IID_IUpdateServiceManager2, (void **)&serviceManager);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

	hr = IUpdateServiceManager2_AddService2(serviceManager, SysAllocString(MicrosoftUpdateServiceID), asfAllowPendingRegistration | asfAllowOnlineRegistration | asfRegisterServiceWithAU, SysAllocString(L""), &registration);
	if (!SUCCEEDED(hr)) {
		goto end;
	}

end:
	if (registration != NULL) {
		IUpdateServiceManager2_Release(registration);
	}

	if (serviceManager != NULL) {
		IUpdateServiceManager2_Release(serviceManager);
	}

	pushint(hr);
}
