#pragma once

// ElevationHelper.h : Declaration of the CElevationHelper class.

#include "resource.h"
#include "LegacyUpdate_i.h"

extern "C" {

STDMETHODIMP CreateElevationHelper(IUnknown *pUnkOuter, REFIID riid, void **ppv);

BOOL ProgIDIsPermitted(PWSTR progID);
STDMETHODIMP CoCreateInstanceAsAdmin(HWND hwnd, REFCLSID rclsid, REFIID riid, void **ppv);

typedef struct CElevationHelper CElevationHelper;

typedef struct CElevationHelperVtbl {
	// IUnknown
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(CElevationHelper *This, REFIID riid, void **ppvObject);
	ULONG   (STDMETHODCALLTYPE *AddRef)(CElevationHelper *This);
	ULONG   (STDMETHODCALLTYPE *Release)(CElevationHelper *This);

	// IDispatch
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(CElevationHelper *This, UINT *pctinfo);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(CElevationHelper *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(CElevationHelper *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	HRESULT (STDMETHODCALLTYPE *Invoke)(CElevationHelper *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

	// IElevationHelper
	HRESULT (STDMETHODCALLTYPE *CreateObject)(CElevationHelper *This, BSTR progID, IDispatch **retval);
	HRESULT (STDMETHODCALLTYPE *Reboot)(CElevationHelper *This);
	HRESULT (STDMETHODCALLTYPE *BeforeUpdate)(CElevationHelper *This);
	HRESULT (STDMETHODCALLTYPE *AfterUpdate)(CElevationHelper *This);
} CElevationHelperVtbl;

struct CElevationHelper {
	CElevationHelperVtbl *lpVtbl;
	LONG refCount;
};

// IUnknown
STDMETHODIMP ElevationHelper_QueryInterface(CElevationHelper *This, REFIID riid, void **ppvObject);
ULONG STDMETHODCALLTYPE ElevationHelper_AddRef(CElevationHelper *This);
ULONG STDMETHODCALLTYPE ElevationHelper_Release(CElevationHelper *This);

// IDispatch
STDMETHODIMP ElevationHelper_GetTypeInfoCount(CElevationHelper *This, UINT *pctinfo);
STDMETHODIMP ElevationHelper_GetTypeInfo(CElevationHelper *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
STDMETHODIMP ElevationHelper_GetIDsOfNames(CElevationHelper *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
STDMETHODIMP ElevationHelper_Invoke(CElevationHelper *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

// IElevationHelper
STDMETHODIMP ElevationHelper_CreateObject(CElevationHelper *This, BSTR progID, IDispatch **retval);
STDMETHODIMP ElevationHelper_Reboot(CElevationHelper *This);
STDMETHODIMP ElevationHelper_BeforeUpdate(CElevationHelper *This);
STDMETHODIMP ElevationHelper_AfterUpdate(CElevationHelper *This);

}
