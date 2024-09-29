#include "stdafx.h"
#include "VersionInfo.h"

static BOOL _loadedOwnVersion = FALSE;
static LPWSTR _version;

HRESULT GetOwnVersion(LPWSTR *version) {
	if (!_loadedOwnVersion) {
		_loadedOwnVersion = TRUE;

		LPWSTR filename;
		GetOwnFileName(&filename);

		DWORD verHandle;
		DWORD verInfoSize = GetFileVersionInfoSize(filename, &verHandle);
		if (verInfoSize == 0) {
			LocalFree(filename);
			return HRESULT_FROM_WIN32(GetLastError());
		}

		LPVOID verInfo = LocalAlloc(LPTR, verInfoSize);
		if (!GetFileVersionInfo(filename, verHandle, verInfoSize, verInfo)) {
			LocalFree(filename);
			LocalFree(verInfo);
			return HRESULT_FROM_WIN32(GetLastError());
		}

		LocalFree(filename);

		UINT size;
		if (!VerQueryValue(verInfo, L"\\StringFileInfo\\040904B0\\ProductVersion", (LPVOID *)&_version, &size)) {
			LocalFree(verInfo);
			return HRESULT_FROM_WIN32(GetLastError());
		}

		LocalFree(verInfo);
	}

	*version = _version;
	return _version == NULL ? E_FAIL : S_OK;
}
