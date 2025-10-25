#include <windows.h>
#include <nsis/pluginapi.h>
#include <wincrypt.h>
#include "HResult.h"

PLUGIN_METHOD(UpdateRoots) {
	PLUGIN_INIT();

	HRESULT hr = E_FAIL;
	WCHAR stateStr[1024], store[1024], path[1024];
	popstringn(stateStr, sizeof(stateStr) / sizeof(WCHAR));
	popstringn(store, sizeof(store) / sizeof(WCHAR));
	popstringn(path, sizeof(path) / sizeof(WCHAR));

	if (lstrlen(stateStr) == 0 || lstrlen(store) == 0 || lstrlen(path) == 0) {
		pushint(E_INVALIDARG);
		return;
	}

	BOOL add = FALSE;
	if (lstrcmpi(stateStr, L"/update") == 0) {
		add = TRUE;
	} else if (lstrcmpi(stateStr, L"/delete") == 0) {
		add = FALSE;
	} else {
		pushint(E_INVALIDARG);
		return;
	}

	HCERTSTORE srcStore = CertOpenStore(CERT_STORE_PROV_FILENAME_W, 0, 0, CERT_STORE_READONLY_FLAG, path);
	HCERTSTORE dstStore = NULL;
	if (!srcStore) {
		TRACE(L"CertOpenStore for %ls failed: %08x", path, hr);
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto end;
	}

	dstStore = CertOpenStore(CERT_STORE_PROV_SYSTEM_REGISTRY_W, 0, 0, CERT_SYSTEM_STORE_LOCAL_MACHINE, store);
	if (!dstStore) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		TRACE(L"CertOpenStore for %ls failed: %08x", store, hr);
		goto end;
	}

	PCCERT_CONTEXT cert = NULL;
	while ((cert = CertEnumCertificatesInStore(srcStore, cert)) != NULL) {
		BOOL result = add
			? CertAddCertificateContextToStore(dstStore, cert, CERT_STORE_ADD_REPLACE_EXISTING, NULL)
			: CertDeleteCertificateFromStore(CertDuplicateCertificateContext(cert));
		if (!result) {
			TRACE(L"cert %ls in %ls failed: %d\n", add ? L"add" : L"delete", store, GetLastError());
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto end;
		}
	}

	hr = S_OK;

end:
	if (srcStore) {
		CertCloseStore(srcStore, 0);
	}
	if (dstStore) {
		CertCloseStore(dstStore, 0);
	}

	pushint(hr);
}
