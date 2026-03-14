#include "ViewLog.h"

HRESULT ViewLog(LogAction log, int nCmdShow, BOOL showErrors) {
	if (log < 0 || log > LogActionWindowsUpdate) {
		return E_INVALIDARG;
	}

	WCHAR workDir[MAX_PATH];

	switch (log) {
	case LogActionSystem: {
		LPCWSTR logsPath = AtLeastWinVista() ? L"%SystemRoot%\\Logs" : L"%SystemRoot%\\Temp";
		ExpandEnvironmentStrings(logsPath, workDir, ARRAYSIZE(workDir));
		break;
	}

	case LogActionLocal: {
		GetTempPath(ARRAYSIZE(workDir), workDir);
		break;
	}

	case LogActionLocalLow: {
		if (!AtLeastWinVista()) {
			return HRESULT_FROM_WIN32(ERROR_OLD_WIN_VERSION);
		}

		if (AtLeastWin7()) {
			// AppData\Low\Temp
			typedef HRESULT (*_SHGetKnownFolderPath)(const KNOWNFOLDERID *rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
			_SHGetKnownFolderPath $SHGetKnownFolderPath = (_SHGetKnownFolderPath)GetProcAddress(GetModuleHandle(L"shell32.dll"), "SHGetKnownFolderPath");
			if ($SHGetKnownFolderPath == NULL) {
				return HRESULT_FROM_WIN32(ERROR_OLD_WIN_VERSION);
			}

			LPWSTR localLow;
			HRESULT hr = $SHGetKnownFolderPath(&FOLDERID_LocalAppDataLow, CSIDL_FLAG_CREATE, NULL, &localLow);
			CHECK_HR_OR_RETURN(L"SHGetKnownFolderPath");

			wsprintf(workDir, L"%ls\\Temp", localLow);
			LocalFree(localLow);
		} else {
			// AppData\Local\Temp\Low
			WCHAR localTemp[MAX_PATH];
			GetTempPath(ARRAYSIZE(localTemp), localTemp);
			wsprintf(workDir, L"%ls\\Low", localTemp);
		}
		break;
	}

	case LogActionWindowsUpdate: {
		HRESULT hr = SHGetFolderPath(0, CSIDL_WINDOWS, NULL, 0, workDir);
		CHECK_HR_OR_RETURN(L"SHGetFolderPath");

		if (AtLeastWin10()) {
			// Windows 10 moves WU/USO logs to ETW. The ETW logs can be converted back to a plain-text .log using a cmdlet.
			WCHAR powershell[MAX_PATH];
			ExpandEnvironmentStrings(L"%SystemRoot%\\System32\\WindowsPowerShell\\v1.0\\powershell.exe", powershell, ARRAYSIZE(powershell));

			DWORD code = 0;
			hr = Exec(NULL, powershell, L"-NoProfile -Command Get-WindowsUpdateLog", workDir, nCmdShow, TRUE, &code);
			CHECK_HR_OR_RETURN(L"Exec");

			if (code != 0) {
				return E_FAIL;
			}

			// On success, the log is written to Desktop\WindowsUpdate.log.
			hr = SHGetFolderPath(0, CSIDL_DESKTOP, NULL, 0, workDir);
			CHECK_HR_OR_RETURN(L"SHGetFolderPath");
		}
		break;
	}
	}

	LPCWSTR logFileName = log == LogActionWindowsUpdate ? L"WindowsUpdate.log" : L"LegacyUpdate.log";
	WCHAR finalPath[MAX_PATH];
	wsprintf(finalPath, L"%ls\\%ls", workDir, logFileName);

	SHELLEXECUTEINFO execInfo = {0};
	execInfo.cbSize = sizeof(execInfo);
	execInfo.lpVerb = L"open";
	execInfo.lpFile = finalPath;
	execInfo.nShow = nCmdShow;
	execInfo.fMask = showErrors ? 0 : SEE_MASK_FLAG_NO_UI;
	HRESULT hr = ExecEx(&execInfo, FALSE, NULL);
	return hr;
}
