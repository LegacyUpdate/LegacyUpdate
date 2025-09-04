#pragma once

#include <combaseapi.h>
#include <oleauto.h>
#include <ocidl.h>
#include "ComClass.h"

template<typename TImpl>
class IOleObjectImpl : public ComClass<TImpl, IOleObject> {
public:
	IOleObjectImpl(TImpl *pParent) : ComClass<TImpl, IOleObject>(pParent) {}

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

	STDMETHODIMP Update(void) {
		return S_OK;
	}

	STDMETHODIMP IsUpToDate(void) {
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
