/*
 * ReactOS ATL
 *
 * Copyright 2009 Andrew Hill <ash77@reactos.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "atlcore.h"


#ifdef _MSC_VER
// It is common to use this in ATL constructors. They only store this for later use, so the usage is safe.
#pragma warning(disable:4355)
#endif

#ifndef _ATL_PACKING
#define _ATL_PACKING 8
#endif

#ifndef _ATL_FREE_THREADED
#ifndef _ATL_APARTMENT_THREADED
#ifndef _ATL_SINGLE_THREADED
#define _ATL_FREE_THREADED
#endif
#endif
#endif

#ifndef ATLTRY
#define ATLTRY(x) x;
#endif

#ifdef _ATL_DISABLE_NO_VTABLE
#define ATL_NO_VTABLE
#else
#define ATL_NO_VTABLE __declspec(novtable)
#endif

namespace ATL
{

inline HRESULT AtlHresultFromLastError() throw()
{
    DWORD dwError = ::GetLastError();
    return HRESULT_FROM_WIN32(dwError);
}

template <class T>
class _NoAddRefReleaseOnCComPtr : public T
{
  private:
    virtual ULONG STDMETHODCALLTYPE AddRef() = 0;
    virtual ULONG STDMETHODCALLTYPE Release() = 0;
};

// Begin code borrowed from Microsoft <atlcomcli.h>
//CComPtrBase provides the basis for all other smart pointers
//The other smartpointers add their own constructors and operators
template <class T>
class CComPtrBase
{
protected:
    CComPtrBase() throw()
    {
        p = NULL;
    }
    CComPtrBase(_Inout_opt_ T* lp) throw()
    {
        p = lp;
        if (p != NULL)
            p->AddRef();
    }
    void Swap(CComPtrBase& other)
    {
        T* pTemp = p;
        p = other.p;
        other.p = pTemp;
    }
public:
    typedef T _PtrClass;
    ~CComPtrBase() throw()
    {
        if (p)
            p->Release();
    }
    operator T*() const throw()
    {
        return p;
    }
    T& operator*() const
    {
        ATLENSURE(p!=NULL);
        return *p;
    }
    //The assert on operator& usually indicates a bug.  If this is really
    //what is needed, however, take the address of the p member explicitly.
    T** operator&() throw()
    {
        ATLASSERT(p==NULL);
        return &p;
    }
    _NoAddRefReleaseOnCComPtr<T>* operator->() const throw()
    {
        ATLASSERT(p!=NULL);
        return (_NoAddRefReleaseOnCComPtr<T>*)p;
    }
    bool operator!() const throw()
    {
        return (p == NULL);
    }
    bool operator<(_In_opt_ T* pT) const throw()
    {
        return p < pT;
    }
    bool operator!=(_In_opt_ T* pT) const
    {
        return !operator==(pT);
    }
    bool operator==(_In_opt_ T* pT) const throw()
    {
        return p == pT;
    }

    // Release the interface and set to NULL
    void Release() throw()
    {
        T* pTemp = p;
        if (pTemp)
        {
            p = NULL;
            pTemp->Release();
        }
    }
    // Compare two objects for equivalence
    inline bool IsEqualObject(_Inout_opt_ IUnknown* pOther) throw();

    // Attach to an existing interface (does not AddRef)
    void Attach(_In_opt_ T* p2) throw()
    {
        if (p)
        {
            ULONG ref = p->Release();
            (ref);
            // Attaching to the same object only works if duplicate references are being coalesced.  Otherwise
            // re-attaching will cause the pointer to be released and may cause a crash on a subsequent dereference.
            ATLASSERT(ref != 0 || p2 != p);
        }
        p = p2;
    }
    // Detach the interface (does not Release)
    T* Detach() throw()
    {
        T* pt = p;
        p = NULL;
        return pt;
    }
    _Check_return_ HRESULT CopyTo(_COM_Outptr_result_maybenull_ T** ppT) throw()
    {
        ATLASSERT(ppT != NULL);
        if (ppT == NULL)
            return E_POINTER;
        *ppT = p;
        if (p)
            p->AddRef();
        return S_OK;
    }
    _Check_return_ HRESULT SetSite(_Inout_opt_ IUnknown* punkParent) throw()
    {
        return AtlSetChildSite(p, punkParent);
    }
    _Check_return_ HRESULT Advise(
        _Inout_ IUnknown* pUnk,
        _In_ const IID& iid,
        _Out_ LPDWORD pdw) throw()
    {
        return AtlAdvise(p, pUnk, iid, pdw);
    }
    _Check_return_ HRESULT CoCreateInstance(
        _In_ REFCLSID rclsid,
        _Inout_opt_ LPUNKNOWN pUnkOuter = NULL,
        _In_ DWORD dwClsContext = CLSCTX_ALL) throw()
    {
        ATLASSERT(p == NULL);
        return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
    }
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
    _Check_return_ HRESULT CoCreateInstance(
        _In_z_ LPCOLESTR szProgID,
        _Inout_opt_ LPUNKNOWN pUnkOuter = NULL,
        _In_ DWORD dwClsContext = CLSCTX_ALL) throw()
    {
        CLSID clsid;
        HRESULT hr = CLSIDFromProgID(szProgID, &clsid);
        ATLASSERT(p == NULL);
        if (SUCCEEDED(hr))
            hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
        return hr;
    }
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
    template <class Q>
    _Check_return_ HRESULT QueryInterface(_Outptr_ Q** pp) const throw()
    {
        ATLASSERT(pp != NULL);
        return p->QueryInterface(__uuidof(Q), (void**)pp);
    }
    T* p;
};
// End code borrowed from Microsoft <atlcomcli.h>

template<class T>
class CComPtr :
    public CComPtrBase<T>
{
public:
    T *p;
public:
    CComPtr()
    {
        p = NULL;
    }

    CComPtr(T *lp)
    {
        p = lp;
        if (p != NULL)
            p->AddRef();
    }

    CComPtr(const CComPtr<T> &lp)
    {
        p = lp.p;
        if (p != NULL)
            p->AddRef();
    }

    ~CComPtr()
    {
        if (p != NULL)
            p->Release();
    }

    T *operator = (T *lp)
    {
        T* pOld = p;

        p = lp;
        if (p != NULL)
            p->AddRef();

        if (pOld != NULL)
            pOld->Release();

        return *this;
    }

    T *operator = (const CComPtr<T> &lp)
    {
        T* pOld = p;

        p = lp.p;
        if (p != NULL)
            p->AddRef();

        if (pOld != NULL)
            pOld->Release();

        return *this;
    }

    // We cannot enable this until gcc starts supporting __uuidof
    // See CORE-12710
#if 0
    template <typename Q>
    T* operator=(const CComPtr<Q>& lp)
    {
        T* pOld = p;

        if (!lp.p || FAILED(lp.p->QueryInterface(__uuidof(T), (void**)(IUnknown**)&p)))
            p = NULL;

        if (pOld != NULL)
            pOld->Release();

        return *this;
    }
#endif

    void Release()
    {
        if (p != NULL)
        {
            p->Release();
            p = NULL;
        }
    }

    void Attach(T *lp)
    {
        if (p != NULL)
            p->Release();
        p = lp;
    }

    T *Detach()
    {
        T *saveP;

        saveP = p;
        p = NULL;
        return saveP;
    }

    T **operator & ()
    {
        ATLASSERT(p == NULL);
        return &p;
    }

    operator T * ()
    {
        return p;
    }

    _NoAddRefReleaseOnCComPtr<T> *operator -> () const
    {
        ATLASSERT(p != NULL);
        return (_NoAddRefReleaseOnCComPtr<T> *)p;
    }
};

// Begin code borrowed from Microsoft <atlcomcli.h>
//specialization for IDispatch
template <>
class CComPtr<IDispatch> :
    public CComPtrBase<IDispatch>
{
public:
    CComPtr() throw()
    {
    }
    CComPtr(_Inout_opt_ IDispatch* lp) throw() :
        CComPtrBase<IDispatch>(lp)
    {
    }
    CComPtr(_Inout_ const CComPtr<IDispatch>& lp) throw() :
        CComPtrBase<IDispatch>(lp.p)
    {
    }
    IDispatch* operator=(_Inout_opt_ IDispatch* lp) throw()
    {
        if(*this!=lp)
        {
            CComPtr(lp).Swap(*this);
        }
        return *this;
    }
    IDispatch* operator=(_Inout_ const CComPtr<IDispatch>& lp) throw()
    {
        if(*this!=lp)
        {
            CComPtr(lp).Swap(*this);
        }
        return *this;
    }
    CComPtr(_Inout_ CComPtr<IDispatch>&& lp) throw() :
        CComPtrBase<IDispatch>()
    {
        this->p = lp.p;
        lp.p = NULL;
    }
    IDispatch* operator=(_Inout_ CComPtr<IDispatch>&& lp) throw()
    {
        CComPtr(static_cast<CComPtr&&>(lp)).Swap(*this);
        return *this;
    }
    _Check_return_ HRESULT GetPropertyByName(
        _In_z_ LPCOLESTR lpsz,
        _Out_ VARIANT* pVar) throw()
    {
        ATLASSERT(this->p);
        ATLASSERT(pVar);
        DISPID dwDispID;
        HRESULT hr = GetIDOfName(lpsz, &dwDispID);
        if (SUCCEEDED(hr))
            hr = GetProperty(dwDispID, pVar);
        return hr;
    }
    _Check_return_ HRESULT GetProperty(
        _In_ DISPID dwDispID,
        _Out_ VARIANT* pVar) throw()
    {
        return GetProperty(this->p, dwDispID, pVar);
    }
    _Check_return_ HRESULT PutPropertyByName(
        _In_z_ LPCOLESTR lpsz,
        _In_ VARIANT* pVar) throw()
    {
        ATLASSERT(this->p);
        ATLASSERT(pVar);
        DISPID dwDispID;
        HRESULT hr = GetIDOfName(lpsz, &dwDispID);
        if (SUCCEEDED(hr))
            hr = PutProperty(dwDispID, pVar);
        return hr;
    }
    _Check_return_ HRESULT PutProperty(
        _In_ DISPID dwDispID,
        _In_ VARIANT* pVar) throw()
    {
        return PutProperty(this->p, dwDispID, pVar);
    }
    _Check_return_ HRESULT GetIDOfName(
        _In_z_ LPCOLESTR lpsz,
        _Out_ DISPID* pdispid) throw()
    {
        return this->p->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&lpsz), 1, LOCALE_USER_DEFAULT, pdispid);
    }
    // Invoke a method by DISPID with no parameters
    _Check_return_ HRESULT Invoke0(
        _In_ DISPID dispid,
        _Out_opt_ VARIANT* pvarRet = NULL) throw()
    {
        DISPPARAMS dispparams = { NULL, NULL, 0, 0};
        return this->p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
    }
    // Invoke a method by name with no parameters
    _Check_return_ HRESULT Invoke0(
        _In_z_ LPCOLESTR lpszName,
        _Out_opt_ VARIANT* pvarRet = NULL) throw()
    {
        HRESULT hr;
        DISPID dispid;
        hr = GetIDOfName(lpszName, &dispid);
        if (SUCCEEDED(hr))
            hr = Invoke0(dispid, pvarRet);
        return hr;
    }
    // Invoke a method by DISPID with a single parameter
    _Check_return_ HRESULT Invoke1(
        _In_ DISPID dispid,
        _In_ VARIANT* pvarParam1,
        _Out_opt_ VARIANT* pvarRet = NULL) throw()
    {
        DISPPARAMS dispparams = { pvarParam1, NULL, 1, 0};
        return this->p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
    }
    // Invoke a method by name with a single parameter
    _Check_return_ HRESULT Invoke1(
        _In_z_ LPCOLESTR lpszName,
        _In_ VARIANT* pvarParam1,
        _Out_opt_ VARIANT* pvarRet = NULL) throw()
    {
        DISPID dispid;
        HRESULT hr = GetIDOfName(lpszName, &dispid);
        if (SUCCEEDED(hr))
            hr = Invoke1(dispid, pvarParam1, pvarRet);
        return hr;
    }
    // Invoke a method by DISPID with two parameters
    _Check_return_ HRESULT Invoke2(
        _In_ DISPID dispid,
        _In_ VARIANT* pvarParam1,
        _In_ VARIANT* pvarParam2,
        _Out_opt_ VARIANT* pvarRet = NULL) throw();
    // Invoke a method by name with two parameters
    _Check_return_ HRESULT Invoke2(
        _In_z_ LPCOLESTR lpszName,
        _In_ VARIANT* pvarParam1,
        _In_ VARIANT* pvarParam2,
        _Out_opt_ VARIANT* pvarRet = NULL) throw()
    {
        DISPID dispid;
        HRESULT hr = GetIDOfName(lpszName, &dispid);
        if (SUCCEEDED(hr))
            hr = Invoke2(dispid, pvarParam1, pvarParam2, pvarRet);
        return hr;
    }
    // Invoke a method by DISPID with N parameters
    _Check_return_ HRESULT InvokeN(
        _In_ DISPID dispid,
        _In_ VARIANT* pvarParams,
        _In_ int nParams,
        _Out_opt_ VARIANT* pvarRet = NULL) throw()
    {
        DISPPARAMS dispparams = {pvarParams, NULL, (unsigned int)nParams, 0};
        return this->p->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
    }
    // Invoke a method by name with Nparameters
    _Check_return_ HRESULT InvokeN(
        _In_z_ LPCOLESTR lpszName,
        _In_ VARIANT* pvarParams,
        _In_ int nParams,
        _Out_opt_ VARIANT* pvarRet = NULL) throw()
    {
        HRESULT hr;
        DISPID dispid;
        hr = GetIDOfName(lpszName, &dispid);
        if (SUCCEEDED(hr))
            hr = InvokeN(dispid, pvarParams, nParams, pvarRet);
        return hr;
    }

    _Check_return_ static HRESULT PutProperty(
        _In_ IDispatch* pDispatch,
        _In_ DISPID dwDispID,
        _In_ VARIANT* pVar) throw()
    {
        ATLASSERT(pDispatch);
        ATLASSERT(pVar != NULL);
        if (pVar == NULL)
            return E_POINTER;

        if (pDispatch == NULL)
            return E_INVALIDARG;

        DISPPARAMS dispparams = {NULL, NULL, 1, 1};
        dispparams.rgvarg = pVar;
        DISPID dispidPut = DISPID_PROPERTYPUT;
        dispparams.rgdispidNamedArgs = &dispidPut;

        if (pVar->vt == VT_UNKNOWN || pVar->vt == VT_DISPATCH ||
            (pVar->vt & VT_ARRAY) || (pVar->vt & VT_BYREF))
        {
            HRESULT hr = pDispatch->Invoke(dwDispID, IID_NULL,
                LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF,
                &dispparams, NULL, NULL, NULL);
            if (SUCCEEDED(hr))
                return hr;
        }
        return pDispatch->Invoke(dwDispID, IID_NULL,
                LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
                &dispparams, NULL, NULL, NULL);
    }

    _Check_return_ static HRESULT GetProperty(
        _In_ IDispatch* pDispatch,
        _In_ DISPID dwDispID,
        _Out_ VARIANT* pVar) throw()
    {
        ATLASSERT(pDispatch);
        ATLASSERT(pVar != NULL);
        if (pVar == NULL)
            return E_POINTER;

        if (pDispatch == NULL)
            return E_INVALIDARG;

        DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};
        return pDispatch->Invoke(dwDispID, IID_NULL,
                LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,
                &dispparamsNoArgs, pVar, NULL, NULL);
    }
};
// End code borrowed from Microsoft <atlcomcli.h>


//CComQIIDPtr<I_ID(Itype)> is the gcc compatible version of CComQIPtr<Itype>
#define I_ID(Itype) Itype,&IID_##Itype

template <class T, const IID* piid>
class CComQIIDPtr :
    public CComPtr<T>
{
public:
    // Let's tell GCC how to find a symbol.
    using CComPtr<T>::p;

    CComQIIDPtr()
    {
    }
    CComQIIDPtr(_Inout_opt_ T* lp) :
        CComPtr<T>(lp)
    {
    }
    CComQIIDPtr(_Inout_ const CComQIIDPtr<T,piid>& lp):
        CComPtr<T>(lp.p)
    {
    }
    CComQIIDPtr(_Inout_opt_ IUnknown* lp)
    {
        if (lp != NULL)
        {
            if (FAILED(lp->QueryInterface(*piid, (void**)(IUnknown**)&p)))
                p = NULL;
        }
    }
    T *operator = (T *lp)
    {
        T* pOld = p;

        p = lp;
        if (p != NULL)
            p->AddRef();

        if (pOld != NULL)
            pOld->Release();

        return *this;
    }

    T *operator = (const CComQIIDPtr<T,piid> &lp)
    {
        T* pOld = p;

        p = lp.p;
        if (p != NULL)
            p->AddRef();

        if (pOld != NULL)
            pOld->Release();

        return *this;
    }

    T * operator=(IUnknown* lp)
    {
        T* pOld = p;

        if (!lp || FAILED(lp->QueryInterface(*piid, (void**)(IUnknown**)&p)))
            p = NULL;

        if (pOld != NULL)
            pOld->Release();

        return *this;
    }
};


class CComBSTR
{
public:
    BSTR m_str;
public:
    CComBSTR() :
        m_str(NULL)
    {
    }

    CComBSTR(LPCOLESTR pSrc)
    {
        if (pSrc == NULL)
            m_str = NULL;
        else
            m_str = ::SysAllocString(pSrc);
    }

    CComBSTR(int length)
    {
        if (length == 0)
            m_str = NULL;
        else
            m_str = ::SysAllocStringLen(NULL, length);
    }

    CComBSTR(int length, LPCOLESTR pSrc)
    {
        if (length == 0)
            m_str = NULL;
        else
            m_str = ::SysAllocStringLen(pSrc, length);
    }

    CComBSTR(PCSTR pSrc)
    {
        if (pSrc)
        {
            int len = MultiByteToWideChar(CP_THREAD_ACP, 0, pSrc, -1, NULL, 0);
            m_str = ::SysAllocStringLen(NULL, len - 1);
            if (m_str)
            {
                int res = MultiByteToWideChar(CP_THREAD_ACP, 0, pSrc, -1, m_str, len);
                ATLASSERT(res == len);
                if (res != len)
                {
                    ::SysFreeString(m_str);
                    m_str = NULL;
                }
            }
        }
        else
        {
            m_str = NULL;
        }
    }

    CComBSTR(const CComBSTR &other)
    {
        m_str = other.Copy();
    }

    CComBSTR(REFGUID guid)
    {
        OLECHAR szGuid[40];
        ::StringFromGUID2(guid, szGuid, 40);
        m_str = ::SysAllocString(szGuid);
    }

    ~CComBSTR()
    {
        ::SysFreeString(m_str);
        m_str = NULL;
    }

    operator BSTR () const
    {
        return m_str;
    }

    BSTR *operator & ()
    {
        return &m_str;
    }

    CComBSTR &operator = (const CComBSTR &other)
    {
        ::SysFreeString(m_str);
        m_str = other.Copy();
        return *this;
    }

    void Attach(BSTR bstr)
    {
        ::SysFreeString(m_str);
        m_str = bstr;
    }

    BSTR Detach()
    {
        BSTR str = m_str;
        m_str = NULL;
        return str;
    }

    BSTR Copy() const
    {
        if (!m_str)
            return NULL;
        return ::SysAllocStringLen(m_str, ::SysStringLen(m_str));
    }

    HRESULT CopyTo(BSTR *other) const
    {
        if (!other)
            return E_POINTER;
        *other = Copy();
        return S_OK;
    }

    bool LoadString(HMODULE module, DWORD uID)
    {
        ::SysFreeString(m_str);
        m_str = NULL;
        const wchar_t *ptr = NULL;
        int len = ::LoadStringW(module, uID, (PWSTR)&ptr, 0);
        if (len)
            m_str = ::SysAllocStringLen(ptr, len);
        return m_str != NULL;
    }

    unsigned int Length() const
    {
        return ::SysStringLen(m_str);
    }

    unsigned int ByteLength() const
    {
        return ::SysStringByteLen(m_str);
    }
};


class CComVariant : public tagVARIANT
{
public:
    CComVariant()
    {
        ::VariantInit(this);
    }

    CComVariant(const CComVariant& other)
    {
        V_VT(this) = VT_EMPTY;
        Copy(&other);
    }

    ~CComVariant()
    {
        Clear();
    }

    CComVariant(LPCOLESTR lpStr)
    {
        V_VT(this) = VT_BSTR;
        V_BSTR(this) = ::SysAllocString(lpStr);
    }

    CComVariant(LPCSTR lpStr)
    {
        V_VT(this) = VT_BSTR;
        CComBSTR str(lpStr);
        V_BSTR(this) = str.Detach();
    }

    CComVariant(bool value)
    {
        V_VT(this) = VT_BOOL;
        V_BOOL(this) = value ? VARIANT_TRUE : VARIANT_FALSE;
    }

    CComVariant(char value)
    {
        V_VT(this) = VT_I1;
        V_I1(this) = value;
    }

    CComVariant(BYTE value)
    {
        V_VT(this) = VT_UI1;
        V_UI1(this) = value;
    }

    CComVariant(short value)
    {
        V_VT(this) = VT_I2;
        V_I2(this) = value;
    }

    CComVariant(unsigned short value)
    {
        V_VT(this) = VT_UI2;
        V_UI2(this) = value;
    }

    CComVariant(int value, VARENUM type = VT_I4)
    {
        if (type == VT_I4 || type == VT_INT)
        {
            V_VT(this) = type;
            V_I4(this) = value;
        }
        else
        {
            V_VT(this) = VT_ERROR;
            V_ERROR(this) = E_INVALIDARG;
        }
    }

    CComVariant(unsigned int value, VARENUM type = VT_UI4)
    {
        if (type == VT_UI4 || type == VT_UINT)
        {
            V_VT(this) = type;
            V_UI4(this) = value;
        }
        else
        {
            V_VT(this) = VT_ERROR;
            V_ERROR(this) = E_INVALIDARG;
        }
    }

    CComVariant(long value, VARENUM type = VT_I4)
    {
        if (type == VT_I4 || type == VT_ERROR)
        {
            V_VT(this) = type;
            V_I4(this) = value;
        }
        else
        {
            V_VT(this) = VT_ERROR;
            V_ERROR(this) = E_INVALIDARG;
        }
    }

    CComVariant(unsigned long value)
    {
        V_VT(this) = VT_UI4;
        V_UI4(this) = value;
    }

    CComVariant(float value)
    {
        V_VT(this) = VT_R4;
        V_R4(this) = value;
    }

    CComVariant(double value, VARENUM type = VT_R8)
    {
        if (type == VT_R8 || type == VT_DATE)
        {
            V_VT(this) = type;
            V_R8(this) = value;
        }
        else
        {
            V_VT(this) = VT_ERROR;
            V_ERROR(this) = E_INVALIDARG;
        }
    }

    CComVariant(const LONGLONG& value)
    {
        V_VT(this) = VT_I8;
        V_I8(this) = value;
    }

    CComVariant(const ULONGLONG& value)
    {
        V_VT(this) = VT_UI8;
        V_UI8(this) = value;
    }

    CComVariant(const CY& value)
    {
        V_VT(this) = VT_CY;
        V_I8(this) = value.int64;
    }


    HRESULT Clear()
    {
        return ::VariantClear(this);
    }

    HRESULT Copy(_In_ const VARIANT* src)
    {
        return ::VariantCopy(this, const_cast<VARIANT*>(src));
    }

    HRESULT ChangeType(_In_ VARTYPE newType, _In_opt_ const LPVARIANT src = NULL)
    {
        const LPVARIANT lpSrc = src ? src : this;
        return ::VariantChangeType(this, lpSrc, 0, newType);
    }
};



}; // namespace ATL

#ifndef _ATL_NO_AUTOMATIC_NAMESPACE
using namespace ATL;
#endif //!_ATL_NO_AUTOMATIC_NAMESPACE
