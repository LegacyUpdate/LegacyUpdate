#pragma once

#include <combaseapi.h>
#include <oleauto.h>
#include <ocidl.h>
#include "ComClass.h"

template<typename TImpl>
class IQuickActivateImpl : public ComClass<TImpl, IQuickActivate> {
public:
	IQuickActivateImpl(TImpl *pParent) : ComClass<TImpl, IQuickActivate>(pParent) {}

	// IQuickActivate
	STDMETHODIMP QuickActivate(QACONTAINER *pQaContainer, QACONTROL *pQaControl) {
		CComPtr<IOleObject> oleObject;
		CComPtr<IViewObjectEx> viewObject;
		this->m_pParent->QueryInterface(IID_IOleObject, (void **)&oleObject);
		this->m_pParent->QueryInterface(IID_IViewObjectEx, (void **)&viewObject);
		if (!oleObject || !viewObject) {
			return E_NOINTERFACE;
		}

		oleObject->SetClientSite(pQaContainer->pClientSite);

		if (pQaContainer->pAdviseSink) {
			viewObject->SetAdvise(DVASPECT_CONTENT, 0, pQaContainer->pAdviseSink);
		}

		oleObject->GetMiscStatus(DVASPECT_CONTENT, &pQaControl->dwMiscStatus);
		viewObject->GetViewStatus(&pQaControl->dwViewStatus);

		return S_OK;
	}

	STDMETHODIMP SetContentExtent(LPSIZEL pSizel) {
		CComPtr<IOleObject> oleObject;
		this->m_pParent->QueryInterface(IID_IOleObject, (void **)&oleObject);
		if (!oleObject) {
			return E_NOINTERFACE;
		}

		return oleObject->SetExtent(DVASPECT_CONTENT, pSizel);
	}

	STDMETHODIMP GetContentExtent(LPSIZEL pSizel) {
		CComPtr<IOleObject> oleObject;
		this->m_pParent->QueryInterface(IID_IOleObject, (void **)&oleObject);
		if (!oleObject) {
			return E_NOINTERFACE;
		}

		return oleObject->GetExtent(DVASPECT_CONTENT, pSizel);
	}
};
