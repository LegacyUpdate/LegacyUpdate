// ProgressBarControl.cpp : Implementation of CProgressBarControl

#include "ProgressBarControl.h"
#include <commctrl.h>
#include <new>

DEFINE_UUIDOF(CProgressBarControl, CLSID_ProgressBarControl);

STDMETHODIMP CreateProgressBarControl(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
	if (pUnkOuter != NULL) {
		return CLASS_E_NOAGGREGATION;
	}

	CProgressBarControl *pThis = (CProgressBarControl *)CoTaskMemAlloc(sizeof(CProgressBarControl));
	if (pThis == NULL) {
		return E_OUTOFMEMORY;
	}

	new(pThis) CProgressBarControl();
	HRESULT hr = pThis->QueryInterface(riid, ppv);
	pThis->Release();

	return hr;
}

CProgressBarControl::~CProgressBarControl() {
	if (m_progressHwnd) {
		DestroyWindow(m_progressHwnd);
	}
}

STDMETHODIMP CProgressBarControl::QueryInterface(REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	*ppvObject = NULL;

	// Handle IUnknown
	if (IsEqualIID(riid, IID_IUnknown)) {
		*ppvObject = (IUnknown *)(IProgressBarControl *)this;
		AddRef();
		return S_OK;
	}

	// Let IDispatchImpl handle IDispatch and IProgressBarControl
	HRESULT hr = IDispatchImpl<IProgressBarControl, &LIBID_LegacyUpdateLib>::QueryInterface(riid, ppvObject);
	if (SUCCEEDED(hr)) {
		return hr;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CProgressBarControl::AddRef() {
	return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) CProgressBarControl::Release() {
	ULONG count = InterlockedDecrement(&m_refCount);
	if (count == 0) {
		this->~CProgressBarControl();
		CoTaskMemFree(this);
	}
	return count;
}

STDMETHODIMP CProgressBarControl::get_Value(SHORT *pValue) {
	if (pValue == NULL) {
		return E_POINTER;
	}

	if (m_progressHwnd == NULL) {
		*pValue = 0;
		return S_OK;
	}

	if (GetWindowLongPtr(m_progressHwnd, GWL_STYLE) & PBS_MARQUEE) {
		// Marquee - no value
		*pValue = -1;
	} else {
		// Normal - return PBM_GETPOS
		*pValue = (SHORT)SendMessage(m_progressHwnd, PBM_GETPOS, 0, 0);
	}

	return S_OK;
}

STDMETHODIMP CProgressBarControl::put_Value(SHORT value) {
	if (m_progressHwnd == NULL) {
		return E_FAIL;
	}

	if (value == -1) {
		// Marquee style
		SetWindowLongPtr(m_progressHwnd, GWL_STYLE, GetWindowLongPtr(m_progressHwnd, GWL_STYLE) | PBS_MARQUEE);
		SendMessage(m_progressHwnd, PBM_SETMARQUEE, TRUE, 100);
	} else {
		// Normal style
		SHORT oldValue = -1;
		get_Value(&oldValue);
		if (oldValue == -1) {
			SendMessage(m_progressHwnd, PBM_SETMARQUEE, FALSE, 0);
			SetWindowLongPtr(m_progressHwnd, GWL_STYLE, GetWindowLongPtr(m_progressHwnd, GWL_STYLE) & ~PBS_MARQUEE);
			SendMessage(m_progressHwnd, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		}
		SendMessage(m_progressHwnd, PBM_SETPOS, value, 0);
	}

	return S_OK;
}
