#pragma once

#include <windows.h>

#if _WIN64
	#define OurGetNativeSystemInfo(lpSystemInfo) GetNativeSystemInfo(lpSystemInfo)
	#define DisableWow64FsRedirection(OldValue)  TRUE
	#define RevertWow64FsRedirection(OldValue)   TRUE
#else
	EXTERN_C void OurGetNativeSystemInfo(LPSYSTEM_INFO lpSystemInfo);
	EXTERN_C BOOL DisableWow64FsRedirection(PVOID *OldValue);
	EXTERN_C BOOL RevertWow64FsRedirection(PVOID OldValue);
#endif
