// ProgressBarControl.cpp : Implementation of CProgressBarControl
#include "stdafx.h"
#include "ProgressBarControl.h"
#include <CommCtrl.h>
#include "Registry.h"
#include "VersionInfo.h"

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef PBS_MARQUEE
#define PBS_MARQUEE 0x08
#endif

#ifndef PBM_SETMARQUEE
#define PBM_SETMARQUEE (WM_USER + 10)
#endif

STDMETHODIMP CProgressBarControl::UpdateRegistry(BOOL bRegister) {
	if (bRegister) {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar", NULL, REG_SZ, L"Legacy Update Progress Bar Control"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar\\CurVer", NULL, REG_SZ, L"LegacyUpdate.ProgressBar.1"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar.1", NULL, REG_SZ, L"Legacy Update Progress Bar Control"},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar.1\\CLSID", NULL, REG_SZ, L"%CLSID_ProgressBarControl%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%", NULL, REG_SZ, L"Legacy Update Progress Bar Control"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%", L"AppID", REG_SZ, L"%APPID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\ProgID", NULL, REG_SZ, L"LegacyUpdate.ProgressBar.1"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\VersionIndependentProgID", NULL, REG_SZ, L"LegacyUpdate.ProgressBar"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\Programmable", NULL, REG_SZ, L""},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\InprocServer32", NULL, REG_SZ, L"%MODULE%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\InprocServer32", L"ThreadingModel", REG_SZ, L"Apartment"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\Control", NULL, REG_SZ, L""},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\TypeLib", NULL, REG_SZ, L"%LIBID%"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\Version", NULL, REG_SZ, L"1.0"},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\MiscStatus", NULL, REG_DWORD, (LPVOID)_GetMiscStatus()},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\Implemented Categories\\{7DD95801-9882-11CF-9FA9-00AA006C42C4}", NULL, REG_SZ, L""},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%\\Implemented Categories\\{7DD95802-9882-11CF-9FA9-00AA006C42C4}", NULL, REG_SZ, L""},
			{}
		};
		return SetRegistryEntries(entries, TRUE);
	} else {
		RegistryEntry entries[] = {
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar", NULL, REG_SZ, DELETE_THIS},
			{HKEY_CLASSES_ROOT, L"LegacyUpdate.ProgressBar.1", NULL, REG_SZ, DELETE_THIS},
			{HKEY_CLASSES_ROOT, L"CLSID\\%CLSID_ProgressBarControl%", NULL, REG_SZ, DELETE_THIS},
			{}
		};
		return SetRegistryEntries(entries, TRUE);
	}
}

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
