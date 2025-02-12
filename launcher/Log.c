#include "WULog.h"
#include "MsgBox.h"

void LaunchLog(int nCmdShow) {
	HRESULT hr = ViewWindowsUpdateLog(nCmdShow);
	if (!SUCCEEDED(hr)) {
		MsgBox(NULL, GetMessageForHresult(hr), NULL, MB_OK);
	}

	PostQuitMessage(hr);
}
