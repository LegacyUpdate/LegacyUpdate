#include "MsgBox.h"
#include "ViewLog.h"

// Matches LogAction enum in ViewLog.h
static const LPCWSTR logActions[][4] = {
	{L"system", L"sys", NULL},
	{L"local", L"user", NULL},
	{L"locallow", L"userlow", L"low", NULL},
	{L"windowsupdate", L"wu", NULL},
};

void LaunchLog(int argc, LPWSTR *argv, int nCmdShow) {
	LogAction action = LogActionSystem;

	if (argc > 0) {
		BOOL found = FALSE;
		for (DWORD i = 0; i < ARRAYSIZE(logActions); i++) {
			for (DWORD j = 0; logActions[i][j] != NULL; j++) {
				if (wcscmp(argv[0], logActions[i][j]) == 0) {
					action = (LogAction)i;
					found = TRUE;
					break;
				}
			}

			if (found) {
				break;
			}
		}
	}

	HRESULT hr = ViewLog(action, nCmdShow, FALSE);
	if (!SUCCEEDED(hr)) {
		LPWSTR message = GetMessageForHresult(hr);
		MsgBox(NULL, message, NULL, MB_OK);
		LocalFree(message);
	}

	PostQuitMessage(hr);
}
