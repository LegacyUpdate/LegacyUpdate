#include <windows.h>
#include <nsis/pluginapi.h>
#include "Registry.h"
#include "VersionInfo.h"

typedef BOOL (WINAPI *_GetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);

static BOOL productInfoLoaded = FALSE;
static _GetProductInfo $GetProductInfo;

BOOL GetVistaProductInfo(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType) {
	if (!productInfoLoaded) {
		productInfoLoaded = TRUE;
		$GetProductInfo = (_GetProductInfo)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetProductInfo");
	}

	if ($GetProductInfo) {
		return $GetProductInfo(dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
	}

	*pdwReturnedProductType = PRODUCT_UNDEFINED;
	return FALSE;
}

PLUGIN_METHOD(IsServerCore) {
	PLUGIN_INIT();

    // Server Core introduced with Server 2008, skip on earlier versions.
    if (!AtLeastWinVista()) {
		pushint(0);
		return;
	}

    // Server 2008 and 2008 R2 use GetProductInfo, 2012 and newer use the registry.
    if (IsWinVista() || IsWin7()) {
        DWORD productType = 0;
        if (!GetVistaProductInfo(6, 0, 0, 0, &productType)) {
            pushint(0);
		    return;
        }

        switch (productType) {
            case PRODUCT_DATACENTER_SERVER_CORE:
            case PRODUCT_STANDARD_SERVER_CORE:
            case PRODUCT_ENTERPRISE_SERVER_CORE:
            case PRODUCT_WEB_SERVER_CORE:
            case PRODUCT_DATACENTER_SERVER_CORE_V:
            case PRODUCT_STANDARD_SERVER_CORE_V:
            case PRODUCT_ENTERPRISE_SERVER_CORE_V:
            case PRODUCT_HYPERV:
            case PRODUCT_STORAGE_EXPRESS_SERVER_CORE:
            case PRODUCT_STORAGE_STANDARD_SERVER_CORE:
            case PRODUCT_STORAGE_WORKGROUP_SERVER_CORE:
            case PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:
            case PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:
            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:
            case PRODUCT_CLUSTER_SERVER_V:
                pushint(1);
                return;
        }
    } else {
        // Only need to test for full GUI, MinShell on 2012/2012 R2 does not have IE and is considered regular Core.
        DWORD serverCore = 0;
        DWORD serverGuiShell = 0;
        GetRegistryDword(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Server\\ServerLevels", L"ServerCore", KEY_WOW64_64KEY, &serverCore);
        GetRegistryDword(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Server\\ServerLevels", L"Server-Gui-Shell", KEY_WOW64_64KEY, &serverGuiShell);
        pushint((serverCore && !serverGuiShell) ? 1 : 0);
        return;
    }

    pushint(0);
}
