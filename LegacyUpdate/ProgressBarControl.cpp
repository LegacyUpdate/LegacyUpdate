// ProgressBarControl.cpp : Implementation of CProgressBarControl
#include "stdafx.h"
#include "ProgressBarControl.h"
#include "Dispatch.h"
#include <commctrl.h>

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef PBS_MARQUEE
#define PBS_MARQUEE 0x08
#endif

#ifndef PBM_SETMARQUEE
#define PBM_SETMARQUEE (WM_USER + 10)
#endif

static CProgressBarControlVtbl CProgressBarControlVtable = {
	ProgressBarControl_QueryInterface,
	ProgressBarControl_AddRef,
	ProgressBarControl_Release,
	ProgressBarControl_GetTypeInfoCount,
	ProgressBarControl_GetTypeInfo,
	ProgressBarControl_GetIDsOfNames,
	ProgressBarControl_Invoke,
	ProgressBarControl_get_Value,
	ProgressBarControl_put_Value
};

STDMETHODIMP CreateProgressBarControl(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
	if (pUnkOuter != NULL) {
		return CLASS_E_NOAGGREGATION;
	}

	CProgressBarControl *pThis = (CProgressBarControl *)CoTaskMemAlloc(sizeof(CProgressBarControl));
	if (pThis == NULL) {
		return E_OUTOFMEMORY;
	}

	ZeroMemory(pThis, sizeof(CProgressBarControl));
	pThis->lpVtbl = &CProgressBarControlVtable;
	pThis->refCount = 1;
	pThis->windowOnly = TRUE;

	HRESULT hr = ProgressBarControl_QueryInterface(pThis, riid, ppv);
	ProgressBarControl_Release(pThis);

	return hr;
}

STDMETHODIMP ProgressBarControl_QueryInterface(CProgressBarControl *This, REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, IID_IProgressBarControl)) {
		*ppvObject = This;
		ProgressBarControl_AddRef(This);
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ProgressBarControl_AddRef(CProgressBarControl *This) {
	return InterlockedIncrement(&This->refCount);
}

ULONG STDMETHODCALLTYPE ProgressBarControl_Release(CProgressBarControl *This) {
	ULONG refCount = InterlockedDecrement(&This->refCount);

	if (refCount == 0) {
		if (This->progressHwnd) {
			DestroyWindow(This->progressHwnd);
		}

		CoTaskMemFree(This);
	}

	return refCount;
}

STDMETHODIMP ProgressBarControl_GetTypeInfoCount(CProgressBarControl *This, UINT *pctinfo) {
	if (pctinfo == NULL) {
		return E_POINTER;
	}
	*pctinfo = 1;
	return S_OK;
}

STDMETHODIMP ProgressBarControl_GetTypeInfo(CProgressBarControl *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) {
	return Dispatch_GetTypeInfo((IDispatch *)This, IID_IProgressBarControl, iTInfo, lcid, ppTInfo);
}

STDMETHODIMP ProgressBarControl_GetIDsOfNames(CProgressBarControl *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) {
	return Dispatch_GetIDsOfNames((IDispatch *)This, riid, rgszNames, cNames, lcid, rgDispId);
}

STDMETHODIMP ProgressBarControl_Invoke(CProgressBarControl *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) {
	return Dispatch_Invoke((IDispatch *)This, dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

STDMETHODIMP ProgressBarControl_get_Value(CProgressBarControl *This, SHORT *pValue) {
	if (pValue == NULL) {
		return E_POINTER;
	}

	if (This->progressHwnd == NULL) {
		*pValue = 0;
		return S_OK;
	}

	if (GetWindowLongPtr(This->progressHwnd, GWL_STYLE) & PBS_MARQUEE) {
		// Marquee - no value
		*pValue = -1;
	} else {
		// Normal - return PBM_GETPOS
		*pValue = (SHORT)SendMessage(This->progressHwnd, PBM_GETPOS, 0, 0);
	}

	return S_OK;
}

STDMETHODIMP ProgressBarControl_put_Value(CProgressBarControl *This, SHORT value) {
	if (This->progressHwnd == NULL) {
		return E_FAIL;
	}

	if (value == -1) {
		// Marquee style
		SetWindowLongPtr(This->progressHwnd, GWL_STYLE, GetWindowLongPtr(This->progressHwnd, GWL_STYLE) | PBS_MARQUEE);
		SendMessage(This->progressHwnd, PBM_SETMARQUEE, TRUE, 100);
	} else {
		// Normal style
		SHORT oldValue = -1;
		ProgressBarControl_get_Value(This, &oldValue);
		if (oldValue == -1) {
			SendMessage(This->progressHwnd, PBM_SETMARQUEE, FALSE, 0);
			SetWindowLongPtr(This->progressHwnd, GWL_STYLE, GetWindowLongPtr(This->progressHwnd, GWL_STYLE) & ~PBS_MARQUEE);
			SendMessage(This->progressHwnd, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		}
		SendMessage(This->progressHwnd, PBM_SETPOS, value, 0);
	}

	return S_OK;
}
