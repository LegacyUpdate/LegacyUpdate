; Product
!define NAME               "Legacy Update"
!define DOMAIN             "legacyupdate.net"

; Version
!getdllversion "..\Release\LegacyUpdate.dll" DLLVersion_
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
!define WIN81UPGRADE_URL   "https://go.microsoft.com/fwlink/?LinkId=798437"

; Control Panel entry
!define CPL_GUID           "{FFBE8D44-E9CF-4DD8-9FD6-976802C94D9C}"
!define CPL_APPNAME        "LegacyUpdate"

; RunOnce
!define RUNONCE_USERNAME   "LegacyUpdateTemp"
!define RUNONCE_PASSWORD   "Legacy_Update0"

; Registry keys
!define REGPATH_LEGACYUPDATE_SETUP "Software\Hashbang Productions\Legacy Update\Setup"
!define REGPATH_UNINSTSUBKEY       "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
!define REGPATH_WUPOLICY           "Software\Policies\Microsoft\Windows\WindowsUpdate"
!define REGPATH_WUAUPOLICY         "${REGPATH_WUPOLICY}\AU"
!define REGPATH_WU                 "Software\Microsoft\Windows\CurrentVersion\WindowsUpdate"
!define REGPATH_INETSETTINGS       "Software\Microsoft\Windows\CurrentVersion\Internet Settings"
!define REGPATH_ZONEDOMAINS        "${REGPATH_INETSETTINGS}\ZoneMap\Domains"
!define REGPATH_ZONEESCDOMAINS     "${REGPATH_INETSETTINGS}\ZoneMap\EscDomains"
!define REGPATH_CPLNAMESPACE       "Software\Microsoft\Windows\CurrentVersion\Explorer\ControlPanel\NameSpace\${CPL_GUID}"
!define REGPATH_CPLCLSID           "CLSID\${CPL_GUID}"
!define REGPATH_WINLOGON           "Software\Microsoft\Windows NT\CurrentVersion\Winlogon"
!define REGPATH_POSREADY           "System\WPA\PosReady"
!define REGPATH_SCHANNEL_PROTOCOLS "System\CurrentControlSet\Control\SecurityProviders\SChannel\Protocols"
!define REGPATH_DOTNET_V2          "Software\Microsoft\.NETFramework\v2.0.50727"
!define REGPATH_DOTNET_V4          "Software\Microsoft\.NETFramework\v4.0.30319"
!define REGPATH_RUNONCE            "Software\Microsoft\Windows\CurrentVersion\RunOnce"
!define REGPATH_PACKAGEINDEX       "Software\Microsoft\Windows\CurrentVersion\Component Based Servicing\PackageIndex"
!define REGPATH_SERVICING_SHA2     "Software\Microsoft\Windows\CurrentVersion\Servicing\Codesigning\SHA2"
!define REGPATH_COMPONENT_THEMES   "Software\Microsoft\Active Setup\Installed Components\{2C7339CF-2B09-4501-B3F3-F3508C9228ED}"

; COM
!define CLSID_UpdateServiceManager "{F8D253D9-89A4-4DAA-87B6-1168369F0B21}"
!define IID_IUpdateServiceManager2 "{B596CC9F-56E5-419E-A622-E01BB457431E}"
!define METHOD_AddService2         0x60030003

; Win32 constants
!define EWX_REBOOT       0x02
!define EWX_FORCE        0x04

!define TDCBF_YES_BUTTON 0x2
!define TDCBF_NO_BUTTON  0x4

!define TD_WARNING_ICON  65535

!define IDYES            6

!define PF_XMMI64_INSTRUCTIONS_AVAILABLE    10

!define ES_CONTINUOUS      0x80000000
!define ES_SYSTEM_REQUIRED 0x00000001

; WinINet constants
!define INTERNET_FLAG_RELOAD          0x80000000
!define INTERNET_FLAG_NO_CACHE_WRITE  0x04000000
!define INTERNET_FLAG_KEEP_CONNECTION 0x00400000
!define INTERNET_FLAG_NO_COOKIES      0x00080000
!define INTERNET_FLAG_NO_UI           0x00000200

!define SECURITY_FLAG_STRENGTH_STRONG 0x20000000

; WinHTTP constants
!define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1   0x00000080
!define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1 0x00000200
!define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 0x00000800

!define ERROR_INTERNET_OPERATION_CANCELLED  12017

; Windows Update constants
!define WU_S_ALREADY_INSTALLED 2359302     ; 0x00240006
!define WU_E_NOT_APPLICABLE    -2145124329 ; 0x80240017

!define WU_MU_SERVICE_ID       "7971f918-a847-4430-9279-4a52d1efe18d"
