#pragma once

STDMETHODIMP CreateClassFactory(IUnknown *pUnkOuter, REFIID riid, void **ppv);

typedef struct CClassFactory CClassFactory;

typedef struct CClassFactoryVtbl {
	// IUnknown
	HRESULT (STDMETHODCALLTYPE *QueryInterface)(CClassFactory *This, REFIID riid, void **ppvObject);
	ULONG   (STDMETHODCALLTYPE *AddRef)(CClassFactory *This);
	ULONG   (STDMETHODCALLTYPE *Release)(CClassFactory *This);

	// IClassFactory
	HRESULT (STDMETHODCALLTYPE *CreateInstance)(CClassFactory *This, IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
	HRESULT (STDMETHODCALLTYPE *LockServer)(CClassFactory *This, BOOL fLock);
} CClassFactoryVtbl;

struct CClassFactory {
	const struct CClassFactoryVtbl *lpVtbl;
	LONG refCount;
	STDMETHODIMP (*createFunc)(IUnknown *pUnkOuter, REFIID riid, void **ppv);
	const GUID *clsid;
};

// IUnknown
STDMETHODIMP ClassFactory_QueryInterface(CClassFactory *This, REFIID riid, void **ppvObject);
ULONG STDMETHODCALLTYPE ClassFactory_AddRef(CClassFactory *This);
ULONG STDMETHODCALLTYPE ClassFactory_Release(CClassFactory *This);

// IClassFactory
STDMETHODIMP ClassFactory_CreateInstance(CClassFactory *This, IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
STDMETHODIMP ClassFactory_LockServer(CClassFactory *This, BOOL fLock);
