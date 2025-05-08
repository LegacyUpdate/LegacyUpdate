#pragma once

#include <windows.h>
#include "stdafx.h"

#if _WIN64
	#define OurGetNativeSystemInfo(lpSystemInfo) GetNativeSystemInfo(lpSystemInfo)
	#define DisableWow64FsRedirection(OldValue)  TRUE
	#define RevertWow64FsRedirection(OldValue)   TRUE
#else
	void OurGetNativeSystemInfo(LPSYSTEM_INFO lpSystemInfo);
	BOOL DisableWow64FsRedirection(PVOID *OldValue);
	BOOL RevertWow64FsRedirection(PVOID OldValue);
#endif
