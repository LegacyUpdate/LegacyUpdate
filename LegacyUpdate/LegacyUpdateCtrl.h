#pragma once

// LegacyUpdateCtrl.h : Declaration of the CLegacyUpdateCtrl ActiveX Control class.

// CLegacyUpdateCtrl : See LegacyUpdateCtrl.cpp for implementation.

#include <mshtml.h>
#include <wuapi.h>
#include "resource.h"
#include "LegacyUpdate_i.h"
#include "ccomptr.h"

extern "C" {

STDMETHODIMP CreateLegacyUpdateCtrl(IUnknown *pUnkOuter, REFIID riid, void **ppv);

typedef struct CLegacyUpdateCtrl CLegacyUpdateCtrl;

typedef struct CLegacyUpdateCtrlVtbl {
	// IUnknown
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(CLegacyUpdateCtrl *This, REFIID riid, void **ppvObject);
	ULONG   (STDMETHODCALLTYPE *AddRef)(CLegacyUpdateCtrl *This);
	ULONG   (STDMETHODCALLTYPE *Release)(CLegacyUpdateCtrl *This);

	// IDispatch
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(CLegacyUpdateCtrl *This, UINT *pctinfo);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(CLegacyUpdateCtrl *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(CLegacyUpdateCtrl *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	HRESULT (STDMETHODCALLTYPE *Invoke)(CLegacyUpdateCtrl *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

	// ILegacyUpdateCtrl
	HRESULT (STDMETHODCALLTYPE *CheckControl)(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval);
	HRESULT (STDMETHODCALLTYPE *MessageForHresult)(CLegacyUpdateCtrl *This, LONG inHresult, BSTR *retval);
	HRESULT (STDMETHODCALLTYPE *GetOSVersionInfo)(CLegacyUpdateCtrl *This, OSVersionField osField, LONG systemMetric, VARIANT *retval);
	HRESULT (STDMETHODCALLTYPE *RequestElevation)(CLegacyUpdateCtrl *This);
	HRESULT (STDMETHODCALLTYPE *CreateObject)(CLegacyUpdateCtrl *This, BSTR progID, IDispatch **retval);
	HRESULT (STDMETHODCALLTYPE *SetBrowserHwnd)(CLegacyUpdateCtrl *This, IUpdateInstaller *installer);
	HRESULT (STDMETHODCALLTYPE *GetUserType)(CLegacyUpdateCtrl *This, UserType *retval);
	HRESULT (STDMETHODCALLTYPE *get_IsRebootRequired)(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval);
	HRESULT (STDMETHODCALLTYPE *get_IsWindowsUpdateDisabled)(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval);
	HRESULT (STDMETHODCALLTYPE *RebootIfRequired)(CLegacyUpdateCtrl *This);
	HRESULT (STDMETHODCALLTYPE *ViewWindowsUpdateLog)(CLegacyUpdateCtrl *This);
	HRESULT (STDMETHODCALLTYPE *OpenWindowsUpdateSettings)(CLegacyUpdateCtrl *This);
	HRESULT (STDMETHODCALLTYPE *get_IsUsingWsusServer)(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval);
	HRESULT (STDMETHODCALLTYPE *get_WsusServerUrl)(CLegacyUpdateCtrl *This, BSTR *retval);
	HRESULT (STDMETHODCALLTYPE *get_WsusStatusServerUrl)(CLegacyUpdateCtrl *This, BSTR *retval);
	HRESULT (STDMETHODCALLTYPE *BeforeUpdate)(CLegacyUpdateCtrl *This);
	HRESULT (STDMETHODCALLTYPE *AfterUpdate)(CLegacyUpdateCtrl *This);
} CLegacyUpdateCtrlVtbl;

struct CLegacyUpdateCtrl {
	CLegacyUpdateCtrlVtbl *lpVtbl;
	LONG refCount;

	IOleClientSite *clientSite;
	IOleInPlaceSite *inPlaceSite;
	IOleContainer *container;

	CComPtr<IElevationHelper> elevatedHelper;
	CComPtr<IElevationHelper> nonElevatedHelper;

	HWND hwnd;
	BOOL windowOnly;
};

// IUnknown
STDMETHODIMP LegacyUpdateCtrl_QueryInterface(CLegacyUpdateCtrl *This, REFIID riid, void **ppvObject);
ULONG STDMETHODCALLTYPE LegacyUpdateCtrl_AddRef(CLegacyUpdateCtrl *This);
ULONG STDMETHODCALLTYPE LegacyUpdateCtrl_Release(CLegacyUpdateCtrl *This);

// IDispatch
STDMETHODIMP LegacyUpdateCtrl_GetTypeInfoCount(CLegacyUpdateCtrl *This, UINT *pctinfo);
STDMETHODIMP LegacyUpdateCtrl_GetTypeInfo(CLegacyUpdateCtrl *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
STDMETHODIMP LegacyUpdateCtrl_GetIDsOfNames(CLegacyUpdateCtrl *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
STDMETHODIMP LegacyUpdateCtrl_Invoke(CLegacyUpdateCtrl *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

// IOleObject
STDMETHODIMP LegacyUpdateCtrl_SetClientSite(CLegacyUpdateCtrl *This, IOleClientSite *pClientSite);

// ILegacyUpdateCtrl
STDMETHODIMP LegacyUpdateCtrl_CheckControl(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval);
STDMETHODIMP LegacyUpdateCtrl_MessageForHresult(CLegacyUpdateCtrl *This, LONG inHresult, BSTR *retval);
STDMETHODIMP LegacyUpdateCtrl_GetOSVersionInfo(CLegacyUpdateCtrl *This, OSVersionField osField, LONG systemMetric, VARIANT *retval);
STDMETHODIMP LegacyUpdateCtrl_RequestElevation(CLegacyUpdateCtrl *This);
STDMETHODIMP LegacyUpdateCtrl_CreateObject(CLegacyUpdateCtrl *This, BSTR progID, IDispatch **retval);
STDMETHODIMP LegacyUpdateCtrl_SetBrowserHwnd(CLegacyUpdateCtrl *This, IUpdateInstaller *installer);
STDMETHODIMP LegacyUpdateCtrl_GetUserType(CLegacyUpdateCtrl *This, UserType *retval);
STDMETHODIMP LegacyUpdateCtrl_get_IsRebootRequired(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval);
STDMETHODIMP LegacyUpdateCtrl_get_IsWindowsUpdateDisabled(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval);
STDMETHODIMP LegacyUpdateCtrl_RebootIfRequired(CLegacyUpdateCtrl *This);
STDMETHODIMP LegacyUpdateCtrl_ViewWindowsUpdateLog(CLegacyUpdateCtrl *This);
STDMETHODIMP LegacyUpdateCtrl_OpenWindowsUpdateSettings(CLegacyUpdateCtrl *This);
STDMETHODIMP LegacyUpdateCtrl_get_IsUsingWsusServer(CLegacyUpdateCtrl *This, VARIANT_BOOL *retval);
STDMETHODIMP LegacyUpdateCtrl_get_WsusServerUrl(CLegacyUpdateCtrl *This, BSTR *retval);
STDMETHODIMP LegacyUpdateCtrl_get_WsusStatusServerUrl(CLegacyUpdateCtrl *This, BSTR *retval);
STDMETHODIMP LegacyUpdateCtrl_BeforeUpdate(CLegacyUpdateCtrl *This);
STDMETHODIMP LegacyUpdateCtrl_AfterUpdate(CLegacyUpdateCtrl *This);

}
