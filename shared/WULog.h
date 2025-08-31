#pragma once

#include "stdafx.h"
#include <windows.h>
#include <shlobj.h>
#include "Exec.h"
#include "HResult.h"
#include "VersionInfo.h"

static inline HRESULT ViewWindowsUpdateLog(int nCmdShow) {
	WCHAR windir[MAX_PATH];
	HRESULT hr = SHGetFolderPath(0, CSIDL_WINDOWS, NULL, 0, windir);
	CHECK_HR_OR_RETURN(L"SHGetFolderPath");

	LPWSTR workDir = windir;

	if (AtLeastWin10()) {
		// Windows 10 moves WU/USO logs to ETW. The ETW logs can be converted back to a plain-text .log using a cmdlet.
		WCHAR powershell[MAX_PATH];
		ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\WindowsPowerShell\\v1.0\\powershell.exe", powershell, ARRAYSIZE(powershell));

		DWORD code = 0;
		HRESULT hr = Exec(NULL, powershell, L"-NoProfile -Command Get-WindowsUpdateLog", windir, nCmdShow, TRUE, &code);
		CHECK_HR_OR_RETURN(L"Exec");

		if (code != 0) {
			return E_FAIL;
		}

		// On success, the log is written to Desktop\WindowsUpdate.log.
		hr = SHGetFolderPath(0, CSIDL_DESKTOP, NULL, 0, workDir);
		CHECK_HR_OR_RETURN(L"SHGetFolderPath");
	}

	return Exec(L"open", L"WindowsUpdate.log", NULL, workDir, nCmdShow, FALSE, NULL);
}
