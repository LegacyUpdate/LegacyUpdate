#include "stdafx.h"
#include "Exec.h"

HRESULT Exec(LPWSTR verb, LPWSTR file, LPWSTR params, LPWSTR workingDir, WORD show, BOOL wait, LPDWORD exitCode) {
	SHELLEXECUTEINFO execInfo = {0};
	execInfo.cbSize = sizeof(execInfo);
	execInfo.lpVerb = verb;
	execInfo.lpFile = file;
	execInfo.lpParameters = params;
	execInfo.lpDirectory = workingDir;
	execInfo.nShow = show;
	return ExecEx(&execInfo, wait, exitCode);
}

HRESULT ExecEx(LPSHELLEXECUTEINFO execInfo, BOOL wait, LPDWORD exitCode) {
	if (wait) {
		execInfo->fMask |= SEE_MASK_NOCLOSEPROCESS;
	}

	if (!ShellExecuteEx(execInfo)) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	HRESULT hr = S_OK;

	if (wait) {
		if (execInfo->hProcess == NULL) {
			TRACE(L"ShellExecuteEx() didn't return a handle: %u", GetLastError());
			hr = E_FAIL;
		}

		WaitForSingleObject(execInfo->hProcess, INFINITE);

		if (exitCode != NULL && GetExitCodeProcess(execInfo->hProcess, exitCode) == 0) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			TRACE(L"GetExitCodeProcess() failed: %p\n", hr);
		}

		CloseHandle(execInfo->hProcess);
	}

	return hr;
}
