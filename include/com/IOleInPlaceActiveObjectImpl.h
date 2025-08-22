#pragma once

#include <combaseapi.h>
#include <oleauto.h>
#include <ocidl.h>
#include "ComClass.h"

template<typename TImpl>
class IOleInPlaceActiveObjectImpl : public ComClass<TImpl, IOleInPlaceActiveObject> {
public:
	IOleInPlaceActiveObjectImpl(TImpl *pParent) : ComClass<TImpl, IOleInPlaceActiveObject>(pParent) {}

	// IOleWindow
	STDMETHODIMP GetWindow(HWND *phwnd) {
		if (phwnd == NULL) {
			return E_POINTER;
		}
		*phwnd = this->m_pParent->m_innerHwnd;
		return *phwnd == NULL ? E_FAIL : S_OK;
	}

	STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode) {
		return E_NOTIMPL;
	}

	// IOleInPlaceActiveObject
	STDMETHODIMP TranslateAccelerator(LPMSG lpmsg) {
		return S_FALSE;
	}

	STDMETHODIMP OnFrameWindowActivate(WINBOOL fActivate) {
		return S_OK;
	}

	STDMETHODIMP OnDocWindowActivate(WINBOOL fActivate) {
		return S_OK;
	}

	STDMETHODIMP ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, WINBOOL fFrameWindow) {
		return S_OK;
	}

	STDMETHODIMP EnableModeless(WINBOOL fEnable) {
		return S_OK;
	}
};
