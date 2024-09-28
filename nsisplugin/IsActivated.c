#include <windows.h>
#include <nsis/pluginapi.h>
#include "main.h"
#include "WMI.h"
#include "VersionInfo.h"
#include "licdll.h"

#define WindowsLicensingAppId L"55c92734-d682-4d71-983e-d6ec3f16059f"

enum {
	LicenseStatusUnlicensed,
	LicenseStatusLicensed,
	LicenseStatusOOBGrace,
	LicenseStatusOOTGrace,
	LicenseStatusNonGenuineGrace,
	LicenseStatusNotification,
	LicenseStatusExtendedGrace
};

PLUGIN_METHOD(IsActivated) {
	PLUGIN_INIT();

	// Activation is irrelevant prior to XP
	if (!IsOSVersionOrLater(5, 1)) {
		pushint(1);
		return;
	}

	if (IsOSVersionOrLater(6, 0)) {
		// Vista+: Ask the Software Licensing Service
		VARIANT value;
		HRESULT hr = QueryWMIProperty(L"SELECT LicenseStatus FROM SoftwareLicensingProduct WHERE ApplicationID = '" WindowsLicensingAppId L"'", L"LicenseStatus", &value);
		if (!SUCCEEDED(hr)) {
			// Treat as success because we can't determine the status
			pushint(1);
			return;
		}

		pushint(value.intVal == LicenseStatusLicensed);
		VariantClear(&value);
	} else {
		// XP: Use private API
		ICOMLicenseAgent *agent;
		HRESULT hr = CoCreateInstance(&CLSID_COMLicenseAgent, NULL, CLSCTX_INPROC_SERVER, &IID_ICOMLicenseAgent, (void **)&agent);
		if (!SUCCEEDED(hr)) {
			goto end;
		}

		ULONG result;
		hr = ICOMLicenseAgent_Initialize(agent, 0xC475, 3, NULL, &result);
		if (!SUCCEEDED(hr) || result != 0) {
			pushint(1);
			goto end;
		}

		ULONG wpaLeft, evalLeft;
		hr = ICOMLicenseAgent_GetExpirationInfo(agent, &wpaLeft, &evalLeft);
		if (!SUCCEEDED(hr)) {
			pushint(1);
			goto end;
		}

		pushint(wpaLeft == MAXLONG);

end:
		if (agent) {
			ICOMLicenseAgent_Release(agent);
		}
	}
}
