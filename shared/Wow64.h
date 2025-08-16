#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#if _WIN64
	#define OurGetNativeSystemInfo(lpSystemInfo) GetNativeSystemInfo(lpSystemInfo)
	#define DisableWow64FsRedirection(OldValue)  TRUE
	#define RevertWow64FsRedirection(OldValue)   TRUE
#else
	void OurGetNativeSystemInfo(LPSYSTEM_INFO lpSystemInfo);
	BOOL DisableWow64FsRedirection(PVOID *OldValue);
	BOOL RevertWow64FsRedirection(PVOID OldValue);
#endif

#ifdef __cplusplus
}
#endif
