#pragma once

// LegacyUpdateCtrl.h : Declaration of the CLegacyUpdateCtrl ActiveX Control class.

// CLegacyUpdateCtrl : See LegacyUpdateCtrl.cpp for implementation.

#include <mshtml.h>
#include <wuapi.h>
#include "resource.h"
#include "com.h"
#include "LegacyUpdate_i.h"

class CLegacyUpdateCtrl;

class DECLSPEC_NOVTABLE CLegacyUpdateCtrl_IOleObject :
	public IOleObjectImpl<CLegacyUpdateCtrl> {
public:
	CLegacyUpdateCtrl_IOleObject(CLegacyUpdateCtrl *pParent) :
		IOleObjectImpl<CLegacyUpdateCtrl>(pParent) {}
};

class DECLSPEC_NOVTABLE CLegacyUpdateCtrl :
	public IDispatchImpl<ILegacyUpdateCtrl, &LIBID_LegacyUpdateLib> {
public:
	CLegacyUpdateCtrl() :
		m_IOleObject(this),
		m_refCount(1),
		m_clientSite(NULL),
		m_adviseSink(NULL),
		m_elevatedHelper(NULL),
		m_nonElevatedHelper(NULL) {}

	virtual ~CLegacyUpdateCtrl();

	static STDMETHODIMP Create(IUnknown *pUnkOuter, REFIID riid, void **ppv);
	static STDMETHODIMP UpdateRegistry(BOOL bRegister);

private:
	CLegacyUpdateCtrl_IOleObject m_IOleObject;
	LONG m_refCount;

public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	IOleClientSite *m_clientSite;
	IAdviseSink *m_adviseSink;

private:
	IElevationHelper *m_elevatedHelper;
	IElevationHelper *m_nonElevatedHelper;

	STDMETHODIMP GetHTMLDocument(IHTMLDocument2 **retval);
	STDMETHODIMP IsPermitted();
	STDMETHODIMP GetIEWindowHWND(HWND *retval);
	STDMETHODIMP GetElevatedHelper(IElevationHelper **retval);

public:
	// ILegacyUpdateCtrl
	STDMETHODIMP CheckControl(VARIANT_BOOL *retval);
	STDMETHODIMP MessageForHresult(LONG inHresult, BSTR *retval);
	STDMETHODIMP GetOSVersionInfo(OSVersionField osField, LONG systemMetric, VARIANT *retval);
	STDMETHODIMP RequestElevation();
	STDMETHODIMP CreateObject(BSTR progID, IDispatch **retval);
	STDMETHODIMP SetBrowserHwnd(IUpdateInstaller *installer);
	STDMETHODIMP GetUserType(UserType *retval);
	STDMETHODIMP get_IsRebootRequired(VARIANT_BOOL *retval);
	STDMETHODIMP get_IsWindowsUpdateDisabled(VARIANT_BOOL *retval);
	STDMETHODIMP RebootIfRequired();
	STDMETHODIMP ViewWindowsUpdateLog();
	STDMETHODIMP OpenWindowsUpdateSettings();
	STDMETHODIMP get_IsUsingWsusServer(VARIANT_BOOL *retval);
	STDMETHODIMP get_WsusServerUrl(BSTR *retval);
	STDMETHODIMP get_WsusStatusServerUrl(BSTR *retval);
	STDMETHODIMP BeforeUpdate();
	STDMETHODIMP AfterUpdate();
};
