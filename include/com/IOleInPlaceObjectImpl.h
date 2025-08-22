#pragma once

#include <combaseapi.h>
#include <oleauto.h>
#include <ocidl.h>
#include "comclass.h"

template<typename TImpl>
class IOleInPlaceObjectImpl : public ComClass<TImpl, IOleInPlaceObject> {
public:
	IOleInPlaceObjectImpl(TImpl *pParent) : ComClass<TImpl, IOleInPlaceObject>(pParent) {}

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

	// IOleInPlaceObject
	STDMETHODIMP InPlaceDeactivate() {
		return S_OK;
	}

	STDMETHODIMP UIDeactivate() {
		return S_OK;
	}

	STDMETHODIMP SetObjectRects(LPCRECT lprcPosRect, LPCRECT lprcClipRect) {
		if (lprcPosRect == NULL) {
			return E_POINTER;
		}

		if (this->m_pParent->m_innerHwnd) {
			SetWindowPos(
				this->m_pParent->m_innerHwnd, NULL,
				lprcPosRect->left, lprcPosRect->top,
				lprcPosRect->right - lprcPosRect->left,
				lprcPosRect->bottom - lprcPosRect->top,
				SWP_NOZORDER | SWP_SHOWWINDOW
			);
		}

		return S_OK;
	}

	STDMETHODIMP ReactivateAndUndo() {
		return E_NOTIMPL;
	}
};
