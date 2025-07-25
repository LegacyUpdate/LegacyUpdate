#include "stdafx.h"
#include <comdef.h>
#include <atlstr.h>
#include <shlwapi.h>
#include "VersionInfo.h"
#include "WMI.h"
#include "Wow64.h"

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

#define STR_WINXP             1
#define STR_SRV03             2
#define STR_IA64              3
#define STR_EMBEDDED          4
#define STR_HOME              5
#define STR_PRO               6
#define STR_STANDARD          7
#define STR_ENTERPRISE        8
#define STR_DATACENTER        9
#define STR_BLADE            10
#define STR_SBS              11

#define STR_TABLETPC2005     12

#define STR_WIN              13

#define STR_TABLETPC         14
#define STR_MEDIACENTER      15
#define STR_STARTER          16
#define STR_EMBPOS           17
#define STR_WINFLP           18
#define STR_EMBSTD2009       19
#define STR_EMBPOS2009       20

#define STR_PROIA64          21
#define STR_STANDARDIA64     22
#define STR_ENTERPRISEIA64   23
#define STR_DATACENTERIA64   24
#define STR_PROX64           25
#define STR_STANDARDX64      26
#define STR_ENTERPRISEX64    27
#define STR_DATACENTERX64    28

#define STR_SRV03R2          29

#define STR_APPLIANCE        30

#define STR_STORAGESERVER_1  31
#define STR_STORAGESERVER_2  32
#define STR_UDSSERVER_2      33

#define STR_COMPUTECLUSTER_1 34
#define STR_COMPUTECLUSTER_2 35
#define STR_COMPUTECLUSTER_3 36

#define STR_HOMESERVER_1     37
#define STR_HOMESERVER_2     38

static const WinNT5BrandString nt5BrandStrings[] = {
	// Skip index 0
	{},

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
	{L"wssbrand.dll",  1104}, // "Windows UDS Server 2003"

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
	{0x0501, MAXDWORD,          MAXWORD,                      {STR_WINXP}},                    // "Microsoft Windows XP"

	// Server 2003
	{0x0502, OS_PROFESSIONAL,   PROCESSOR_ARCHITECTURE_IA64,  {STR_WINXP, STR_PRO, STR_IA64}}, // "Microsoft Windows XP Professional 64-Bit Edition"
	{0x0502, OS_PROFESSIONAL,   PROCESSOR_ARCHITECTURE_AMD64, {STR_WINXP, STR_PROX64}},        // "Microsoft Windows XP Professional x64 Edition"
	{0x0502, OS_SERVER,         PROCESSOR_ARCHITECTURE_IA64,  {STR_STANDARDIA64}},             // "Microsoft(R) Windows(R) Server 2003, Standard Edition for 64-Bit Itanium-based Systems"
	{0x0502, OS_ADVSERVER,      PROCESSOR_ARCHITECTURE_IA64,  {STR_ENTERPRISEIA64}},           // "Microsoft(R) Windows(R) Server 2003, Enterprise Edition for 64-Bit Itanium-based Systems"
	{0x0502, OS_DATACENTER,     PROCESSOR_ARCHITECTURE_IA64,  {STR_DATACENTERIA64}},           // "Microsoft(R) Windows(R) Server 2003, Datacenter Edition for 64-Bit Itanium-based Systems"
	{0x0502, OS_APPLIANCE,      MAXWORD,                      {STR_SRV03, STR_APPLIANCE}},     // "Microsoft Windows Server 2003, Appliance Server"
	{0x0502, OS_STORAGESERVER,  MAXWORD,                      {STR_STORAGESERVER_1, STR_STORAGESERVER_2}}, // "Microsoft Windows Storage Server 2003 R2"
	// TODO: How do we detect UDS Server?
	{0x0502, OS_COMPUTECLUSTER, MAXWORD,                      {STR_COMPUTECLUSTER_1, STR_COMPUTECLUSTER_2, STR_COMPUTECLUSTER_3}}, // "Microsoft Windows Server 2003 Compute Cluster Edition"
	{0x0502, OS_HOMESERVER,     MAXWORD,                      {STR_HOMESERVER_1, STR_HOMESERVER_2}}, // "Microsoft Windows Home Server"
	// I don't think any of the above editions identify as OS_SERVER, but just in case
	{0x0502, OS_SERVER,         PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_STANDARD}},      // "Microsoft Windows Server 2003, Standard Edition"
	{0x0502, OS_ADVSERVER,      PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_ENTERPRISE}},    // "Microsoft Windows Server 2003, Enterprise Edition"
	{0x0502, OS_WEBSERVER,      PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_BLADE}},         // "Microsoft Windows Server 2003, Web Edition"
	{0x0502, OS_DATACENTER,     PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_DATACENTER}},    // "Microsoft Windows Server 2003, Datacenter Edition"
	{0x0502, OS_SERVER,         PROCESSOR_ARCHITECTURE_AMD64, {STR_SRV03, STR_STANDARDX64}},   // "Microsoft Windows Server 2003, Standard x64 Edition"
	{0x0502, OS_ADVSERVER,      PROCESSOR_ARCHITECTURE_AMD64, {STR_SRV03, STR_ENTERPRISEX64}}, // "Microsoft Windows Server 2003, Enterprise x64 Edition"
	{0x0502, OS_DATACENTER,     PROCESSOR_ARCHITECTURE_AMD64, {STR_SRV03, STR_DATACENTERX64}}, // "Microsoft Windows Server 2003, Datacenter x64 Edition"
	{0x0502, OS_SMALLBUSINESSSERVER, PROCESSOR_ARCHITECTURE_INTEL, {STR_SRV03, STR_SBS}},      // "Microsoft Windows Server 2003 for Small Business Server"
	{0x0502, MAXDWORD,          MAXWORD,                      {STR_SRV03}},                    // "Microsoft Windows Server 2003"
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
				if (thisVariant.version == winver &&
					(thisVariant.archFlag == MAXWORD || thisVariant.archFlag == systemInfo.wProcessorArchitecture) &&
					(thisVariant.osFlag == MAXDWORD || IsOS(thisVariant.osFlag))) {
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

					// If XP Tablet PC SP2, override to 2005 string
					if (id == STR_TABLETPC && GetVersionInfo()->wServicePackMajor >= 2) {
						id = STR_TABLETPC2005;
					}

					WinNT5BrandString brandString = nt5BrandStrings[id];
					WCHAR str[340];
					HMODULE dll = LoadLibraryEx(brandString.library, NULL, LOAD_LIBRARY_AS_DATAFILE);
					if (dll) {
						LoadString(dll, brandString.stringID, str, ARRAYSIZE(str));
						FreeLibrary(dll);
					}

					// If Server 2003 (except SBS), add comma
					if (i == 1) {
						UINT lastID = variant.stringIDs[i - 1];
						if (lastID == STR_SRV03 && !IsOS(OS_SMALLBUSINESSSERVER)) {
							wcscat(brandStr, L",");
						}
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
