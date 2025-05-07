#include "SelfElevate.h"
#include <windows.h>
#include "Exec.h"
#include "VersionInfo.h"

HRESULT SelfElevate(LPWSTR args, LPDWORD code) {
	LPWSTR fileName = NULL;
	GetOwnFileName(&fileName);
	HRESULT hr = Exec(L"runas", fileName, args, NULL, SW_SHOWDEFAULT, TRUE, code);
	LocalFree(fileName);
	return hr;
}
