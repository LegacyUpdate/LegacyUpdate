!define ERROR_ELEVATION_REQUIRED 740

!define MUI_UI_HEADERIMAGE "modern_headerbmp_full.exe"

!define MUI_COMPONENTSPAGE_TEXT_TOP "Legacy Update will be installed. Windows Update will be configured to use the Legacy Update proxy server.$\r$\nAn internet connection is required to download additional components from Microsoft. You can select optional system updates from the list below."
!define MUI_COMPONENTSPAGE_SMALLDESC

!define MUI_UNCONFIRMPAGE_TEXT_TOP "Legacy Update will be uninstalled. Your Windows Update configuration will be reset to directly use Microsoft servers."

!define NAME    "Legacy Update"
!define VERSION "1.1"
!define DOMAIN  "legacyupdate.net"

!define WEBSITE            "http://legacyupdate.net/"
!define UPDATE_URL         "http://legacyupdate.net/windowsupdate/v6/"
!define WSUS_SERVER        "http://legacyupdate.net/v6"
!define WSUS_STATUS_SERVER "http://stats.update.microsoft.com"

!define REGPATH_UNINSTSUBKEY   "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
!define REGPATH_WUPOLICY       "Software\Policies\Microsoft\Windows\WindowsUpdate"
!define REGPATH_WUAUPOLICY     "Software\Policies\Microsoft\Windows\WindowsUpdate\AU"
!define REGPATH_WU             "Software\Microsoft\Windows\CurrentVersion\WindowsUpdate"
!define REGPATH_ZONEDOMAINS    "Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains"
!define REGPATH_ZONEESCDOMAINS "Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\EscDomains"

Name "${NAME}"
Caption "Install ${NAME}"
BrandingText "${NAME} ${VERSION} - ${DOMAIN}"
OutFile "${NAME} ${VERSION}.exe"
InstallDir "$ProgramFiles\$(^Name)"
InstallDirRegKey HKLM "${REGPATH_UNINSTSUBKEY}" "UninstallString"

Unicode True
RequestExecutionLevel Admin

!define MUI_ABORTWARNING

!define MUI_ICON "icon.ico"
!define MUI_UNICON "icon.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "setupbanner.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "setupbanner.bmp"

!include FileFunc.nsh
!include Integration.nsh
!include LogicLib.nsh
!include MUI2.nsh
!include Sections.nsh
!include WinVer.nsh
!include WordFunc.nsh
!include x64.nsh

!include DownloadW2K.nsh
!include DownloadWUA.nsh
!include Reboot.nsh
!include UpdateRoots.nsh
!include VCRedist.nsh

!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

!macro EnsureAdminRights
	UserInfo::GetAccountType
	Pop $0
	${If} $0 != "admin" ; Require admin rights on WinNT4+
		MessageBox MB_IconStop "Log on as an administrator to install Legacy Update."
		SetErrorLevel ERROR_ELEVATION_REQUIRED
		Quit
	${EndIf}
!macroend

Function RestartWUAUService
	ExecShellWait "" "net" "stop wuauserv" SW_HIDE
	Delete "$WINDIR\SoftwareDistribution\WuRedir"
	ExecShellWait "" "net" "start wuauserv" SW_HIDE
FunctionEnd

Function .onInit
	SetShellVarContext All
	!insertmacro EnsureAdminRights

	; Ensure extraction to SysWOW64 on x64
	${EnableX64FSRedirection}

	${If} ${IsWin2000}
		${WinVerGetServicePackLevel} $0
		${If} $0 < 3
			!insertmacro SelectSection "Windows 2000 SP4"
		${Else}
			!insertmacro RemoveSection "Windows 2000 SP4"
		${EndIf}

		${GetFileVersion} "$SYSDIR\kernel32.dll" $0
		${VersionCompare} $0 "5.00.2195.7000" $1
		${If} $1 == 2
			!insertmacro SelectSection "Windows 2000 KB835732 Update"
		${Else}
			!insertmacro RemoveSection "Windows 2000 KB835732 Update"
		${EndIf}

		${GetFileVersion} "$SYSDIR\mshtml.dll" $0
		${VersionCompare} $0 "6.0.2600.0" $1
		${If} $1 == 2
			!insertmacro SelectSection "Internet Explorer 6.0 SP1"
		${Else}
			!insertmacro RemoveSection "Internet Explorer 6.0 SP1"
		${EndIf}
	${Else}
		!insertmacro RemoveSection "Windows 2000 SP4"
		!insertmacro RemoveSection "Windows 2000 KB835732 Update"
		!insertmacro RemoveSection "Internet Explorer 6.0 SP1"
	${EndIf}
FunctionEnd

Function .onInstSuccess
	!insertmacro CheckRebootRequired
	IfRebootFlag +2 0
		Exec 'rundll32.exe "$SYSDIR\LegacyUpdate.dll",LaunchUpdateSite'
FunctionEnd

Function un.onInit
	SetShellVarContext All
	!insertmacro EnsureAdminRights
FunctionEnd

; Win2k prerequisities
Section "Windows 2000 SP4" W2KSP4
	SectionIn Ro
	Call DownloadW2KSP4
	!insertmacro CheckRebootRequired
SectionEnd

Section "Windows 2000 KB835732 Update" KB835732
	SectionIn Ro
	Call DownloadKB835732
SectionEnd

Section "Internet Explorer 6.0 SP1" IE6SP1
	SectionIn Ro
	Call DownloadIE6
	!insertmacro CheckRebootRequired
SectionEnd

; Shared prerequisites
Section -VCRedist
	Call InstallVCRedist
SectionEnd

Section -WUA
	Call DownloadWUA
SectionEnd

; Main installation
Section "Legacy Update" LEGACYUPDATE
	SectionIn Ro

	SetOutPath $InstDir
	WriteUninstaller "Uninstall.exe"

	; Add uninstall entry
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayName" "${NAME}"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayIcon" "$InstDir\Uninstall.exe,0"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayVersion" "${VERSION}"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "Publisher" "${NAME}"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "URLInfoAbout" "${WEBSITE}"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "UninstallString" '"$InstDir\Uninstall.exe"'
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "QuietUninstallString" '"$InstDir\Uninstall.exe" /S'
	WriteRegDWORD HKLM "${REGPATH_UNINSTSUBKEY}" "NoModify" 1
	WriteRegDWORD HKLM "${REGPATH_UNINSTSUBKEY}" "NoRepair" 1

	; Needed so the shortcut points to the right path
	${If} ${RunningX64}
		SetOutPath $WINDIR\SysWOW64
	${Else}
		SetOutPath $SYSDIR
	${EndIf}

	; Register DLL
	File /oname=LegacyUpdate.dll "..\Release\LegacyUpdateOCX.dll"
	RegDLL "$OUTDIR\LegacyUpdate.dll"

	; Create shortcut
	CreateShortcut "$COMMONSTARTMENU\${NAME}.lnk" "rundll32.exe" '"$OUTDIR\LegacyUpdate.dll",LaunchUpdateSite' "$OUTDIR\LegacyUpdate.dll" 0 SW_SHOWNORMAL 0 "Install Windows Updates"

	; Set WSUS server
	WriteRegStr HKLM "${REGPATH_WUPOLICY}" "WUServer" "${WSUS_SERVER}"
	WriteRegStr HKLM "${REGPATH_WUPOLICY}" "WUStatusServer" "${WSUS_STATUS_SERVER}"
	WriteRegDWORD HKLM "${REGPATH_WUAUPOLICY}" "UseWUStatusServer" 1

	; Set WU URL
	WriteRegStr HKLM "${REGPATH_WU}" "URL" "${UPDATE_URL}"
	
	; Add to trusted sites
	WriteRegDword HKCU "${REGPATH_ZONEDOMAINS}\${DOMAIN}" "http"  2
	WriteRegDword HKCU "${REGPATH_ZONEDOMAINS}\${DOMAIN}" "https" 2
	WriteRegDword HKCU "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}" "http"  2
	WriteRegDword HKCU "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}" "https" 2

	; Restart service
	DetailPrint "Restarting Windows Update service..."
	Call RestartWUAUService
SectionEnd

; Extras
Section "Update root certificates store" ROOTCERTS
	Call UpdateRoots
SectionEnd

!macro DeleteFileOrAskAbort path
	ClearErrors
	Delete "${path}"
	IfErrors 0 +3
		MessageBox MB_ABORTRETRYIGNORE|MB_ICONSTOP 'Unable to delete "${path}". If Internet Explorer is open, close it and click Retry.' IDRETRY -3 IDIGNORE +2
		Abort "Aborted"
!macroend

Section -Uninstall
	; Delete shortcut
	${UnpinShortcut} "$COMMONSTARTMENU\${NAME}.lnk"
	Delete "$COMMONSTARTMENU\${NAME}.lnk"

	; Unregister dll
	!insertmacro DeleteFileOrAskAbort "$SYSDIR\LegacyUpdate.dll"
	UnRegDLL "$OUTDIR\LegacyUpdate.dll"

	; Clear WSUS server
	DeleteRegValue HKLM "${REGPATH_WUPOLICY}" "WUServer"
	DeleteRegValue HKLM "${REGPATH_WUPOLICY}" "WUStatusServer"
	DeleteRegValue HKLM "${REGPATH_WUAUPOLICY}" "UseWUStatusServer"

	; Clear WU URL
	DeleteRegValue HKLM "${REGPATH_WU}" "URL"
	
	; Remove from trusted sites
	DeleteRegKey HKCU "${REGPATH_ZONEDOMAINS}\${DOMAIN}"
	DeleteRegKey HKCU "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}"

	; Restart service
	DetailPrint "Restarting Windows Update service..."
	Call RestartWUAUService

	; Delete the rest
	Delete "$InstDir\Uninstall.exe"
	RMDir "$InstDir"
	DeleteRegKey HKLM "${REGPATH_UNINSTSUBKEY}"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${W2KSP4}       "Windows 2000 SP3 or later is required to use Legacy Update. This will download and install SP4."
  !insertmacro MUI_DESCRIPTION_TEXT ${KB835732}     "KB835732 is required to upgrade some operating system functionality. This will download and install KB835732."
  !insertmacro MUI_DESCRIPTION_TEXT ${IE6SP1}       "Internet Explorer 6 is required to use Legacy Update. This will download and install Internet Explorer 6.0 SP1."
  !insertmacro MUI_DESCRIPTION_TEXT ${LEGACYUPDATE} "Installs Legacy Update ActiveX control and Start Menu shortcut."
	!insertmacro MUI_DESCRIPTION_TEXT ${ROOTCERTS}    "Update the root certificate store with the latest from Microsoft. Fixes connection issues with some websites."
!insertmacro MUI_FUNCTION_DESCRIPTION_END
