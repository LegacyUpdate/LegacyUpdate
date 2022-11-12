!define ERROR_ELEVATION_REQUIRED 740

!define MUI_UI_HEADERIMAGE              "modern_headerbmp_full.exe"
!define MUI_UI_COMPONENTSPAGE_SMALLDESC "modern_smalldesc.exe"

!define MUI_COMPONENTSPAGE_TEXT_TOP "Legacy Update will be installed. Windows Update will be configured to use the Legacy Update proxy server. An internet connection is required to download additional components from Microsoft. Windows will restart automatically if needed."
!define MUI_COMPONENTSPAGE_SMALLDESC

!define MUI_UNCONFIRMPAGE_TEXT_TOP "Legacy Update will be uninstalled. Your Windows Update configuration will be reset to directly use Microsoft servers."

!define NAME        "Legacy Update"
!define VERSION     "1.1.1"
!define LONGVERSION "1.1.1.0"
!define DOMAIN      "legacyupdate.net"

!define WEBSITE            "http://legacyupdate.net/"
!define UPDATE_URL         "http://legacyupdate.net/windowsupdate/v6/"
!define UPDATE_URL_HTTPS   "https://legacyupdate.net/windowsupdate/v6/"
!define WSUS_SERVER        "http://legacyupdate.net/v6"
!define WSUS_SERVER_HTTPS  "https://legacyupdate.net/v6"

!define CPL_GUID           "{FFBE8D44-E9CF-4DD8-9FD6-976802C94D9C}"
!define CPL_APPNAME        "LegacyUpdate"

!define REGPATH_LEGACYUPDATE_SETUP "Software\Hashbang Productions\Legacy Update\Setup"
!define REGPATH_UNINSTSUBKEY       "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
!define REGPATH_WUPOLICY           "Software\Policies\Microsoft\Windows\WindowsUpdate"
!define REGPATH_WUAUPOLICY         "Software\Policies\Microsoft\Windows\WindowsUpdate\AU"
!define REGPATH_WU                 "Software\Microsoft\Windows\CurrentVersion\WindowsUpdate"
!define REGPATH_ZONEDOMAINS        "Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains"
!define REGPATH_ZONEESCDOMAINS     "Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\EscDomains"
!define REGPATH_CPLNAMESPACE       "Software\Microsoft\Windows\CurrentVersion\Explorer\ControlPanel\NameSpace\${CPL_GUID}"
!define REGPATH_CPLCLSID           "CLSID\${CPL_GUID}"

Name "${NAME}"
Caption "Install ${NAME}"
BrandingText "${NAME} ${VERSION} - ${DOMAIN}"
OutFile "LegacyUpdate.exe"
InstallDir "$ProgramFiles\$(^Name)"
InstallDirRegKey HKLM "${REGPATH_LEGACYUPDATE_SETUP}" "InstallDir"

Unicode True
RequestExecutionLevel Admin
AutoCloseWindow true

VIAddVersionKey /LANG=1033 "ProductName"     "${NAME}"
VIAddVersionKey /LANG=1033 "ProductVersion"  "${LONGVERSION}"
VIAddVersionKey /LANG=1033 "CompanyName"     "Hashbang Productions"
VIAddVersionKey /LANG=1033 "LegalCopyright"  "© Hashbang Productions. All rights reserved."
VIAddVersionKey /LANG=1033 "FileDescription" "${NAME}"
VIAddVersionKey /LANG=1033 "FileVersion"     "${LONGVERSION}"
VIProductVersion ${LONGVERSION}
VIFileVersion ${LONGVERSION}

!define MUI_ICON   "..\icon.ico"
!define MUI_UNICON "..\icon.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP   "setupbanner.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "setupbanner.bmp"

!define MEMENTO_REGISTRY_ROOT HKLM
!define MEMENTO_REGISTRY_KEY  "${REGPATH_LEGACYUPDATE_SETUP}"

!include FileFunc.nsh
!include Integration.nsh
!include LogicLib.nsh
!include Memento.nsh
!include MUI2.nsh
!include Sections.nsh
!include WinVer.nsh
!include WordFunc.nsh
!include x64.nsh

!include Common.nsh
!include Reboot.nsh
!include DownloadW2K.nsh
!include DownloadWUA.nsh
!include UpdateRoots.nsh

!insertmacro GetParameters
!insertmacro GetOptions

!define MUI_PAGE_CUSTOMFUNCTION_PRE ComponentsPageCheck

!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

!macro EnsureAdminRights
	UserInfo::GetAccountType
	Pop $0
	${If} $0 != "admin" ; Require admin rights on NT4+
		MessageBox MB_USERICON "Log on as an administrator to install Legacy Update." /SD IDOK
		SetErrorLevel ERROR_ELEVATION_REQUIRED
		Quit
	${EndIf}
!macroend

!macro DeleteFileOrAskAbort path
	ClearErrors
	Delete "${path}"
	IfErrors 0 +3
		MessageBox MB_RETRYCANCEL|MB_USERICON 'Unable to delete "${path}".$\r$\n$\r$\nIf Internet Explorer is open, close it and click Retry.' /SD IDCANCEL IDRETRY -3
		Abort
!macroend

!macro RestartWUAUService
	!insertmacro DetailPrint "Restarting Windows Update service..."
	ExecShellWait "" "net" "stop wuauserv" SW_HIDE
	Delete "$WINDIR\SoftwareDistribution\WuRedir"
	ExecShellWait "" "net" "start wuauserv" SW_HIDE
!macroend

Function ComponentsPageCheck
	; Skip the page if we're being launched via RunOnce
	${GetParameters} $0
	ClearErrors
	${GetOptions} $0 /runonce $1
	${IfNot} ${Errors}
		Abort
	${EndIf}
FunctionEnd

; Win2k prerequisities
Section "Windows 2000 Service Pack 4" W2KSP4
	SectionIn Ro
	SetOutPath $PLUGINSDIR
	Call DownloadW2KSP4
	Call DownloadKB835732
	Call RebootIfRequired
SectionEnd

${MementoSection} "Internet Explorer 6.0 Service Pack 1" IE6SP1
	SectionIn Ro
	SetOutPath $PLUGINSDIR
	Call DownloadIE6
	Call RebootIfRequired
${MementoSectionEnd}

; Shared prerequisites
Section "Windows Update Agent" WUA
	SectionIn Ro
	Call DownloadWUA
SectionEnd

${MementoSection} "Update root certificates store" ROOTCERTS
	Call UpdateRoots
${MementoSectionEnd}

; Main installation
Section "Legacy Update" LEGACYUPDATE
	SectionIn Ro

	SetOutPath $INSTDIR
	WriteUninstaller "Uninstall.exe"

	; Add uninstall entry
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayName" "${NAME}"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayIcon" '"$OUTDIR\Uninstall.exe",0'
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayVersion" "${VERSION}"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "Publisher" "${NAME}"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "URLInfoAbout" "${WEBSITE}"
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "UninstallString" '"$OUTDIR\Uninstall.exe"'
	WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "QuietUninstallString" '"$OUTDIR\Uninstall.exe" /S'
	WriteRegDWORD HKLM "${REGPATH_UNINSTSUBKEY}" "NoModify" 1
	WriteRegDWORD HKLM "${REGPATH_UNINSTSUBKEY}" "NoRepair" 1

	; Add Control Panel entry
	; Category 5:  XP Performance and Maintenance, Vista System and Maintenance, 7+ System and Security
	; Category 10: XP SP2 Security Center, Vista Security, 7+ System and Security
	WriteRegStr HKCR "${REGPATH_CPLCLSID}" "" "${NAME}"
	WriteRegStr HKCR "${REGPATH_CPLCLSID}" "LocalizedString" '@"$OUTDIR\LegacyUpdate.dll",-2'
	WriteRegStr HKCR "${REGPATH_CPLCLSID}" "InfoTip" '@"$OUTDIR\LegacyUpdate.dll",-4'
	WriteRegStr HKCR "${REGPATH_CPLCLSID}\DefaultIcon" "" '"$OUTDIR\LegacyUpdate.dll",0'
	WriteRegStr HKCR "${REGPATH_CPLCLSID}\Shell\Open\Command" "" 'rundll32.exe "$OUTDIR\LegacyUpdate.dll",LaunchUpdateSite'
	WriteRegDWORD HKCR "${REGPATH_CPLCLSID}\ShellFolder" "Attributes" 0
	WriteRegDWORD HKCR "${REGPATH_CPLCLSID}" "{305CA226-D286-468e-B848-2B2E8E697B74} 2" 5
	WriteRegStr HKCR "${REGPATH_CPLCLSID}" "System.ApplicationName" "${CPL_APPNAME}"
	WriteRegStr HKCR "${REGPATH_CPLCLSID}" "System.ControlPanelCategory" "5,10"
	${If} ${RunningX64}
		WriteRegStr HKCR "${REGPATH_CPLCLSID}" "System.Software.TasksFileUrl" "$OUTDIR\LegacyUpdate.dll,-203"
	${Else}
		WriteRegStr HKCR "${REGPATH_CPLCLSID}" "System.Software.TasksFileUrl" "$OUTDIR\LegacyUpdate.dll,-202"
	${EndIf}
	WriteRegStr HKLM "${REGPATH_CPLNAMESPACE}" "" "${NAME}"

	; Install DLL, with detection for it being in use by IE
	ClearErrors
	SetOverwrite try
	File /oname=LegacyUpdate.dll "..\Release\LegacyUpdateOCX.dll"
	IfErrors 0 +3
		MessageBox MB_RETRYCANCEL|MB_USERICON 'Unable to write to "$OUTDIR\LegacyUpdate.dll".$\r$\n$\r$\nIf Internet Explorer is open, close it and click Retry.' /SD IDCANCEL IDRETRY -3
		Abort
	SetOverwrite on

	; Register DLL
	RegDLL "$OUTDIR\LegacyUpdate.dll"
	IfErrors 0 +3
		MessageBox MB_RETRYCANCEL|MB_USERICON 'Unable to register Legacy Update ActiveX control.$\r$\n$\r$\nIf Internet Explorer is open, close it and click Retry.' /SD IDCANCEL IDRETRY -3
		Abort

	; Create shortcut
	CreateShortcut "$COMMONSTARTMENU\${NAME}.lnk" \
		"$SYSDIR\rundll32.exe" '"$OUTDIR\LegacyUpdate.dll",LaunchUpdateSite' \
		"$OUTDIR\LegacyUpdate.dll" 0 \
		SW_SHOWNORMAL "" \
		'@"$OUTDIR\LegacyUpdate.dll",-4'

	; Set WSUS server
	${If} ${AtMostWin2003}
		; Check if Schannel is going to work with modern TLS
		!insertmacro DetailPrint "Checking SSL connectivity..."
		inetc::get /quiet /tostack "${WSUS_SERVER_HTTPS}/ClientWebService/ping.bin" "" /end
		Pop $0

		${If} $0 == "OK"
			; HTTPS will work
			WriteRegStr HKLM "${REGPATH_WUPOLICY}" "WUServer" "${WSUS_SERVER_HTTPS}"
			WriteRegStr HKLM "${REGPATH_WUPOLICY}" "WUStatusServer" "${WSUS_SERVER_HTTPS}"
			WriteRegStr HKLM "${REGPATH_WU}" "URL" "${UPDATE_URL_HTTPS}"
		${Else}
			; Probably not supported; use HTTP
		WriteRegStr HKLM "${REGPATH_WUPOLICY}" "WUServer" "${WSUS_SERVER}"
		WriteRegStr HKLM "${REGPATH_WUPOLICY}" "WUStatusServer" "${WSUS_SERVER}"
			WriteRegStr HKLM "${REGPATH_WU}" "URL" "${UPDATE_URL}"
		${EndIf}
		WriteRegDWORD HKLM "${REGPATH_WUAUPOLICY}" "UseWUServer" 1
	${EndIf}

	; Add to trusted sites
	WriteRegDword HKCU "${REGPATH_ZONEDOMAINS}\${DOMAIN}" "http"  2
	WriteRegDword HKCU "${REGPATH_ZONEDOMAINS}\${DOMAIN}" "https" 2
	WriteRegDword HKCU "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}" "http"  2
	WriteRegDword HKCU "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}" "https" 2

	; Delete old LegacyUpdate.dll in System32
	${If} ${FileExists} $WINDIR\System32\LegacyUpdate.dll
		UnRegDLL $WINDIR\System32\LegacyUpdate.dll
		Delete $WINDIR\System32\LegacyUpdate.dll
	${EndIf}

	; Delete old LegacyUpdate.inf
	${If} ${FileExists} $WINDIR\inf\LegacyUpdate.inf
		Delete $WINDIR\inf\LegacyUpdate.inf
	${EndIf}

	; Restart service
	!insertmacro RestartWUAUService
SectionEnd

${MementoSectionDone}

Section -Uninstall
	; Delete shortcut
	${UnpinShortcut} "$COMMONSTARTMENU\${NAME}.lnk"
	Delete "$COMMONSTARTMENU\${NAME}.lnk"

	; Delete Control Panel entry
	DeleteRegKey HKLM "${REGPATH_CPLNAMESPACE}"
	DeleteRegKey HKCR "${REGPATH_CPLCLSID}"

	; Unregister dll
	UnRegDLL "$INSTDIR\LegacyUpdate.dll"

	; Clear WSUS server
	${If} ${AtMostWin2003}
		DeleteRegValue HKLM "${REGPATH_WUPOLICY}" "WUServer"
		DeleteRegValue HKLM "${REGPATH_WUPOLICY}" "WUStatusServer"
		DeleteRegValue HKLM "${REGPATH_WUAUPOLICY}" "UseWUStatusServer"
	DeleteRegValue HKLM "${REGPATH_WU}" "URL"
	${EndIf}

	; Remove from trusted sites
	DeleteRegKey HKCU "${REGPATH_ZONEDOMAINS}\${DOMAIN}"
	DeleteRegKey HKCU "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}"

	; Restart service
	!insertmacro RestartWUAUService

	; Delete the rest
	Delete "$INSTDIR\Uninstall.exe"
	!insertmacro DeleteFileOrAskAbort "$INSTDIR\LegacyUpdate.dll"
	RMDir "$INSTDIR"
	DeleteRegKey HKLM "${REGPATH_UNINSTSUBKEY}"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${W2KSP4}       "Updates Windows 2000 to Service Pack 4, as required to install the Windows Update Agent."
	!insertmacro MUI_DESCRIPTION_TEXT ${IE6SP1}       "Updates Internet Explorer to 6.0 SP1, as required for Legacy Update."
	!insertmacro MUI_DESCRIPTION_TEXT ${WUA}          "Updates the Windows Update Agent to the appropriate version required for Legacy Update."
	!insertmacro MUI_DESCRIPTION_TEXT ${ROOTCERTS}    "Updates the root certificate store to the latest from Microsoft. Fixes connection issues with some websites."
	!insertmacro MUI_DESCRIPTION_TEXT ${LEGACYUPDATE} "Installs Legacy Update ActiveX control and Start Menu shortcut."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function .onInit
	SetShellVarContext All
	${If} ${RunningX64}
		SetRegView 64
	${EndIf}
	!insertmacro EnsureAdminRights
	SetDetailsPrint listonly

	SetOutPath $PLUGINSDIR
	File Patches.ini

	${MementoSectionRestore}

	${If} ${IsWin2000}
		; Determine whether Win2k prereqs need to be installed
		${GetFileVersion} "$SYSDIR\kernel32.dll" $1
		${VersionCompare} $1 "5.0.2195.6897" $2
		${If} ${AtLeastServicePack} 3
		${OrIf} $2 != 2
			!insertmacro RemoveSection ${W2KSP4}
		${EndIf}

		${GetFileVersion} "$SYSDIR\mshtml.dll" $0
		${VersionCompare} $0 "6.0.2600.0" $1
		${If} $1 != 2
			!insertmacro RemoveSection ${W2KSP4}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${W2KSP4}
		!insertmacro RemoveSection ${IE6SP1}
	${EndIf}

	Call DetermineWUAVersion
	${If} $0 == ""
		!insertmacro RemoveSection ${WUA}
	${Else}
		!insertmacro SelectSection ${WUA}
	${EndIf}

	${If} ${AtLeastWin8}
		!insertmacro RemoveSection ${ROOTCERTS}
	${EndIf}

	${If} ${AtLeastWin8}
		MessageBox MB_YESNO|MB_USERICON "Legacy Update is intended for earlier versions of Windows. Visit legacyupdate.net for more information.$\r$\n$\r$\nContinue anyway?" /SD IDYES IDYES +2
			Quit
	${EndIf}
FunctionEnd

Function .onInstSuccess
	${MementoSectionSave}
	Call RebootIfRequired
	${IfNot} ${RebootFlag}
		Exec '$SYSDIR\rundll32.exe "$INSTDIR\LegacyUpdate.dll",LaunchUpdateSite'
	${EndIf}
FunctionEnd

Function un.onInit
	SetShellVarContext All
	${If} ${RunningX64}
		SetRegView 64
	${EndIf}
	!insertmacro EnsureAdminRights
	SetDetailsPrint listonly
FunctionEnd

Function un.onUninstSuccess
	Call un.RebootIfRequired
	${IfNot} ${RebootFlag}
		Quit
	${EndIf}
FunctionEnd
