#pragma once

#include <combaseapi.h>
#include <oleauto.h>
#include <ocidl.h>
#include "ComClass.h"

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
