// ProgressBarControl.cpp : Implementation of CProgressBarControl
#include "stdafx.h"
#include "ProgressBarControl.h"
#include <CommCtrl.h>

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef PBS_MARQUEE
#define PBS_MARQUEE 0x08
#endif

#ifndef PBM_SETMARQUEE
#define PBM_SETMARQUEE (WM_USER + 10)
#endif

LRESULT CProgressBarControl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) {
	RECT rc;
	GetWindowRect(&rc);
	rc.right -= rc.left;
	rc.bottom -= rc.top;
	rc.left = 0;
	rc.top = 0;
	m_ctl.Create(m_hWnd, rc, PROGRESS_CLASS, WS_CHILD | WS_VISIBLE, WS_EX_CLIENTEDGE);
	put_Value(-1);
	return 0;
}

STDMETHODIMP CProgressBarControl::SetObjectRects(LPCRECT prcPos, LPCRECT prcClip) {
	IOleInPlaceObjectWindowlessImpl<CProgressBarControl>::SetObjectRects(prcPos, prcClip);
	::SetWindowPos(m_ctl.m_hWnd, NULL, 0, 0,
		prcPos->right - prcPos->left,
		prcPos->bottom - prcPos->top,
		SWP_NOZORDER | SWP_NOACTIVATE);
	return S_OK;
}

STDMETHODIMP CProgressBarControl::get_Value(SHORT *pValue) {
	if (m_ctl.GetWindowLongPtr(GWL_STYLE) & PBS_MARQUEE) {
		// Marquee - no value
		*pValue = -1;
	} else {
		// Normal - return PBM_GETPOS
		*pValue = (SHORT)m_ctl.SendMessage(PBM_GETPOS, 0, 0);
	}
	return S_OK;
}

STDMETHODIMP CProgressBarControl::put_Value(SHORT value) {
	if (value == -1) {
		// Marquee style
		m_ctl.SetWindowLongPtr(GWL_STYLE, m_ctl.GetWindowLongPtr(GWL_STYLE) | PBS_MARQUEE);
		m_ctl.SendMessage(PBM_SETMARQUEE, TRUE, 100);
	} else {
		// Normal style
		SHORT oldValue;
		get_Value(&oldValue);
		if (oldValue == -1) {
			m_ctl.SetWindowLongPtr(GWL_STYLE, m_ctl.GetWindowLongPtr(GWL_STYLE) & ~PBS_MARQUEE);
			m_ctl.SendMessage(PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		}

		m_ctl.SendMessage(PBM_SETPOS, value, 0);
	}
	return S_OK;
}
