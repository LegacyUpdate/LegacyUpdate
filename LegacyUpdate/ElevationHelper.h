#pragma once

// ElevationHelper.h : Declaration of the CElevationHelper class.

#include "resource.h"
#include "com.h"
#include "LegacyUpdate_i.h"

STDMETHODIMP CreateElevationHelper(IUnknown *pUnkOuter, REFIID riid, void **ppv);

BOOL ProgIDIsPermitted(PWSTR progID);
STDMETHODIMP CoCreateInstanceAsAdmin(HWND hwnd, REFCLSID rclsid, REFIID riid, void **ppv);

class DECLSPEC_NOVTABLE CElevationHelper :
	public IDispatchImpl<IElevationHelper, &LIBID_LegacyUpdateLib> {
public:
	CElevationHelper() :
		m_refCount(1) {}

	virtual ~CElevationHelper();

private:
	LONG m_refCount;

public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IElevationHelper
	STDMETHODIMP CreateObject(BSTR progID, IDispatch **retval);
	STDMETHODIMP Reboot();
	STDMETHODIMP BeforeUpdate();
	STDMETHODIMP AfterUpdate();
};
