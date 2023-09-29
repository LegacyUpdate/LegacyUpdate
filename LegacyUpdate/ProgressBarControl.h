#pragma once

// ProgressBarControl.h : Declaration of the CProgressBarControl class.

#include <atlctl.h>
#include "resource.h"
#include "LegacyUpdate_i.h"

// CProgressBarControl
class ATL_NO_VTABLE CProgressBarControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IProgressBarControl, &IID_IProgressBarControl, &LIBID_LegacyUpdateLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleControlImpl<CProgressBarControl>,
	public IOleObjectImpl<CProgressBarControl>,
	public IOleInPlaceActiveObjectImpl<CProgressBarControl>,
	public IViewObjectExImpl<CProgressBarControl>,
	public IOleInPlaceObjectWindowlessImpl<CProgressBarControl>,
	public CComCoClass<CProgressBarControl, &CLSID_ProgressBarControl>,
	public CComControl<CProgressBarControl>
{
public:
	CContainedWindow m_ctl;

	CProgressBarControl() : m_ctl(L"msctls_progress32", this, 1) {
		m_bWindowOnly = TRUE;
	}

	DECLARE_OLEMISC_STATUS(
		OLEMISC_RECOMPOSEONRESIZE |
		OLEMISC_CANTLINKINSIDE |
		OLEMISC_INSIDEOUT |
		OLEMISC_ACTIVATEWHENVISIBLE |
		OLEMISC_SETCLIENTSITEFIRST
	)

	DECLARE_REGISTRY_RESOURCEID(IDR_PROGRESSBARCONTROL)

	BEGIN_COM_MAP(CProgressBarControl)
		COM_INTERFACE_ENTRY(IProgressBarControl)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IViewObjectEx)
		COM_INTERFACE_ENTRY(IViewObject2)
		COM_INTERFACE_ENTRY(IViewObject)
		COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceObject)
		COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
		COM_INTERFACE_ENTRY(IOleControl)
		COM_INTERFACE_ENTRY(IOleObject)
	END_COM_MAP()

	BEGIN_PROP_MAP(CProgressBarControl)
	END_PROP_MAP()

	BEGIN_MSG_MAP(CProgressBarControl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		CHAIN_MSG_MAP(CComControl<CProgressBarControl>)
		ALT_MSG_MAP(1)
	END_MSG_MAP()

	// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

	// IProgressBarControl
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct() { return S_OK; }
	void FinalRelease() {}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	STDMETHODIMP SetObjectRects(LPCRECT prcPos, LPCRECT prcClip);

	STDMETHODIMP get_Value(SHORT *pValue);
	STDMETHODIMP put_Value(SHORT value);
};

OBJECT_ENTRY_AUTO(__uuidof(ProgressBarControl), CProgressBarControl)
