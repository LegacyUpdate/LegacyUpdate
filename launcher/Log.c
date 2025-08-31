#include "WULog.h"
#include "MsgBox.h"

void LaunchLog(int nCmdShow) {
	HRESULT hr = ViewWindowsUpdateLog(nCmdShow);
	if (!SUCCEEDED(hr)) {
		LPWSTR message = GetMessageForHresult(hr);
		MsgBox(NULL, message, NULL, MB_OK);
		LocalFree(message);
	}

	PostQuitMessage(hr);
}
