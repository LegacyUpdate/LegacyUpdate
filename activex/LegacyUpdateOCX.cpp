// LegacyUpdateOCX.cpp : Implementation of CLegacyUpdateOCXApp and DLL registration.

#include "stdafx.h"
#include "LegacyUpdateOCX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLegacyUpdateOCXApp theApp;

const GUID CDECL _tlid = { 0x5D22F33, 0xC7C3, 0x4C90, { 0xBD, 0xD9, 0xCE, 0xDC, 0x86, 0xEA, 0x8F, 0xBE } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

BOOL CLegacyUpdateOCXApp::InitInstance() {
	BOOL bInit = COleControlModule::InitInstance();
	if (bInit) {
		// TODO: Add your own module initialization code here.
	}
	return bInit;
}

int CLegacyUpdateOCXApp::ExitInstance() {
	// TODO: Add your own module termination code here.
	return COleControlModule::ExitInstance();
}

// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void) {
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid)) {
		return ResultFromScode(SELFREG_E_TYPELIB);
	}

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE)) {
		return ResultFromScode(SELFREG_E_CLASS);
	}

	// Mark safe for web
	// https://docs.microsoft.com/en-us/cpp/mfc/upgrading-an-existing-activex-control?view=msvc-170#_core_marking_a_control_safe_for_scripting_and_initializing
	wchar_t *keys[] = {
		L"CLSID\\{AD28E0DF-5F5A-40B5-9432-85EFD97D1F9F}\\Implemented Categories",
		L"CLSID\\{AD28E0DF-5F5A-40B5-9432-85EFD97D1F9F}\\Implemented Categories\\{7DD95801-9882-11CF-9FA9-00AA006C42C4}",
		L"CLSID\\{AD28E0DF-5F5A-40B5-9432-85EFD97D1F9F}\\Implemented Categories\\{7DD95802-9882-11CF-9FA9-00AA006C42C4}"
	};
	for (int i = 0; i < 3; i++) {
		HKEY key;
		HRESULT result = RegCreateKeyEx(HKEY_CLASSES_ROOT, keys[i], 0, NULL, 0, 0, 0, &key, NULL);
		if (!SUCCEEDED(result)) {
			return result;
		}
		RegCloseKey(key);
	}

	return NOERROR;
}

// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void) {
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor)) {
		return ResultFromScode(SELFREG_E_TYPELIB);
	}

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE)) {
		return ResultFromScode(SELFREG_E_CLASS);
	}

	return NOERROR;
}
