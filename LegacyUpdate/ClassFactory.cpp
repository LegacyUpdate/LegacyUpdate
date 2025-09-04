#include "ClassFactory.h"
#include "LegacyUpdate_i.h"
#include "LegacyUpdateCtrl.h"
#include "ElevationHelper.h"
#include "ProgressBarControl.h"
#include "dllmain.h"
#include <new>

STDMETHODIMP CClassFactory::Create(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
	if (pUnkOuter != NULL) {
		return CLASS_E_NOAGGREGATION;
	}

	CClassFactory *pThis = (CClassFactory *)CoTaskMemAlloc(sizeof(CClassFactory));
	if (pThis == NULL) {
		return E_OUTOFMEMORY;
	}

	new(pThis) CClassFactory();
	HRESULT hr = pThis->QueryInterface(riid, ppv);
	CHECK_HR_OR_RETURN(L"QueryInterface");
	pThis->Release();
	return hr;
}

CClassFactory::~CClassFactory(void) {
}

STDMETHODIMP CClassFactory::QueryInterface(REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory)) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CClassFactory::AddRef(void) {
	return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) CClassFactory::Release(void) {
	ULONG count = InterlockedDecrement(&m_refCount);
	if (count == 0) {
		this->~CClassFactory();
		CoTaskMemFree(this);
	}
	return count;
}

STDMETHODIMP CClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject) {
	return createFunc(pUnkOuter, riid, ppvObject);
}

STDMETHODIMP CClassFactory::LockServer(BOOL fLock) {
	if (fLock) {
		InterlockedIncrement(&g_serverLocks);
	} else {
		InterlockedDecrement(&g_serverLocks);
	}
	return S_OK;
}
