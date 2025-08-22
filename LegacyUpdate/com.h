#pragma once

#include <combaseapi.h>
#include <oleauto.h>
#include <ocidl.h>
#include "ccomptr.h"

#define DEFINE_UUIDOF(cls, uuid) \
	template<> const GUID &__mingw_uuidof<cls>() { \
		return uuid; \
	}

#define HIMETRIC_PER_INCH 2540
#define MAP_PIX_TO_LOGHIM(x, ppli) MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x, ppli) MulDiv((ppli), (x), HIMETRIC_PER_INCH)

template<typename TImpl, typename TInterface>
class CComClass : public TInterface {
protected:
	TImpl *m_pParent;

public:
	CComClass(TImpl *pParent) : m_pParent(pParent) {}

	// IUnknown delegation
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) {
		return m_pParent->QueryInterface(riid, ppvObject);
	}

	STDMETHODIMP_(ULONG) AddRef() {
		return m_pParent->AddRef();
	}

	STDMETHODIMP_(ULONG) Release() {
		return m_pParent->Release();
	}
};

template<typename TImpl, typename TInterface>
class COMInterfaceImpl : public CComClass<TImpl, TInterface> {
public:
	COMInterfaceImpl(TImpl *pParent) : CComClass<TImpl, TInterface>(pParent) {}
};

class IUnknownImpl {
private:
	LONG m_refCount;

protected:
	IUnknownImpl() : m_refCount(1) {}

	virtual ~IUnknownImpl() {
		if (m_refCount != 0) {
			DebugBreak();
		}
	}

public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) {
		if (ppvObject == NULL) {
			return E_POINTER;
		}

		*ppvObject = NULL;

		if (IsEqualIID(riid, IID_IUnknown)) {
			*ppvObject = (IUnknown *)this;
		}

		if (*ppvObject != NULL) {
			this->AddRef();
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef() {
		return InterlockedIncrement(&m_refCount);
	}

	STDMETHODIMP_(ULONG) Release() {
		ULONG count = InterlockedDecrement(&m_refCount);
		if (count == 0) {
			delete this;
		}
		return count;
	}
};

template<class TImpl, const GUID *plibid = NULL>
class IDispatchImpl : public TImpl
{
private:
	CComPtr<ITypeInfo> m_pTypeInfo;

public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) {
		if (ppvObject == NULL) {
			return E_POINTER;
		}

		*ppvObject = NULL;

		if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, __uuidof(TImpl))) {
			*ppvObject = (TImpl *)this;
			this->AddRef();
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	// IDispatch
	STDMETHODIMP GetTypeInfoCount(UINT *pctinfo) {
		if (pctinfo == NULL) {
			return E_POINTER;
		}
		*pctinfo = 1;
		return S_OK;
	}

	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) {
		if (ppTInfo == NULL) {
			return E_POINTER;
		}

		*ppTInfo = NULL;

		if (iTInfo != 0) {
			return DISP_E_BADINDEX;
		}

		if (m_pTypeInfo == NULL) {
			if (plibid == NULL) {
				return E_NOTIMPL;
			}

			CComPtr<ITypeLib> pTypeLib;
			HRESULT hr = LoadRegTypeLib(*plibid, 1, 0, LOCALE_NEUTRAL, &pTypeLib);
			if (SUCCEEDED(hr)) {
				hr = pTypeLib->GetTypeInfoOfGuid(__uuidof(TImpl), &m_pTypeInfo);
			}

			if (!SUCCEEDED(hr)) {
				return hr;
			}
		}

		*ppTInfo = m_pTypeInfo;
		m_pTypeInfo->AddRef();
		return S_OK;
	}

	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) {
		if (rgszNames == NULL || rgDispId == NULL) {
			return E_POINTER;
		}

		if (!IsEqualIID(riid, IID_NULL)) {
			return DISP_E_UNKNOWNINTERFACE;
		}

		CComPtr<ITypeInfo> pTypeInfo;
		HRESULT hr = GetTypeInfo(0, lcid, &pTypeInfo);
		if (!SUCCEEDED(hr)) {
			return hr;
		}

		return pTypeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
	}

	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) {
		if (!IsEqualIID(riid, IID_NULL)) {
			return DISP_E_UNKNOWNINTERFACE;
		}

		CComPtr<ITypeInfo> pTypeInfo;
		HRESULT hr = GetTypeInfo(0, lcid, &pTypeInfo);
		if (!SUCCEEDED(hr)) {
			return hr;
		}

		return pTypeInfo->Invoke(this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	}
};

template<typename TImpl>
class IOleObjectImpl : public CComClass<TImpl, IOleObject> {
public:
	IOleObjectImpl(TImpl *pParent) : CComClass<TImpl, IOleObject>(pParent) {}

	// IOleObject
	STDMETHODIMP SetClientSite(IOleClientSite *pClientSite) {
		if (this->m_pParent->m_clientSite) {
			this->m_pParent->m_clientSite->Release();
		}
		this->m_pParent->m_clientSite = pClientSite;
		if (this->m_pParent->m_clientSite) {
			this->m_pParent->m_clientSite->AddRef();
		}
		return S_OK;
	}

	STDMETHODIMP GetClientSite(IOleClientSite **ppClientSite) {
		if (ppClientSite == NULL) {
			return E_POINTER;
		}

		if (this->m_pParent->m_clientSite == NULL) {
			*ppClientSite = NULL;
			return E_FAIL;
		}

		this->m_pParent->m_clientSite->AddRef();
		*ppClientSite = this->m_pParent->m_clientSite;
		return S_OK;
	}

	STDMETHODIMP SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj) {
		return S_OK;
	}

	STDMETHODIMP Close(DWORD dwSaveOption) {
		if (this->m_pParent->m_clientSite) {
			this->m_pParent->m_clientSite->Release();
			this->m_pParent->m_clientSite = NULL;
		}
		return S_OK;
	}

	STDMETHODIMP SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk) {
		return E_NOTIMPL;
	}

	STDMETHODIMP GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) {
		return E_NOTIMPL;
	}

	STDMETHODIMP InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved) {
		return E_NOTIMPL;
	}

	STDMETHODIMP GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject) {
		return E_NOTIMPL;
	}

	STDMETHODIMP DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect) {
		switch (iVerb) {
		case OLEIVERB_INPLACEACTIVATE:
		case OLEIVERB_UIACTIVATE:
		case OLEIVERB_SHOW:
			return S_OK;

		default:
			return OLEOBJ_S_INVALIDVERB;
		}
	}

	STDMETHODIMP EnumVerbs(IEnumOLEVERB **ppEnumOleVerb) {
		if (ppEnumOleVerb == NULL) {
			return E_POINTER;
		}
		return OleRegEnumVerbs(__uuidof(TImpl), ppEnumOleVerb);
	}

	STDMETHODIMP Update() {
		return S_OK;
	}

	STDMETHODIMP IsUpToDate() {
		return S_OK;
	}

	STDMETHODIMP GetUserClassID(CLSID *pClsid) {
		if (pClsid == NULL) {
			return E_POINTER;
		}
		*pClsid = __uuidof(TImpl);
		return S_OK;
	}

	STDMETHODIMP GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType) {
		if (pszUserType == NULL) {
			return E_POINTER;
		}
		return OleRegGetUserType(__uuidof(TImpl), dwFormOfType, pszUserType);
	}

	STDMETHODIMP SetExtent(DWORD dwDrawAspect, SIZEL *psizel) {
		return S_OK;
	}

	STDMETHODIMP GetExtent(DWORD dwDrawAspect, SIZEL *psizel) {
		if (psizel == NULL) {
			return E_POINTER;
		}
		psizel->cx = 0;
		psizel->cy = 0;
		return S_OK;
	}

	STDMETHODIMP Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection) {
		return S_OK;
	}

	STDMETHODIMP Unadvise(DWORD dwConnection) {
		return E_NOTIMPL;
	}

	STDMETHODIMP EnumAdvise(IEnumSTATDATA **ppenumAdvise) {
		return E_NOTIMPL;
	}

	STDMETHODIMP GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus) {
		return OleRegGetMiscStatus(__uuidof(TImpl), dwAspect, pdwStatus);
	}

	STDMETHODIMP SetColorScheme(LOGPALETTE *pLogpal) {
		return E_NOTIMPL;
	}
};

template<typename TImpl>
class IViewObjectExImpl : public CComClass<TImpl, IViewObjectEx> {
public:
	IViewObjectExImpl(TImpl *pParent) : CComClass<TImpl, IViewObjectEx>(pParent) {}

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

template<typename TImpl>
class IOleInPlaceObjectImpl : public CComClass<TImpl, IOleInPlaceObject> {
public:
	IOleInPlaceObjectImpl(TImpl *pParent) : CComClass<TImpl, IOleInPlaceObject>(pParent) {}

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

template<typename TImpl>
class IOleInPlaceActiveObjectImpl : public CComClass<TImpl, IOleInPlaceActiveObject> {
public:
	IOleInPlaceActiveObjectImpl(TImpl *pParent) : CComClass<TImpl, IOleInPlaceActiveObject>(pParent) {}

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
