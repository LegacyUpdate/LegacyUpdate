#pragma once

// ProgressBarControl.h : Declaration of the CProgressBarControl class.

#include "resource.h"
#include "com.h"
#include "LegacyUpdate_i.h"

STDMETHODIMP CreateProgressBarControl(IUnknown *pUnkOuter, REFIID riid, void **ppv);

class DECLSPEC_NOVTABLE CProgressBarControl :
	public IDispatchImpl<IProgressBarControl, &LIBID_LegacyUpdateLib> {
public:
	CProgressBarControl() :
		m_refCount(1),
		m_hwnd(NULL),
		m_progressHwnd(NULL) {}

	virtual ~CProgressBarControl();

private:
	LONG m_refCount;

public:
	HWND m_hwnd;
	HWND m_progressHwnd;
	BOOL m_windowOnly;

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IProgressBarControl
	STDMETHODIMP get_Value(SHORT *pValue);
	STDMETHODIMP put_Value(SHORT value);
};
