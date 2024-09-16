#include "stdafx.h"
#include "VersionInfo.h"

static BOOL _loadedOwnVersion = FALSE;
static LPWSTR _version;
static UINT _versionSize;

HRESULT GetOwnVersion(LPWSTR *version, LPDWORD size) {
	if (!_loadedOwnVersion) {
		LPWSTR filename;
		DWORD filenameSize;
		GetOwnFileName(&filename, &filenameSize);

		DWORD verHandle;
		DWORD verInfoSize = GetFileVersionInfoSize(filename, &verHandle);
		if (verInfoSize == 0) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		LPVOID verInfo = LocalAlloc(LPTR, verInfoSize);
		if (!GetFileVersionInfo(filename, verHandle, verInfoSize, verInfo)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (!VerQueryValue(verInfo, L"\\StringFileInfo\\040904B0\\ProductVersion", (LPVOID *)&_version, &_versionSize)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}
	}

	*version = _version;
	*size = _versionSize;
	return _version == NULL ? E_FAIL : NOERROR;
}
