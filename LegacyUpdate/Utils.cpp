#include "stdafx.h"
#include <comdef.h>
#include <atlstr.h>
#include <shlwapi.h>
#include "Exec.h"
#include "HResult.h"
#include "LegacyUpdate.h"
#include "VersionInfo.h"
#include "Wow64.h"
#include "WMI.h"

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib")

typedef DWORD (WINAPI *_InitiateShutdownW)(LPWSTR lpMachineName, LPWSTR lpMessage, DWORD dwGracePeriod, DWORD dwShutdownFlags, DWORD dwReason);

static CComVariant _productName;

typedef struct {
	LPWSTR library;
	UINT stringID;
} WinNT5BrandString;

typedef struct {
	DWORD version;
	DWORD osFlag;
	WORD archFlag;
	UINT stringIDs[3];
} WinNT5Variant;

#define STR_WINXP             0
#define STR_SRV03             1
#define STR_IA64              2
#define STR_EMBEDDED          3
#define STR_HOME              4
#define STR_PRO               5
#define STR_STANDARD          6
#define STR_ENTERPRISE        7
#define STR_DATACENTER        8
#define STR_BLADE             9
#define STR_SBS              10

#define STR_TABLETPC2005     11

#define STR_WIN              12

#define STR_TABLETPC         13
#define STR_MEDIACENTER      14
#define STR_STARTER          15
#define STR_EMBPOS           16
#define STR_WINFLP           17
#define STR_EMBSTD2009       18
#define STR_EMBPOS2009       19

#define STR_PROIA64          20
#define STR_STANDARDIA64     21
#define STR_ENTERPRISEIA64   22
#define STR_DATACENTERIA64   23
#define STR_PROX64           24
#define STR_STANDARDX64      25
#define STR_ENTERPRISEX64    26
#define STR_DATACENTERX64    27

#define STR_SRV03R2          28

#define STR_APPLIANCE        29

#define STR_STORAGESERVER_1  30
#define STR_STORAGESERVER_2  31

#define STR_COMPUTECLUSTER_1 32
#define STR_COMPUTECLUSTER_2 33
#define STR_COMPUTECLUSTER_3 34

#define STR_HOMESERVER_1     35
#define STR_HOMESERVER_2     36

static const WinNT5BrandString nt5BrandStrings[] = {
	// Base editions
	{L"sysdm.cpl",      180}, // "Microsoft Windows XP"
	{L"sysdm.cpl",      181}, // "Microsoft Windows Server 2003"
	{L"sysdm.cpl",      188}, // "64-Bit Edition"
	{L"sysdm.cpl",      189}, // "Embedded"
	{L"sysdm.cpl",      190}, // "Home Edition"
	{L"sysdm.cpl",      191}, // "Professional"
	{L"sysdm.cpl",      192}, // "Standard Edition"
	{L"sysdm.cpl",      193}, // "Enterprise Edition"
	{L"sysdm.cpl",      194}, // "Datacenter Edition"
	{L"sysdm.cpl",      196}, // "Web Edition"
	{L"sysdm.cpl",      197}, // "for Small Business Server"

	// Post-SP1
	{L"xpsp2res.dll", 13813}, // "Tablet PC Edition 2005"

	// Post-SP2
	{L"xpsp3res.dll",  2000}, // "Microsoft Windows"

	// XP
	{L"winbrand.dll",  2000}, // "Tablet PC Edition"
	{L"winbrand.dll",  2001}, // "Media Center Edition"
	{L"winbrand.dll",  2002}, // "Starter Edition"
	{L"winbrand.dll",  2003}, // "Embedded for Point of Service"
	{L"winbrand.dll",  2004}, // "Fundamentals for Legacy PCs"
	{L"winbrand.dll",  2005}, // "Windows Embedded Standard"
	{L"winbrand.dll",  2006}, // "Embedded POSReady 2009"

	// Server 2003
	{L"ws03res.dll",   4600}, // "Microsoft(R) Windows(R) XP 64-Bit Edition for Itanium-based Systems"
	{L"ws03res.dll",   4601}, // "Microsoft(R) Windows(R) Server 2003, Standard Edition for 64-Bit Itanium-based Systems"
	{L"ws03res.dll",   4602}, // "Microsoft(R) Windows(R) Server 2003, Enterprise Edition for 64-Bit Itanium-based Systems"
	{L"ws03res.dll",   4603}, // "Microsoft(R) Windows(R) Server 2003, Datacenter Edition for 64-Bit Itanium-based Systems"
	{L"ws03res.dll",  13812}, // "Professional x64 Edition"
	{L"ws03res.dll",  13814}, // "Standard x64 Edition"
	{L"ws03res.dll",  13815}, // "Enterprise x64 Edition"
	{L"ws03res.dll",  13816}, // "Datacenter x64 Edition"

	// R2
	{L"r2brand.dll",   1101}, // "Microsoft Windows Server 2003 R2"

	// Appliance Server
	{L"winbrand.dll",  2002}, // "Appliance Server"

	// Storage Server
	{L"wssbrand.dll",  1101}, // "Microsoft"
	{L"wssbrand.dll",  1102}, // "Windows Storage Server 2003 R2"

	// Compute Cluster
	{L"hpcbrand.dll",  1101}, // "Microsoft"
	{L"hpcbrand.dll",  1102}, // "Windows Server 2003"
	{L"hpcbrand.dll",  1103}, // "Compute Cluster Edition"

	// Home Server
	{L"whsbrand.dll",  1101}, // "Microsoft"
	{L"whsbrand.dll",  1102}  // "Windows Home Server"
};

static const WinNT5Variant nt5Variants[] = {
	// XP
	{0x0501, OS_PROFESSIONAL,   PROCESSOR_ARCHITECTURE_IA64,  {STR_WINXP, STR_PRO, STR_IA64}}, // "Microsoft Windows XP Professional 64-Bit Edition"
	{0x0501, OS_HOME,           PROCESSOR_ARCHITECTURE_INTEL, {STR_WINXP, STR_HOME}},          // "Microsoft Windows XP Home Edition"
	{0x0501, OS_TABLETPC,       PROCESSOR_ARCHITECTURE_INTEL, {STR_WINXP, STR_TABLETPC}},      // "Microsoft Windows XP Tablet PC Edition"
	{0x0501, OS_MEDIACENTER,    PROCESSOR_ARCHITECTURE_INTEL, {STR_WINXP, STR_MEDIACENTER}},   // "Microsoft Windows XP Media Center Edition"
	{0x0501, OS_STARTER,        PROCESSOR_ARCHITECTURE_INTEL, {STR_WINXP, STR_STARTER}},       // "Microsoft Windows XP Starter Edition"
	{0x0501, OS_EMBPOS,         PROCESSOR_ARCHITECTURE_INTEL, {STR_WIN,   STR_EMBPOS}},        // "Microsoft Windows Embedded for Point of Service"
	{0x0501, OS_WINFLP,         PROCESSOR_ARCHITECTURE_INTEL, {STR_WIN,   STR_WINFLP}},        // "Microsoft Windows Fundamentals for Legacy PCs"
	{0x0501, OS_EMBSTD2009,     PROCESSOR_ARCHITECTURE_INTEL, {STR_WIN,   STR_EMBSTD2009}},    // "Microsoft Windows Embedded Standard"
	{0x0501, OS_EMBPOS2009,     PROCESSOR_ARCHITECTURE_INTEL, {STR_WIN,   STR_EMBPOS2009}},    // "Microsoft Windows Embedded POSReady 2009"
	// Check for XP Embedded last as WES2009 also identifies as OS_EMBEDDED.
	{0x0501, OS_EMBEDDED,       PROCESSOR_ARCHITECTURE_INTEL, {STR_WINXP, STR_EMBEDDED}},      // "Microsoft Windows XP Embedded"
	{0x0501, OS_PROFESSIONAL,   PROCESSOR_ARCHITECTURE_INTEL, {STR_WINXP, STR_PRO}},           // "Microsoft Windows XP Professional"

	// Server 2003
	{0x0502, OS_PROFESSIONAL,   PROCESSOR_ARCHITECTURE_IA64,  {STR_WINXP, STR_PRO, STR_IA64}}, // "Microsoft Windows XP Professional 64-Bit Edition"
	{0x0502, OS_SERVER,         PROCESSOR_ARCHITECTURE_IA64,  {STR_STANDARDIA64}},             // "Microsoft(R) Windows(R) Server 2003, Standard Edition for 64-Bit Itanium-based Systems"
	{0x0502, OS_ADVSERVER,      PROCESSOR_ARCHITECTURE_IA64,  {STR_ENTERPRISEIA64}},           // "Microsoft(R) Windows(R) Server 2003, Enterprise Edition for 64-Bit Itanium-based Systems"
	{0x0502, OS_DATACENTER,     PROCESSOR_ARCHITECTURE_IA64,  {STR_DATACENTERIA64}},           // "Microsoft(R) Windows(R) Server 2003, Datacenter Edition for 64-Bit Itanium-based Systems"
	{0x0502, OS_APPLIANCE,      PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_APPLIANCE}},     // "Microsoft Windows Server 2003 Appliance Server"
	{0x0502, OS_STORAGESERVER,  PROCESSOR_ARCHITECTURE_INTEL, {STR_STORAGESERVER_1, STR_STORAGESERVER_2}}, // "Microsoft Windows Storage Server 2003 R2"
	{0x0502, OS_COMPUTECLUSTER, PROCESSOR_ARCHITECTURE_INTEL, {STR_COMPUTECLUSTER_1, STR_COMPUTECLUSTER_2, STR_COMPUTECLUSTER_3}}, // "Microsoft Windows Server 2003 Compute Cluster Edition"
	{0x0502, OS_HOMESERVER,     PROCESSOR_ARCHITECTURE_INTEL, {STR_HOMESERVER_1, STR_HOMESERVER_2}}, // "Microsoft Windows Home Server"
	// I don't think any of the above editions identify as OS_SERVER, but just in case
	{0x0502, OS_SERVER,         PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_STANDARD}},      // "Microsoft Windows Server 2003 Standard Edition"
	{0x0502, OS_ADVSERVER,      PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_ENTERPRISE}},    // "Microsoft Windows Server 2003 Enterprise Edition"
	{0x0502, OS_WEBSERVER,      PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_BLADE}},         // "Microsoft Windows Server 2003 Web Edition"
	{0x0502, OS_DATACENTER,     PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_DATACENTER}},    // "Microsoft Windows Server 2003 Datacenter Edition"
	{0x0502, OS_SMALLBUSINESSSERVER, PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_SBS}},      // "Microsoft Windows Server 2003 for Small Business Server"
};

HRESULT GetOSProductName(LPVARIANT productName) {
	if (_productName.vt == VT_EMPTY) {
		VariantInit(&_productName);

		// Handle the absolute disaster of Windows XP/Server 2003 edition branding
		WORD winver = GetWinVer();
		if (HIBYTE(winver) == 5) {
			SYSTEM_INFO systemInfo = {0};
			OurGetNativeSystemInfo(&systemInfo);

			WinNT5Variant variant = {0};
			for (DWORD i = 0; i < ARRAYSIZE(nt5Variants); i++) {
				WinNT5Variant thisVariant = nt5Variants[i];
				if (winver == thisVariant.version && systemInfo.wProcessorArchitecture == thisVariant.archFlag && IsOS(thisVariant.osFlag)) {
					variant = thisVariant;
					break;
				}
			}

			if (variant.version) {
				WCHAR brandStr[1024];
				ZeroMemory(brandStr, ARRAYSIZE(brandStr));

				for (DWORD i = 0; variant.stringIDs[i] != 0; i++) {
					UINT id = variant.stringIDs[i];

					// If Server 2003 R2, override to R2 string
					if (id == STR_SRV03 && IsOS(OS_SERVERR2)) {
						id = STR_SRV03R2;
					}

					WinNT5BrandString brandString = nt5BrandStrings[id];
					WCHAR str[340];
					HMODULE dll = LoadLibraryEx(brandString.library, NULL, LOAD_LIBRARY_AS_DATAFILE);
					if (dll) {
						LoadString(dll, brandString.stringID, str, ARRAYSIZE(str));
						FreeLibrary(dll);
					}

					if (i > 0) {
						wcscat(brandStr, L" ");
					}

					wcscat(brandStr, str);
				}

				if (wcslen(brandStr) > 0) {
					_productName.vt = VT_BSTR;
					_productName.bstrVal = SysAllocString(brandStr);
				}
			}
		}

		if (_productName.vt == VT_EMPTY) {
			// Get from WMI
			HRESULT hr = QueryWMIProperty(L"SELECT Caption FROM Win32_OperatingSystem", L"Caption", &_productName);
			if (!SUCCEEDED(hr)) {
				return hr;
			}
		}
	}

	VariantCopy(productName, &_productName);
	return S_OK;
}

HRESULT StartLauncher(LPWSTR params, BOOL wait) {
	LPWSTR path = NULL;
	HRESULT hr = GetInstallPath(&path);
	if (!SUCCEEDED(hr)) {
		return hr;
	}

	PathAppend(path, L"LegacyUpdate.exe");

	DWORD code = 0;
	hr = Exec(L"open", path, params, NULL, SW_SHOW, wait, &code);
	if (SUCCEEDED(hr)) {
		hr = HRESULT_FROM_WIN32(code);
	}

	return hr;
}

HRESULT Reboot() {
	HRESULT hr = E_FAIL;

	HANDLE token = NULL;
	TOKEN_PRIVILEGES privileges = {0};
	LUID shutdownLuid = {0};

	// Make sure we have permission to shut down
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
		hr = AtlHresultFromLastError();
		TRACE("OpenProcessToken() failed: %ls\n", GetMessageForHresult(hr));
		goto end;
	}

	if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &shutdownLuid)) {
		hr = AtlHresultFromLastError();
		TRACE("LookupPrivilegeValue() failed: %ls\n", GetMessageForHresult(hr));
		goto end;
	}

	// Ask the system nicely to give us shutdown privilege
	privileges.PrivilegeCount = 1;
	privileges.Privileges[0].Luid = shutdownLuid;
	privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(token, FALSE, &privileges, 0, NULL, NULL)) {
		hr = AtlHresultFromLastError();
		TRACE("AdjustTokenPrivileges() failed: %ls\n", GetMessageForHresult(hr));
		goto end;
	}

	// Reboot with reason "Operating System: Security fix (Unplanned)", ensuring to install updates.
	// Try InitiateShutdown first (Vista+)
	_InitiateShutdownW $InitiateShutdownW = (_InitiateShutdownW)GetProcAddress(GetModuleHandle(L"advapi32.dll"), "InitiateShutdownW");
	if ($InitiateShutdownW) {
		hr = HRESULT_FROM_WIN32($InitiateShutdownW(NULL, NULL, 0, SHUTDOWN_RESTART | SHUTDOWN_INSTALL_UPDATES, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX));
	}

	// Try InitiateSystemShutdownEx (2k/XP)
	if (!SUCCEEDED(hr)) {
		TRACE("InitiateShutdown() failed: %ls\n", GetMessageForHresult(hr));

		if (InitiateSystemShutdownEx(NULL, NULL, 0, FALSE, TRUE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX) == 0) {
			hr = AtlHresultFromLastError();
			TRACE("InitiateSystemShutdownExW() failed: %ls\n", GetMessageForHresult(hr));
		}
	}

	// Last-ditch attempt ExitWindowsEx (only guaranteed to work for the current logged in user)
	if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_SECURITYFIX)) {
		hr = AtlHresultFromLastError();
		TRACE("ExitWindowsEx() failed: %ls\n", GetMessageForHresult(hr));
	}

end:
	if (token) {
		CloseHandle(token);
	}

	return hr;
}
