#include <windows.h>
#include "Exec.h"
#include "VersionInfo.h"
#include "Wow64.h"

void LaunchOptions(int nCmdShow) {
#if !_WIN64
	// Some issues arise from the working directory being SysWOW64 rather than System32. Notably,
	// Windows Vista - 8.1 don't have wuauclt.exe in SysWOW64. Disable WOW64 redirection temporarily
	// to work around this.
	PVOID oldValue;
	BOOL isRedirected = DisableWow64FsRedirection(&oldValue);
#endif

	HRESULT hr;

	if (AtLeastWin10()) {
		// Windows 10+: Open Settings app
		hr = Exec(NULL, L"ms-settings:windowsupdate-options", NULL, NULL, SW_SHOWDEFAULT, FALSE, NULL);
	} else if (AtLeastWinVista()) {
		// Windows Vista, 7, 8: Open Windows Update control panel
		WCHAR wuauclt[MAX_PATH];
		ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\wuauclt.exe", wuauclt, ARRAYSIZE(wuauclt));
		hr = Exec(NULL, wuauclt, L"/ShowOptions", NULL, SW_SHOWDEFAULT, FALSE, NULL);
	} else {
		// Windows 2000, XP: Open Automatic Updates control panel
		WCHAR wuaucpl[MAX_PATH];
		ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\wuaucpl.cpl", wuaucpl, ARRAYSIZE(wuaucpl));
		hr = Exec(NULL, wuaucpl, NULL, NULL, SW_SHOWDEFAULT, FALSE, NULL);
	}

#if !_WIN64
	// Revert WOW64 redirection if we changed it
	if (isRedirected) {
		RevertWow64FsRedirection(oldValue);
	}
#endif

	PostQuitMessage(hr);
}
