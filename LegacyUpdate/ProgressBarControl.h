#pragma once

// ProgressBarControl.h : Declaration of the CProgressBarControl class.

#include "resource.h"
#include "com.h"
#include "LegacyUpdate_i.h"

STDMETHODIMP CreateProgressBarControl(IUnknown *pUnkOuter, REFIID riid, void **ppv);

class CProgressBarControl;

class DECLSPEC_NOVTABLE CProgressBarControl_IOleObject :
	public IOleObjectImpl<CProgressBarControl> {
public:
	CProgressBarControl_IOleObject(CProgressBarControl *pParent) :
		IOleObjectImpl<CProgressBarControl>(pParent) {}

	STDMETHODIMP GetExtent(DWORD dwDrawAspect, SIZEL *psizel);
	STDMETHODIMP SetExtent(DWORD dwDrawAspect, SIZEL *psizel);
	STDMETHODIMP DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect);
	STDMETHODIMP SetClientSite(IOleClientSite *pClientSite);
	STDMETHODIMP GetClientSite(IOleClientSite **ppClientSite);
	STDMETHODIMP GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus);
	STDMETHODIMP Close(DWORD dwSaveOption);
};

class DECLSPEC_NOVTABLE CProgressBarControl_IViewObjectEx :
	public IViewObjectExImpl<CProgressBarControl> {
public:
	CProgressBarControl_IViewObjectEx(CProgressBarControl *pParent) :
		IViewObjectExImpl<CProgressBarControl>(pParent) {}
};

class DECLSPEC_NOVTABLE CProgressBarControl_IOleInPlaceObject :
	public IOleInPlaceObjectImpl<CProgressBarControl> {
public:
	CProgressBarControl_IOleInPlaceObject(CProgressBarControl *pParent) :
		IOleInPlaceObjectImpl<CProgressBarControl>(pParent) {}

	STDMETHODIMP SetObjectRects(LPCRECT lprcPosRect, LPCRECT lprcClipRect);
};

class DECLSPEC_NOVTABLE CProgressBarControl_IOleInPlaceActiveObject :
	public IOleInPlaceActiveObjectImpl<CProgressBarControl> {
public:
	CProgressBarControl_IOleInPlaceActiveObject(CProgressBarControl *pParent) :
		IOleInPlaceActiveObjectImpl<CProgressBarControl>(pParent) {}

	// IOleInPlaceActiveObject
	STDMETHODIMP ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fFrameWindow);
};

class DECLSPEC_NOVTABLE CProgressBarControl_IOleControl :
	public IOleControlImpl<CProgressBarControl> {
public:
	CProgressBarControl_IOleControl(CProgressBarControl *pParent) :
		IOleControlImpl<CProgressBarControl>(pParent) {}
};

class DECLSPEC_NOVTABLE CProgressBarControl :
	public IDispatchImpl<IProgressBarControl, &LIBID_LegacyUpdateLib> {
public:
	CProgressBarControl() :
		m_IOleObject(this),
		m_IViewObjectEx(this),
		m_IOleInPlaceObject(this),
		m_IOleInPlaceActiveObject(this),
		m_IOleControl(this),
		m_refCount(1),
		m_hwnd(NULL),
		m_innerHwnd(NULL),
		m_width(0),
		m_height(0),
		m_clientSite(NULL) {
	}

	virtual ~CProgressBarControl();

	static STDMETHODIMP UpdateRegistry(BOOL bRegister);

public:
	CProgressBarControl_IOleObject m_IOleObject;
	CProgressBarControl_IViewObjectEx m_IViewObjectEx;
	CProgressBarControl_IOleInPlaceObject m_IOleInPlaceObject;
	CProgressBarControl_IOleInPlaceActiveObject m_IOleInPlaceActiveObject;
	CProgressBarControl_IOleControl m_IOleControl;

private:
	LONG m_refCount;

public:
	HWND m_hwnd;
	HWND m_innerHwnd;
	LONG m_width;
	LONG m_height;
	IOleClientSite *m_clientSite;

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IProgressBarControl
	STDMETHODIMP get_Value(SHORT *pValue);
	STDMETHODIMP put_Value(SHORT value);

	// Helpers
	STDMETHODIMP CreateControlWindow(HWND hParent, const RECT *pRect);
	STDMETHODIMP DestroyControlWindow();
	STDMETHODIMP OnDraw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd, HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds, BOOL (STDMETHODCALLTYPE *pfnContinue)(ULONG_PTR dwContinue), ULONG_PTR dwContinue);
};
