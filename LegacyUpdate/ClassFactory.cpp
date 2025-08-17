#include "ClassFactory.h"
#include "LegacyUpdate_i.h"
#include "LegacyUpdateCtrl.h"
#include "ElevationHelper.h"
#include "ProgressBarControl.h"
#include "dllmain.h"

static CClassFactoryVtbl CClassFactoryVtable = {
	ClassFactory_QueryInterface,
	ClassFactory_AddRef,
	ClassFactory_Release,
	ClassFactory_CreateInstance,
	ClassFactory_LockServer
};

STDMETHODIMP CreateClassFactory(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
	if (pUnkOuter != NULL) {
		return CLASS_E_NOAGGREGATION;
	}

	CClassFactory *pThis = (CClassFactory *)CoTaskMemAlloc(sizeof(CClassFactory));
	if (pThis == NULL) {
		return E_OUTOFMEMORY;
	}

	ZeroMemory(pThis, sizeof(CClassFactory));
	pThis->lpVtbl = &CClassFactoryVtable;
	pThis->refCount = 1;

	HRESULT hr = ClassFactory_QueryInterface(pThis, riid, ppv);
	ClassFactory_Release(pThis);
	return hr;
}

STDMETHODIMP ClassFactory_QueryInterface(CClassFactory *This, REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory)) {
		*ppvObject = This;
		ClassFactory_AddRef(This);
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ClassFactory_AddRef(CClassFactory *This) {
	return InterlockedIncrement(&This->refCount);
}

ULONG STDMETHODCALLTYPE ClassFactory_Release(CClassFactory *This) {
	ULONG refCount = InterlockedDecrement(&This->refCount);
	if (refCount == 0) {
		CoTaskMemFree(This);
	}
	return refCount;
}

STDMETHODIMP ClassFactory_CreateInstance(CClassFactory *This, IUnknown *pUnkOuter, REFIID riid, void **ppvObject) {
	return This->createFunc(pUnkOuter, riid, ppvObject);
}

STDMETHODIMP ClassFactory_LockServer(CClassFactory *This, BOOL fLock) {
	if (fLock) {
		InterlockedIncrement(&g_serverLocks);
	} else {
		InterlockedDecrement(&g_serverLocks);
	}
	return S_OK;
}
