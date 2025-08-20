#pragma once

#include <combaseapi.h>
#include "ccomptr.h"

#define DEFINE_UUIDOF(cls, uuid) \
	template<> const GUID &__mingw_uuidof<cls>() { \
		return uuid; \
	}

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

		if (IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, __uuidof(TImpl))) {
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
			if (plibid != NULL) {
				CComPtr<ITypeLib> pTypeLib;
				HRESULT hr = LoadRegTypeLib(*plibid, 1, 0, LOCALE_NEUTRAL, &pTypeLib);
				if (SUCCEEDED(hr)) {
					hr = pTypeLib->GetTypeInfoOfGuid(__uuidof(TImpl), &m_pTypeInfo);
				}

				if (!SUCCEEDED(hr)) {
					return hr;
				}
			} else {
				return E_NOTIMPL;
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
		return E_NOTIMPL;
	}

	STDMETHODIMP GetClientSite(IOleClientSite **ppClientSite) {
		return E_NOTIMPL;
	}

	STDMETHODIMP SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj) {
		return S_OK;
	}

	STDMETHODIMP Close(DWORD dwSaveOption) {
		return E_NOTIMPL;
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
		return E_NOTIMPL;
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
		return E_NOTIMPL;
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
