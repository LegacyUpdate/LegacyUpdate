#pragma once

// ElevationHelper.h : Declaration of the CElevationHelper class.

#include <atlctl.h>
#include "resource.h"
#include "LegacyUpdate_i.h"

BOOL ProgIDIsPermitted(PWSTR progID);
STDMETHODIMP CoCreateInstanceAsAdmin(HWND hwnd, __in REFCLSID rclsid, __in REFIID riid, __deref_out void **ppv);

// CElevationHelper
class ATL_NO_VTABLE CElevationHelper :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CElevationHelper, &CLSID_ElevationHelper>,
	public ISupportErrorInfo,
	public IDispatchImpl<IElevationHelper, &IID_IElevationHelper, &LIBID_LegacyUpdateLib, /*wMajor =*/ 1, /*wMinor =*/ 0> {

public:
	CElevationHelper();

	BEGIN_COM_MAP(CElevationHelper)
		COM_INTERFACE_ENTRY(IElevationHelper)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
	END_COM_MAP()

	// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid) {
		return IsEqualGUID(riid, IID_IElevationHelper) ? S_OK : S_FALSE;
	}

	// IElevationHelper
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct() { return S_OK; }
	void FinalRelease() {}

	static STDMETHODIMP UpdateRegistry(BOOL bRegister);

	STDMETHODIMP CreateObject(BSTR progID, IDispatch **retval);
	STDMETHODIMP Reboot(void);
};

OBJECT_ENTRY_AUTO(__uuidof(ElevationHelper), CElevationHelper)
