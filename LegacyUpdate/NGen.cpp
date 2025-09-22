#include "NGen.h"
#include "Exec.h"
#include "VersionInfo.h"
#include "Wow64.h"
#include <shlwapi.h>

STDMETHODIMP PauseResumeNGenQueue(BOOL state) {
	// Only necessary prior to Windows 8
	if (AtLeastWin8()) {
		return S_OK;
	}

	// Pause and resume .NET Framework global assembly cache (GAC) and NGen queue
	static LPCWSTR versions[] = {
		L"v4.0.30319",
		L"v2.0.50727"
	};

	SYSTEM_INFO systemInfo;
	OurGetNativeSystemInfo(&systemInfo);
	BOOL hasFramework64 = systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64;

	HRESULT hr = S_OK;

	for (DWORD i = 0; i < ARRAYSIZE(versions); i++) {
		for (int j = 0; j < 2; j++) {
			if (j == 1 && !hasFramework64) {
				break;
			}

			WCHAR path[MAX_PATH], expandedPath[MAX_PATH];
			wsprintf(path, L"%%SystemRoot%%\\Microsoft.NET\\Framework%ls\\%ls\\ngen.exe", j == 1 ? L"64": L"", versions[i]);
			ExpandEnvironmentStrings(path, expandedPath, ARRAYSIZE(expandedPath));

			if (PathFileExists(expandedPath)) {
				SHELLEXECUTEINFO execInfo = {0};
				execInfo.cbSize = sizeof(execInfo);
				execInfo.fMask = SEE_MASK_FLAG_NO_UI;
				execInfo.lpFile = expandedPath;
				execInfo.lpParameters = state ? L"queue pause" : L"queue continue";
				execInfo.nShow = SW_HIDE;
				hr = ExecEx(&execInfo, TRUE, NULL);

				if (SUCCEEDED(hr)) {
					break;
				}
			}
		}
	}

	return hr;
}
