#pragma once

#include "com.h"

STDMETHODIMP CreateClassFactory(IUnknown *pUnkOuter, REFIID riid, void **ppv);

class DECLSPEC_NOVTABLE CClassFactory : public IClassFactory {
public:
	CClassFactory() :
		m_refCount(1),
		createFunc(NULL),
		clsid(NULL) {}

	virtual ~CClassFactory();

	static STDMETHODIMP Create(IUnknown *pUnkOuter, REFIID riid, void **ppv);

private:
	LONG m_refCount;

public:
	STDMETHODIMP (*createFunc)(IUnknown *pUnkOuter, REFIID riid, void **ppv);
	const GUID *clsid;

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IClassFactory
	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
	STDMETHODIMP LockServer(BOOL fLock);
};
