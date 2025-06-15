#include "stdafx.h"
#include "NGen.h"
#include "Exec.h"
#include "VersionInfo.h"
#include "Wow64.h"
#include <Shlwapi.h>

STDMETHODIMP PauseResumeNGenQueue(BOOL state) {
	// Pause and resume .NET Framework global assembly cache (GAC) and NGen queue
	static LPWSTR versions[] = {
		L"v4.0.30319",
		L"v2.0.50727"
	};

	SYSTEM_INFO systemInfo;
	OurGetNativeSystemInfo(&systemInfo);
	BOOL hasFramework64 = systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64;

	HRESULT hr = S_OK;

	for (int i = 0; i < ARRAYSIZE(versions); i++) {
		for (int j = 0; j < 2; j++) {
			if (j == 1 && !hasFramework64) {
				break;
			}

			WCHAR path[MAX_PATH];
			wsprintf(path, L"%%SystemRoot%%\\Microsoft.NET\\Framework%ls\\%ls\\ngen.exe", j == 1 ? L"64": L"", versions[i]);
			ExpandEnvironmentStrings(path, path, ARRAYSIZE(path));

			if (PathFileExists(path)) {
				hr = Exec(NULL, path, state ? L"queue pause" : L"queue continue", NULL, SW_HIDE, TRUE, NULL);

				if (SUCCEEDED(hr)) {
					break;
				}
			}
		}
	}

	return hr;
}
