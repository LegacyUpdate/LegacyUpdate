#include "Dispatch.h"
#include "LegacyUpdate_i.h"

STDMETHODIMP Dispatch_GetTypeInfo(IDispatch *This, REFIID riid, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) {
	if (ppTInfo == NULL) {
		return E_POINTER;
	}

	*ppTInfo = NULL;

	if (iTInfo != 0) {
		return DISP_E_BADINDEX;
	}

	static ITypeInfo *pTypeInfo = NULL;
	if (pTypeInfo == NULL) {
		ITypeLib *pTypeLib = NULL;
		HRESULT hr = LoadRegTypeLib(LIBID_LegacyUpdateLib, 1, 0, LOCALE_NEUTRAL, &pTypeLib);
		if (SUCCEEDED(hr)) {
			hr = pTypeLib->GetTypeInfoOfGuid(riid, &pTypeInfo);
			pTypeLib->Release();
		}
		if (!SUCCEEDED(hr)) {
			return hr;
		}
	}

	*ppTInfo = pTypeInfo;
	pTypeInfo->AddRef();
	return S_OK;
}

STDMETHODIMP Dispatch_GetIDsOfNames(IDispatch *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) {
	if (rgszNames == NULL || rgDispId == NULL) {
		return E_POINTER;
	}

	if (!IsEqualIID(riid, IID_NULL)) {
		return DISP_E_UNKNOWNINTERFACE;
	}

	ITypeInfo *pTypeInfo = NULL;
	HRESULT hr = Dispatch_GetTypeInfo(This, riid, 0, lcid, &pTypeInfo);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = pTypeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
	pTypeInfo->Release();

	return hr;
}

STDMETHODIMP Dispatch_Invoke(IDispatch *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) {
	if (!IsEqualIID(riid, IID_NULL)) {
		return DISP_E_UNKNOWNINTERFACE;
	}

	ITypeInfo *pTypeInfo = NULL;
	HRESULT hr = Dispatch_GetTypeInfo(This, riid, 0, lcid, &pTypeInfo);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	hr = pTypeInfo->Invoke(This, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	pTypeInfo->Release();

	return hr;
}
