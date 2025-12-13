#pragma once

#include <combaseapi.h>
#include <cguid.h>
#include <oleauto.h>
#include <ocidl.h>
#include "ComPtr.h"

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

		if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, __uuidof(TImpl))) {
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
			if (plibid == NULL) {
				return E_NOTIMPL;
			}

			CComPtr<ITypeLib> pTypeLib;
			HRESULT hr = LoadRegTypeLib(*plibid, 1, 0, LOCALE_NEUTRAL, &pTypeLib);
			CHECK_HR_OR_RETURN(L"LoadRegTypeLib");

			hr = pTypeLib->GetTypeInfoOfGuid(__uuidof(TImpl), &m_pTypeInfo);
			CHECK_HR_OR_RETURN(L"GetTypeInfoOfGuid");
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
		CHECK_HR_OR_RETURN(L"GetTypeInfo");

		return pTypeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
	}

	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) {
		if (!IsEqualIID(riid, IID_NULL)) {
			return DISP_E_UNKNOWNINTERFACE;
		}

		CComPtr<ITypeInfo> pTypeInfo;
		HRESULT hr = GetTypeInfo(0, lcid, &pTypeInfo);
		CHECK_HR_OR_RETURN(L"GetTypeInfo");

		return pTypeInfo->Invoke(this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	}
};
