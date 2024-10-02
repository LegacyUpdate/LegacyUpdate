#include <windows.h>
#include <nsis/pluginapi.h>
#include "main.h"
#include "WMI.h"
#include "VersionInfo.h"
#include "licdll.h"
#include <slpublic.h>

typedef HRESULT (WINAPI *_SLOpen)(HSLC *);
typedef HRESULT (WINAPI *_SLGetLicensingStatusInformation)(HSLC, const SLID *, DWORD, DWORD, UINT *, SL_LICENSING_STATUS **);
typedef HRESULT (WINAPI *_SLClose)(HSLC);

static _SLOpen $SLOpen;
static _SLClose $SLClose;
static _SLGetLicensingStatusInformation $SLGetLicensingStatusInformation;

static BOOL g_loadedLicenseStatus = FALSE;
static BOOL g_isActivated = TRUE;

PLUGIN_METHOD(IsActivated) {
	PLUGIN_INIT();

	// Activation is irrelevant prior to XP
	if (g_loadedLicenseStatus || !AtLeastWinXP2002()) {
		pushint(g_isActivated);
		return;
	}

	g_loadedLicenseStatus = TRUE;

	if (AtLeastWinVista()) {
		// Vista+: Ask the Software Licensing Service
		if (!$SLOpen) {
			HMODULE slc = LoadLibrary(L"slc.dll");
			$SLOpen = (_SLOpen)GetProcAddress(slc, "SLOpen");
			$SLClose = (_SLClose)GetProcAddress(slc, "SLClose");
			$SLGetLicensingStatusInformation = (_SLGetLicensingStatusInformation)GetProcAddress(slc, "SLGetLicensingStatusInformation");
		}

		if (!$SLOpen || !$SLClose || !$SLGetLicensingStatusInformation) {
			TRACE(L"Failed to load slc.dll");
			pushint(1);
			return;
		}

		HSLC slc;
		SL_LICENSING_STATUS *status;
		UINT count;
		HRESULT hr = $SLOpen(&slc);
		if (!SUCCEEDED(hr)) {
			goto end_slc;
		}

		hr = $SLGetLicensingStatusInformation(slc, &WINDOWS_SLID, 0, 0, &count, &status);
		if (!SUCCEEDED(hr) || count == 0) {
			goto end_slc;
		}

		g_isActivated = status->eStatus == SL_LICENSING_STATUS_LICENSED;

end_slc:
		if (status) {
			LocalFree(status);
		}
		if (slc) {
			$SLClose(slc);
		}
	} else {
		// XP: Use private API
		ICOMLicenseAgent *agent;
		HRESULT hr = CoCreateInstance(&CLSID_COMLicenseAgent, NULL, CLSCTX_INPROC_SERVER, &IID_ICOMLicenseAgent, (void **)&agent);
		if (!SUCCEEDED(hr)) {
			TRACE(L"COMLicenseAgent load failed: %x", hr);
			goto end_xp;
		}

		ULONG result;
		hr = ICOMLicenseAgent_Initialize(agent, 0xC475, 3, NULL, &result);
		if (!SUCCEEDED(hr) || result != 0) {
			TRACE(L"COMLicenseAgent init failed: %x", hr);
			goto end_xp;
		}

		ULONG wpaLeft, evalLeft;
		hr = ICOMLicenseAgent_GetExpirationInfo(agent, &wpaLeft, &evalLeft);
		if (!SUCCEEDED(hr)) {
			TRACE(L"COMLicenseAgent GetExpirationInfo failed: %x", hr);
			goto end_xp;
		}

		g_isActivated = wpaLeft == MAXLONG;

end_xp:
		if (agent) {
			ICOMLicenseAgent_Release(agent);
		}
	}

	pushint(g_isActivated);
}
