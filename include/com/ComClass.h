#pragma once

#include <combaseapi.h>
#include <oleauto.h>
#include <ocidl.h>

template<typename TImpl, typename TInterface>
class ComClass : public TInterface {
protected:
	TImpl *m_pParent;

public:
	ComClass(TImpl *pParent) : m_pParent(pParent) {}

	// IUnknown delegation
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) {
		return m_pParent->QueryInterface(riid, ppvObject);
	}

	STDMETHODIMP_(ULONG) AddRef(void) {
		return m_pParent->AddRef();
	}

	STDMETHODIMP_(ULONG) Release(void) {
		return m_pParent->Release();
	}
};
