; Product
!define NAME               "Legacy Update"
!define DOMAIN             "legacyupdate.net"

; Build
!if ${DEBUG} == 1
	!define VSBUILD32          "Debug-VC08"
	!define VSBUILD64          "Debug-VC17"
!else
	!if ${CI} == 1
		!define VSBUILD32        "Debug-VC17"
		!define VSBUILD64        "Debug-VC17"
	!else
		!define VSBUILD32        "Release"
		!define VSBUILD64        "Release"
	!endif
!endif

; Version
!getdllversion "..\${VSBUILD32}\LegacyUpdate.dll" DLLVersion_
!define LONGVERSION        "${DLLVersion_1}.${DLLVersion_2}.${DLLVersion_3}.${DLLVersion_4}"
!define VERSION            "${DLLVersion_1}.${DLLVersion_2}.${DLLVersion_3}"

!if ${DLLVersion_3} == 0
	!define /redef VERSION   "${DLLVersion_1}.${DLLVersion_2}"
!endif

; Main URLs
!define WEBSITE            "http://legacyupdate.net/"
!define UPDATE_URL         "http://legacyupdate.net/windowsupdate/v6/"
!define UPDATE_URL_HTTPS   "https://legacyupdate.net/windowsupdate/v6/"
!define WSUS_SERVER        "http://legacyupdate.net/v6"
!define WSUS_SERVER_HTTPS  "https://legacyupdate.net/v6"
!define TRUSTEDR           "http://download.windowsupdate.com/msdownload/update/v3/static/trustedr/en"

; Control Panel entry
!define CPL_GUID           "{FFBE8D44-E9CF-4DD8-9FD6-976802C94D9C}"
!define CPL_APPNAME        "LegacyUpdate"

; Legacy Update keys
!define REGPATH_LEGACYUPDATE_SETUP "Software\Hashbang Productions\Legacy Update\Setup"
!define REGPATH_UNINSTSUBKEY       "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"

; Control Panel entry
!define REGPATH_CPLNAMESPACE       "Software\Microsoft\Windows\CurrentVersion\Explorer\ControlPanel\NameSpace\${CPL_GUID}"
!define REGPATH_HKCR_CPLCLSID      "CLSID\${CPL_GUID}"

; XP POSReady hack
!define REGPATH_POSREADY           "System\WPA\PosReady"

; RunOnce
!define REGPATH_SETUP              "System\Setup"
!define REGPATH_RUNONCE            "Software\Microsoft\Windows\CurrentVersion\RunOnce"

; Windows Update keys
!define REGPATH_WU                 "Software\Microsoft\Windows\CurrentVersion\WindowsUpdate"
!define REGPATH_WU_SERVICES        "${REGPATH_WU}\Services"
!define REGPATH_WU_OSUPGRADE       "${REGPATH_WU}\OSUpgrade"

; Windows Update policies
!define REGPATH_WUPOLICY           "Software\Policies\Microsoft\Windows\WindowsUpdate"
!define REGPATH_WUAUPOLICY         "${REGPATH_WUPOLICY}\AU"

; Component keys
!define REGPATH_CBS                "Software\Microsoft\Windows\CurrentVersion\Component Based Servicing"
!define REGPATH_CBS_PKGSPENDING    "${REGPATH_CBS}\PackagesPending"
!define REGPATH_CBS_PACKAGEINDEX   "${REGPATH_CBS}\PackageIndex"

; IE zone keys
!define REGPATH_INETSETTINGS       "Software\Microsoft\Windows\CurrentVersion\Internet Settings"
!define REGPATH_ZONEDOMAINS        "${REGPATH_INETSETTINGS}\ZoneMap\Domains"
!define REGPATH_ZONEESCDOMAINS     "${REGPATH_INETSETTINGS}\ZoneMap\EscDomains"

; SChannel protocol keys
!define REGPATH_SCHANNEL_PROTOCOLS "System\CurrentControlSet\Control\SecurityProviders\SChannel\Protocols"
!define REGPATH_DOTNET             "Software\Microsoft\.NETFramework"
!define REGPATH_DOTNET_V2          "${REGPATH_DOTNET}\v2.0.50727"
!define REGPATH_DOTNET_V4          "${REGPATH_DOTNET}\v4.0.30319"
