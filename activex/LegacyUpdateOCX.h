#pragma once

// LegacyUpdateOCX.h : main header file for LegacyUpdateOCX.DLL

#if !defined( __AFXCTL_H__ )
#error "include 'afxctl.h' before including this file"
#endif

#include "resource.h"       // main symbols

// CLegacyUpdateOCXApp : See LegacyUpdateOCX.cpp for implementation.

class CLegacyUpdateOCXApp : public COleControlModule {
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;
