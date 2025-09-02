#include <windows.h>
#include <nsis/pluginapi.h>
#include <wuapi.h>
#include "main.h"

static const LPWSTR MicrosoftUpdateServiceID = L"7971f918-a847-4430-9279-4a52d1efe18d";

PLUGIN_METHOD(EnableMicrosoftUpdate) {
	PLUGIN_INIT();

	IUpdateServiceManager2 *serviceManager;
	IUpdateServiceRegistration *registration;

	HRESULT hr = CoCreateInstance(&CLSID_UpdateServiceManager, NULL, CLSCTX_INPROC_SERVER, &IID_IUpdateServiceManager2, (void **)&serviceManager);
	CHECK_HR_OR_GOTO_END(L"CoCreateInstance");

	BSTR clientID = SysAllocString(L"Legacy Update");
	hr = IUpdateServiceManager2_put_ClientApplicationID(serviceManager, clientID);
	SysFreeString(clientID);
	CHECK_HR_OR_GOTO_END(L"put_ClientApplicationID");

	BSTR serviceID = SysAllocString(MicrosoftUpdateServiceID);
	BSTR serviceCab = SysAllocString(L"");
	hr = IUpdateServiceManager2_AddService2(serviceManager, serviceID, asfAllowPendingRegistration | asfAllowOnlineRegistration | asfRegisterServiceWithAU, serviceCab, &registration);
	SysFreeString(serviceID);
	SysFreeString(serviceCab);
	CHECK_HR_OR_GOTO_END(L"AddService2");

end:
	if (registration) {
		IUpdateServiceManager2_Release(registration);
	}

	if (serviceManager) {
		IUpdateServiceManager2_Release(serviceManager);
	}

	pushint(hr);
}
