#pragma once

#include <combaseapi.h>
#include <oleauto.h>
#include <ocidl.h>
#include "ComClass.h"

template<typename TImpl>
class IViewObjectExImpl : public ComClass<TImpl, IViewObjectEx> {
public:
	IViewObjectExImpl(TImpl *pParent) : ComClass<TImpl, IViewObjectEx>(pParent) {}

	// IViewObject
	STDMETHODIMP Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd, HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds, BOOL (STDMETHODCALLTYPE *pfnContinue)(ULONG_PTR dwContinue), ULONG_PTR dwContinue) {
		return this->m_pParent->OnDraw(dwDrawAspect, lindex, pvAspect, ptd, hdcTargetDev, hdcDraw, lprcBounds, lprcWBounds, pfnContinue, dwContinue);
	}

	STDMETHODIMP GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet) {
		if (ppColorSet == NULL) {
			return E_POINTER;
		}
		*ppColorSet = NULL;
		return S_FALSE;
	}

	STDMETHODIMP Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze) {
		return E_NOTIMPL;
	}

	STDMETHODIMP Unfreeze(DWORD dwFreeze) {
		return E_NOTIMPL;
	}

	STDMETHODIMP SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink) {
		if (this->m_pParent->m_adviseSink) {
			this->m_pParent->m_adviseSink->Release();
		}
		this->m_pParent->m_adviseSink = pAdvSink;
		if (this->m_pParent->m_adviseSink) {
			this->m_pParent->m_adviseSink->AddRef();
		}
		return S_OK;
	}

	STDMETHODIMP GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink) {
		if (pAspects) {
			*pAspects = DVASPECT_CONTENT;
		}
		if (pAdvf) {
			*pAdvf = 0;
		}
		if (ppAdvSink) {
			*ppAdvSink = this->m_pParent->m_adviseSink;
		}
		return S_OK;
	}

	// IViewObject2
	STDMETHODIMP GetExtent(DWORD dwAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel) {
		if (lpsizel == NULL) {
			return E_POINTER;
		}

		if (dwAspect != DVASPECT_CONTENT) {
			return DV_E_DVASPECT;
		}

		if (!this->m_pParent->m_innerHwnd) {
			lpsizel->cx = 0;
			lpsizel->cy = 0;
			return E_FAIL;
		}

		RECT rect;
		GetClientRect(this->m_pParent->m_innerHwnd, &rect);

		// Convert to HIMETRIC
		HDC hdc = GetDC(NULL);
		int ppiX = GetDeviceCaps(hdc, LOGPIXELSX);
		int ppiY = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(NULL, hdc);

		lpsizel->cx = MAP_PIX_TO_LOGHIM(rect.right - rect.left, ppiX);
		lpsizel->cy = MAP_PIX_TO_LOGHIM(rect.bottom - rect.top, ppiY);
		return S_OK;
	}

	// IViewObjectEx
	STDMETHODIMP GetRect(DWORD dwAspect, LPRECTL pRect) {
		if (pRect == NULL) {
			return E_POINTER;
		}

		if (dwAspect != DVASPECT_CONTENT) {
			return DV_E_DVASPECT;
		}

		if (!this->m_pParent->m_innerHwnd) {
			return E_FAIL;
		}

		RECT rect;
		GetClientRect(this->m_pParent->m_innerHwnd, &rect);
		pRect->left = rect.left;
		pRect->top = rect.top;
		pRect->right = rect.right;
		pRect->bottom = rect.bottom;
		return S_OK;
	}

	STDMETHODIMP GetViewStatus(DWORD *pdwStatus) {
		if (pdwStatus == NULL) {
			return E_POINTER;
		}
		*pdwStatus = VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE;
		return S_OK;
	}

	STDMETHODIMP QueryHitPoint(DWORD dwAspect, LPCRECT pRectBounds, POINT ptlLoc, LONG lCloseHint, DWORD *pHitResult) {
		if (pRectBounds == NULL || pHitResult == NULL) {
			return E_POINTER;
		}
		*pHitResult = PtInRect(pRectBounds, *(POINT *)&ptlLoc) ? HITRESULT_HIT : HITRESULT_OUTSIDE;
		return S_OK;
	}

	STDMETHODIMP QueryHitRect(DWORD dwAspect, LPCRECT pRectBounds, LPCRECT pRectLoc, LONG lCloseHint, DWORD *pHitResult) {
		if (pRectBounds == NULL || pRectLoc == NULL || pHitResult == NULL) {
			return E_POINTER;
		}
		*pHitResult = IntersectRect((LPRECT)pHitResult, pRectBounds, pRectLoc) ? HITRESULT_HIT : HITRESULT_OUTSIDE;
		return S_OK;
	}

	STDMETHODIMP GetNaturalExtent(DWORD dwAspect, LONG lindex, DVTARGETDEVICE *ptd, HDC hicTargetDev, DVEXTENTINFO *pExtentInfo, LPSIZEL pSizel) {
		return GetExtent(dwAspect, lindex, ptd, pSizel);
	}
};
