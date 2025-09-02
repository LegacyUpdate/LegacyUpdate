// ProgressBarControl.cpp : Implementation of CProgressBarControl

#include "ProgressBarControl.h"
#include <commctrl.h>
#include <new>
#include "Compat.h"
#include "Registry.h"
#include "resource.h"
#include "VersionInfo.h"

#define PROGRESSBARCONTROL_MISCSTATUS (OLEMISC_RECOMPOSEONRESIZE | OLEMISC_CANTLINKINSIDE | OLEMISC_INSIDEOUT | OLEMISC_ACTIVATEWHENVISIBLE | OLEMISC_SETCLIENTSITEFIRST)

DEFINE_UUIDOF(CProgressBarControl, CLSID_ProgressBarControl);

STDMETHODIMP CProgressBarControl::Create(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
	if (pUnkOuter != NULL) {
		return CLASS_E_NOAGGREGATION;
	}

	CProgressBarControl *pThis = (CProgressBarControl *)CoTaskMemAlloc(sizeof(CProgressBarControl));
	if (pThis == NULL) {
		return E_OUTOFMEMORY;
	}

	new(pThis) CProgressBarControl();
	HRESULT hr = pThis->QueryInterface(riid, ppv);
	CHECK_HR_OR_RETURN(L"QueryInterface");
	pThis->Release();

	return hr;
}

CProgressBarControl::~CProgressBarControl() {
	DestroyControlWindow();
	if (m_clientSite) {
		m_clientSite->Release();
		m_clientSite = NULL;
	}
	if (m_adviseSink) {
		m_adviseSink->Release();
		m_adviseSink = NULL;
	}
}

STDMETHODIMP CProgressBarControl::UpdateRegistry(BOOL bRegister) {
	if (bRegister) {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar", NULL, REG_SZ, (LPVOID)L"Legacy Update Progress Bar Control"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar\\CurVer", NULL, REG_SZ, (LPVOID)L"LegacyUpdate.ProgressBar.1"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar.1", NULL, REG_SZ, (LPVOID)L"Legacy Update Progress Bar Control"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar.1\\CLSID", NULL, REG_SZ, (LPVOID)L"%CLSID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%", NULL, REG_SZ, (LPVOID)L"Legacy Update Progress Bar Control"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%", L"AppID", REG_SZ, (LPVOID)L"%APPID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\ProgID", NULL, REG_SZ, (LPVOID)L"LegacyUpdate.ProgressBar.1"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\VersionIndependentProgID", NULL, REG_SZ, (LPVOID)L"LegacyUpdate.ProgressBar"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Programmable", NULL, REG_SZ, NULL},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\InprocServer32", NULL, REG_SZ, (LPVOID)L"%MODULE%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\InprocServer32", L"ThreadingModel", REG_SZ, (LPVOID)L"Apartment"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Control", NULL, REG_SZ, NULL},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\TypeLib", NULL, REG_SZ, (LPVOID)L"%LIBID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Version", NULL, REG_SZ, (LPVOID)L"1.0"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\MiscStatus", NULL, REG_DWORD, (LPVOID)PROGRESSBARCONTROL_MISCSTATUS},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Implemented Categories\\{7DD95801-9882-11CF-9FA9-00AA006C42C4}", NULL, REG_SZ, NULL},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%\\Implemented Categories\\{7DD95802-9882-11CF-9FA9-00AA006C42C4}", NULL, REG_SZ, NULL},
			{}
		};
		return SetRegistryEntries(entries);
	} else {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar", NULL, 0, DELETE_KEY},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar.1", NULL, 0, DELETE_KEY},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID%", NULL, 0, DELETE_KEY},
			{}
		};
		return SetRegistryEntries(entries);
	}
}

#pragma mark - IUnknown

STDMETHODIMP CProgressBarControl::QueryInterface(REFIID riid, void **ppvObject) {
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IOleObject)) {
		*ppvObject = (IOleObject *)&m_IOleObject;
		AddRef();
		return S_OK;
	} else if (IsEqualIID(riid, IID_IViewObject) || IsEqualIID(riid, IID_IViewObject2) || IsEqualIID(riid, IID_IViewObjectEx)) {
		*ppvObject = (IViewObjectEx *)&m_IViewObjectEx;
		AddRef();
		return S_OK;
	} else if (IsEqualIID(riid, IID_IOleInPlaceObject) || IsEqualIID(riid, IID_IOleWindow) || IsEqualIID(riid, IID_IOleInPlaceObjectWindowless)) {
		*ppvObject = (IOleInPlaceObject *)&m_IOleInPlaceObject;
		AddRef();
		return S_OK;
	} else if (IsEqualIID(riid, IID_IOleInPlaceActiveObject)) {
		*ppvObject = (IOleInPlaceActiveObject *)&m_IOleInPlaceActiveObject;
		AddRef();
		return S_OK;
	}

	return IDispatchImpl<IProgressBarControl, &LIBID_LegacyUpdateLib>::QueryInterface(riid, ppvObject);
}

STDMETHODIMP_(ULONG) CProgressBarControl::AddRef() {
	return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) CProgressBarControl::Release() {
	ULONG count = InterlockedDecrement(&m_refCount);
	if (count == 0) {
		this->~CProgressBarControl();
		CoTaskMemFree(this);
	}
	return count;
}

#pragma mark - IOleObject

STDMETHODIMP CProgressBarControl_IOleObject::GetExtent(DWORD dwDrawAspect, SIZEL *psizel) {
	if (psizel == NULL) {
		return E_POINTER;
	}

	if (dwDrawAspect != DVASPECT_CONTENT) {
		return DV_E_DVASPECT;
	}

	// Convert to HIMETRIC
	HDC hdc = GetDC(NULL);
	int ppiX = GetDeviceCaps(hdc, LOGPIXELSX);
	int ppiY = GetDeviceCaps(hdc, LOGPIXELSY);
	ReleaseDC(NULL, hdc);

	psizel->cx = MAP_PIX_TO_LOGHIM(m_pParent->m_width, ppiX);
	psizel->cy = MAP_PIX_TO_LOGHIM(m_pParent->m_height, ppiY);
	return S_OK;
}

STDMETHODIMP CProgressBarControl_IOleObject::SetExtent(DWORD dwDrawAspect, SIZEL *psizel) {
	if (psizel == NULL) {
		return E_POINTER;
	}

	if (dwDrawAspect != DVASPECT_CONTENT) {
		return DV_E_DVASPECT;
	}

	// Convert from HIMETRIC
	HDC hdc = GetDC(NULL);
	int ppiX = GetDeviceCaps(hdc, LOGPIXELSX);
	int ppiY = GetDeviceCaps(hdc, LOGPIXELSY);
	ReleaseDC(NULL, hdc);

	m_pParent->m_width = MAP_LOGHIM_TO_PIX(psizel->cx, ppiX);
	m_pParent->m_height = MAP_LOGHIM_TO_PIX(psizel->cy, ppiY);

	if (m_pParent->m_innerHwnd) {
		SetWindowPos(
			m_pParent->m_innerHwnd, NULL,
			0, 0,
			m_pParent->m_width, m_pParent->m_height,
			SWP_NOZORDER | SWP_NOACTIVATE
		);
	}

	return S_OK;
}

STDMETHODIMP CProgressBarControl_IOleObject::DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect) {
	switch (iVerb) {
	case OLEIVERB_INPLACEACTIVATE:
	case OLEIVERB_UIACTIVATE:
	case OLEIVERB_SHOW:
		if (hwndParent && lprcPosRect) {
			HRESULT hr = m_pParent->CreateControlWindow(hwndParent, lprcPosRect);
			CHECK_HR_OR_RETURN(L"CreateControlWindow");
			ShowWindow(m_pParent->m_innerHwnd, SW_SHOW);
		}
		return S_OK;

	case OLEIVERB_HIDE:
		if (m_pParent->m_innerHwnd) {
			ShowWindow(m_pParent->m_innerHwnd, SW_HIDE);
		}
		return S_OK;

	default:
		return OLEOBJ_S_INVALIDVERB;
	}
}

STDMETHODIMP CProgressBarControl_IOleObject::GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus) {
	if (pdwStatus == NULL) {
		return E_POINTER;
	}

	if (dwAspect == DVASPECT_CONTENT) {
		*pdwStatus = PROGRESSBARCONTROL_MISCSTATUS;
		return S_OK;
	}

	*pdwStatus = 0;
	return S_OK;
}

STDMETHODIMP CProgressBarControl_IOleObject::Close(DWORD dwSaveOption) {
	m_pParent->DestroyControlWindow();
	return IOleObjectImpl<CProgressBarControl>::Close(dwSaveOption);
}

STDMETHODIMP CProgressBarControl::CreateControlWindow(HWND hParent, const RECT *pRect) {
	m_width = pRect->right - pRect->left;
	m_height = pRect->bottom - pRect->top;

	if (m_innerHwnd) {
		if (m_hwnd == hParent) {
			// Just update the position
			SetWindowPos(
				m_innerHwnd, NULL,
				pRect->left, pRect->top,
				m_width, m_height,
				SWP_NOZORDER | SWP_NOACTIVATE
			);
			return S_OK;
		} else {
			// Parent changed, start over
			DestroyControlWindow();
		}
	}

	m_hwnd = hParent;

	// Bind to our manifest to ensure we get visual styles and marquee
	ULONG_PTR cookie;
	IsolationAwareStart(&cookie);

	// Load and init comctl (hopefully 6.0)
	LoadLibrary(L"comctl32.dll");

	INITCOMMONCONTROLSEX initComctl = {0};
	initComctl.dwSize = sizeof(initComctl);
	initComctl.dwICC = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&initComctl);

	m_innerHwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		PROGRESS_CLASS,
		NULL,
		WS_CHILD | WS_VISIBLE,
		pRect->left, pRect->top,
		m_width, m_height,
		m_hwnd,
		NULL,
		OWN_MODULE,
		NULL
	);
	IsolationAwareEnd(&cookie);
	if (!m_innerHwnd) {
		return E_FAIL;
	}

	return put_Value(-1);
}

STDMETHODIMP CProgressBarControl::DestroyControlWindow() {
	if (m_innerHwnd) {
		DestroyWindow(m_innerHwnd);
		m_innerHwnd = NULL;
	}
	m_hwnd = NULL;

	return S_OK;
}

#pragma mark - IViewObjectEx

STDMETHODIMP CProgressBarControl::OnDraw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd, HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds, BOOL (STDMETHODCALLTYPE *pfnContinue)(ULONG_PTR dwContinue), ULONG_PTR dwContinue) {
	if (lprcBounds == NULL) {
		return E_POINTER;
	}

	switch (dwDrawAspect) {
	case DVASPECT_CONTENT:
	case DVASPECT_OPAQUE:
	case DVASPECT_TRANSPARENT:
		break;

	default:
		return DV_E_DVASPECT;
	}

	if (m_innerHwnd) {
		SetWindowPos(
			m_innerHwnd, NULL,
			lprcBounds->left, lprcBounds->top,
			lprcBounds->right - lprcBounds->left,
			lprcBounds->bottom - lprcBounds->top,
			SWP_NOZORDER | SWP_NOACTIVATE
		);
	}

	return S_OK;
}

#pragma mark - IOleInPlaceObject

STDMETHODIMP CProgressBarControl_IOleInPlaceObject::SetObjectRects(LPCRECT lprcPosRect, LPCRECT lprcClipRect) {
	if (lprcPosRect == NULL) {
		return E_POINTER;
	}

	if (m_pParent->m_innerHwnd) {
		SetWindowPos(
			m_pParent->m_innerHwnd, NULL,
			lprcPosRect->left, lprcPosRect->top,
			lprcPosRect->right - lprcPosRect->left,
			lprcPosRect->bottom - lprcPosRect->top,
			SWP_NOZORDER | SWP_NOACTIVATE
		);
	}

	return S_OK;
}

#pragma mark - IOleInPlaceActiveObject

STDMETHODIMP CProgressBarControl_IOleInPlaceActiveObject::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fFrameWindow) {
	if (prcBorder && m_pParent->m_innerHwnd) {
		LONG width = prcBorder->right - prcBorder->left;
		LONG height = prcBorder->bottom - prcBorder->top;

		m_pParent->m_width = width;
		m_pParent->m_height = height;

		SetWindowPos(
			m_pParent->m_innerHwnd, NULL,
			prcBorder->left, prcBorder->top,
			width, height,
			SWP_NOZORDER | SWP_NOACTIVATE
		);
	}

	return S_OK;
}

#pragma mark - IProgressBarControl

STDMETHODIMP CProgressBarControl::get_Value(SHORT *pValue) {
	if (pValue == NULL) {
		return E_POINTER;
	}

	if (m_innerHwnd == NULL) {
		*pValue = 0;
		return S_OK;
	}

	if (GetWindowLongPtr(m_innerHwnd, GWL_STYLE) & PBS_MARQUEE) {
		// Marquee - no value
		*pValue = -1;
	} else {
		// Normal - return PBM_GETPOS
		*pValue = (SHORT)SendMessage(m_innerHwnd, PBM_GETPOS, 0, 0);
	}

	return S_OK;
}

STDMETHODIMP CProgressBarControl::put_Value(SHORT value) {
	if (m_innerHwnd == NULL) {
		return E_FAIL;
	}

	ULONG_PTR cookie;
	IsolationAwareStart(&cookie);

	if (value == -1) {
		// Marquee style
		SetWindowLongPtr(m_innerHwnd, GWL_STYLE, GetWindowLongPtr(m_innerHwnd, GWL_STYLE) | PBS_MARQUEE);
		SendMessage(m_innerHwnd, PBM_SETMARQUEE, TRUE, 100);
	} else {
		// Normal style
		SHORT oldValue = -1;
		get_Value(&oldValue);
		if (oldValue == -1) {
			SendMessage(m_innerHwnd, PBM_SETMARQUEE, FALSE, 0);
			SetWindowLongPtr(m_innerHwnd, GWL_STYLE, GetWindowLongPtr(m_innerHwnd, GWL_STYLE) & ~PBS_MARQUEE);
			SendMessage(m_innerHwnd, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		}
		SendMessage(m_innerHwnd, PBM_SETPOS, value, 0);
	}

	IsolationAwareEnd(&cookie);

	return S_OK;
}
