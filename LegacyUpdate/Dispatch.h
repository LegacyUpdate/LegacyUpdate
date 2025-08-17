#pragma once

#include <windows.h>

STDMETHODIMP Dispatch_GetTypeInfo(IDispatch *This, REFIID riid, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
STDMETHODIMP Dispatch_GetIDsOfNames(IDispatch *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
STDMETHODIMP Dispatch_Invoke(IDispatch *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
