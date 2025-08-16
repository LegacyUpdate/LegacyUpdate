#pragma once

// ProgressBarControl.h : Declaration of the CProgressBarControl class.

#include "resource.h"
#include "LegacyUpdate_i.h"

#ifdef __cplusplus
extern "C" {
#endif

STDMETHODIMP CreateProgressBarControl(IUnknown *pUnkOuter, REFIID riid, void **ppv);

typedef struct CProgressBarControl CProgressBarControl;

typedef struct CProgressBarControlVtbl {
	// IUnknown
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(CProgressBarControl *This, REFIID riid, void **ppvObject);
	ULONG   (STDMETHODCALLTYPE *AddRef)(CProgressBarControl *This);
	ULONG   (STDMETHODCALLTYPE *Release)(CProgressBarControl *This);

	// IDispatch
	HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(CProgressBarControl *This, UINT *pctinfo);
	HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(CProgressBarControl *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(CProgressBarControl *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	HRESULT (STDMETHODCALLTYPE *Invoke)(CProgressBarControl *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

	// IProgressBarControl
	HRESULT (STDMETHODCALLTYPE *get_Value)(CProgressBarControl *This, SHORT *pValue);
	HRESULT (STDMETHODCALLTYPE *put_Value)(CProgressBarControl *This, SHORT value);
} CProgressBarControlVtbl;

struct CProgressBarControl {
	CProgressBarControlVtbl *lpVtbl;
	LONG refCount;

	HWND hwnd;
	HWND progressHwnd;
	BOOL windowOnly;
};

// IUnknown
STDMETHODIMP ProgressBarControl_QueryInterface(CProgressBarControl *This, REFIID riid, void **ppvObject);
ULONG STDMETHODCALLTYPE ProgressBarControl_AddRef(CProgressBarControl *This);
ULONG STDMETHODCALLTYPE ProgressBarControl_Release(CProgressBarControl *This);

// IDispatch
STDMETHODIMP ProgressBarControl_GetTypeInfoCount(CProgressBarControl *This, UINT *pctinfo);
STDMETHODIMP ProgressBarControl_GetTypeInfo(CProgressBarControl *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
STDMETHODIMP ProgressBarControl_GetIDsOfNames(CProgressBarControl *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
STDMETHODIMP ProgressBarControl_Invoke(CProgressBarControl *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

// IProgressBarControl
STDMETHODIMP ProgressBarControl_get_Value(CProgressBarControl *This, SHORT *pValue);
STDMETHODIMP ProgressBarControl_put_Value(CProgressBarControl *This, SHORT value);

#ifdef __cplusplus
}
#endif
