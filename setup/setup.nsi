!include Constants.nsh

Name         "${NAME}"
Caption      "${NAME}"
BrandingText "${NAME} ${VERSION} - ${DOMAIN}"
OutFile      "LegacyUpdate-${VERSION}.exe"
InstallDir   "$PROGRAMFILES64\Legacy Update"
InstallDirRegKey HKLM "${REGPATH_LEGACYUPDATE_SETUP}" "InstallLocation"

Unicode               true
RequestExecutionLevel admin
AutoCloseWindow       true
ManifestSupportedOS   all
ManifestDPIAware      true
AllowSkipFiles        off
SetCompressor         /SOLID lzma

VIAddVersionKey /LANG=1033 "ProductName"     "${NAME}"
VIAddVersionKey /LANG=1033 "ProductVersion"  "${LONGVERSION}"
VIAddVersionKey /LANG=1033 "CompanyName"     "Hashbang Productions"
VIAddVersionKey /LANG=1033 "LegalCopyright"  "${U+00A9} Hashbang Productions. All rights reserved."
VIAddVersionKey /LANG=1033 "FileDescription" "${NAME}"
VIAddVersionKey /LANG=1033 "FileVersion"     "${LONGVERSION}"
VIProductVersion ${LONGVERSION}
VIFileVersion    ${LONGVERSION}

ReserveFile "${NSIS_TARGET}\System.dll"
ReserveFile "${NSIS_TARGET}\NSxfer.dll"
ReserveFile "${NSIS_TARGET}\LegacyUpdateNSIS.dll"
ReserveFile "banner-wordmark-classic.bmp"
ReserveFile "Patches.ini"
ReserveFile "..\${VSBUILD32}\LegacyUpdate.dll"
ReserveFile "..\x64\${VSBUILD64}\LegacyUpdate.dll"
ReserveFile "..\launcher\obj\LegacyUpdate32.exe"
ReserveFile "..\launcher\obj\LegacyUpdate64.exe"
ReserveFile "updroots.exe"

Var /GLOBAL UninstallInstalled

!define RUNONCEDIR "$COMMONPROGRAMDATA\Legacy Update"

!define MUI_UI                       "modern_aerowizard.exe"
!define MUI_UI_HEADERIMAGE           "modern_aerowizard.exe"

!define MUI_CUSTOMFUNCTION_UNGUIINIT un.OnShow
!define MUI_CUSTOMFUNCTION_ABORT     CleanUp

!define MUI_ICON                     "..\LegacyUpdate\icon.ico"
!define MUI_UNICON                   "..\LegacyUpdate\icon.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP       "banner-wordmark-classic.bmp"
!define MUI_HEADERIMAGE_UNBITMAP     "banner-wordmark-classic.bmp"

!define MUI_TEXT_ABORT_TITLE         "Installation Failed"

!define MEMENTO_REGISTRY_ROOT        HKLM
!define MEMENTO_REGISTRY_KEY         "${REGPATH_LEGACYUPDATE_SETUP}"

!include FileFunc.nsh
!include Integration.nsh
!include LogicLib.nsh
!include Memento.nsh
!include MUI2.nsh
!include nsDialogs.nsh
!include Sections.nsh
!include Win\COM.nsh
!include Win\WinError.nsh
!include Win\WinNT.nsh
!include WinCore.nsh
!include WinMessages.nsh
!include WinVer.nsh
!include WordFunc.nsh
!include x64.nsh

!include Win32.nsh
!include Common.nsh
!include AeroWizard.nsh
!include Download2KXP.nsh
!include DownloadVista78.nsh
!include DownloadWUA.nsh
!include RunOnce.nsh
!include UpdateRoots.nsh
; !include ActiveXPage.nsh

!insertmacro GetParameters
!insertmacro GetOptions

!define MUI_PAGE_HEADER_TEXT         "Welcome to Legacy Update"
!define MUI_COMPONENTSPAGE_TEXT_TOP  "Select what you would like Legacy Update to do. An internet connection is required to download additional components from Microsoft. Your computer will restart automatically if needed. Close all other programs before continuing."
!define MUI_PAGE_CUSTOMFUNCTION_PRE  ComponentsPageCheck
!define MUI_PAGE_CUSTOMFUNCTION_SHOW OnShow
!define MUI_PAGE_FUNCTION_GUIINIT    OnShow
!define MUI_CUSTOMFUNCTION_ONMOUSEOVERSECTION OnMouseOverSection

!insertmacro MUI_PAGE_COMPONENTS

; Page custom ActiveXPage

!define MUI_PAGE_HEADER_TEXT         "Performing Actions"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW OnShow

!insertmacro MUI_PAGE_INSTFILES

!define MUI_PAGE_HEADER_TEXT         "Uninstall Legacy Update"
!define MUI_UNCONFIRMPAGE_TEXT_TOP   "Legacy Update will be uninstalled. Your Windows Update configuration will be reset to directly use Microsoft servers."
!define MUI_PAGE_CUSTOMFUNCTION_SHOW un.OnShow

!insertmacro MUI_UNPAGE_CONFIRM

!define MUI_PAGE_HEADER_TEXT         "Performing Actions"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW un.OnShow

!insertmacro MUI_UNPAGE_INSTFILES

!include Strings.nsh

!macro RestartWUAUService
	!insertmacro DetailPrint "$(StatusRestartingWUAU)"
	LegacyUpdateNSIS::Exec '"$WINDIR\system32\net.exe" stop wuauserv'
!macroend

Function OnShow
	Call AeroWizardOnShow
FunctionEnd

Function un.OnShow
	Call un.AeroWizardOnShow
FunctionEnd

Function MakeUninstallEntry
	${IfNot} $UninstallInstalled == 1
		StrCpy $UninstallInstalled 1
		WriteUninstaller "$INSTDIR\Uninstall.exe"

		; Add uninstall entry
		WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayName"          "${NAME}"
		WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayIcon"          '"$INSTDIR\Uninstall.exe",-103'
		WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayVersion"       "${VERSION}"
		WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "Publisher"            "${NAME}"
		WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "URLInfoAbout"         "${WEBSITE}"
		WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "InstallLocation"      "$INSTDIR"
		WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "UninstallString"      '"$INSTDIR\Uninstall.exe"'
		WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "QuietUninstallString" '"$INSTDIR\Uninstall.exe" /S'
		WriteRegDword HKLM "${REGPATH_UNINSTSUBKEY}" "NoModify"             1
		WriteRegDword HKLM "${REGPATH_UNINSTSUBKEY}" "NoRepair"             1
		${MakeARPInstallDate} $0
		WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "InstallDate" $0
	${EndIf}
FunctionEnd

Section -BeforeInstall
	!insertmacro InhibitSleep 1
SectionEnd

Section -PreDownload
	${IfNot} ${IsRunOnce}
	${AndIfNot} ${IsPostInstall}
		Call PreDownload
	${EndIf}
SectionEnd

Section - PREREQS_START
SectionEnd

; Win2k prerequisities
Section "Windows 2000 $(SP) 4" W2KSP4
	SectionIn Ro
	Call InstallW2KSP4
	Call InstallKB835732
	Call RebootIfRequired
SectionEnd

Section "$(IE) 6.0 $(SP) 1" IE6SP1
	SectionIn Ro
	Call InstallIE6
	Call RebootIfRequired
SectionEnd

; XP 2002 prerequisities
${MementoSection} "Windows XP $(SP) 3" XPSP3
	Call InstallXPSP1a
	Call RebootIfRequired
	Call InstallXPSP3
	Call RebootIfRequired
${MementoSectionEnd}

${MementoSection} "Windows XP $(EMB) $(SP) 3" XPESP3
	Call InstallXPESP3
	Call RebootIfRequired
${MementoSectionEnd}

${MementoUnselectedSection} "$(SectionWES09)" WES09
	WriteRegDword HKLM "${REGPATH_POSREADY}" "Installed" 1
${MementoSectionEnd}

; XP 2003 prerequisities
${MementoSection} "Windows XP/$(SRV) 2003 $(SP) 2" 2003SP2
	Call Install2003SP2
	Call RebootIfRequired
${MementoSectionEnd}

; Vista prerequisities
Section "Windows Vista $(SP) 2" VISTASP2
	SectionIn Ro
	Call InstallVistaSP1
	Call RebootIfRequired
	Call InstallVistaSP2
	Call RebootIfRequired
SectionEnd

Section "$(SectionSSU)" VISTASSU
	SectionIn Ro
	Call InstallKB3205638
	Call InstallKB4012583
	Call InstallKB4015195
	Call InstallKB4015380
	Call InstallKB4493730
	Call RebootIfRequired
SectionEnd

${MementoSection} "$(IE) 9" VISTAIE9
	Call InstallKB971512
	Call InstallKB2117917
	Call RebootIfRequired
	Call InstallIE9
	Call RebootIfRequired
${MementoSectionEnd}

; 7 prerequisities
Section "Windows 7 $(SP) 1" WIN7SP1
	SectionIn Ro
	Call InstallWin7SP1
	Call RebootIfRequired
SectionEnd

Section "$(SectionSSU)" WIN7SSU
	SectionIn Ro
	Call InstallKB3138612
	Call InstallKB4474419
	Call InstallKB4490628
	Call RebootIfRequired
SectionEnd

; Windows Home Server 2011 is based on Server 2008 R2, but has its own separate "rollup" updates
Section "$(SectionWHS2011U4)" WHS2011U4
	SectionIn Ro
	Call InstallKB2757011
	Call RebootIfRequired
SectionEnd

; 8 prerequisities
Section "$(SectionSSU)" WIN8SSU
	SectionIn Ro
	Call InstallKB4598297
	Call RebootIfRequired
SectionEnd

; 8.1 prerequisities
Section "Windows 8.1 $(Update) 1" WIN81U1
	SectionIn Ro
	Call InstallKB3021910
	Call InstallClearCompressionFlag
	Call InstallKB2919355
	Call InstallKB2932046
	Call InstallKB2959977
	Call InstallKB2937592
	Call InstallKB2934018
	Call RebootIfRequired
SectionEnd

Section "$(SectionSSU)" WIN81SSU
	SectionIn Ro
	Call InstallKB3021910
	Call RebootIfRequired
SectionEnd

; Shared prerequisites
Section "$(SectionWUA)" WUA
	SectionIn Ro
	Call InstallWUA
SectionEnd

${MementoSection} "$(SectionRootCerts)" ROOTCERTS
	Call ConfigureCrypto

	${IfNot} ${IsPostInstall}
		Call UpdateRoots
	${EndIf}
${MementoSectionEnd}

${MementoSection} "$(SectionEnableMU)" WIN7MU
	LegacyUpdateNSIS::EnableMicrosoftUpdate
	Pop $0
	${If} $0 != 0
		LegacyUpdateNSIS::MessageForHresult $0
		Pop $0
		MessageBox MB_USERICON "$(MsgBoxMUFailed)" /SD IDOK
	${EndIf}
	!insertmacro RestartWUAUService
${MementoSectionEnd}

${MementoSection} "$(SectionActivate)" ACTIVATE
	; No-op; we'll launch the activation wizard in post-install.
${MementoSectionEnd}

Section - PREREQS_END
SectionEnd

; Main installation
${MementoSection} "$(^Name)" LEGACYUPDATE
	; WUSERVER section
	Call MakeUninstallEntry

	${If} ${AtMostWinVista}
		; Check if Schannel is going to work with modern TLS
		${If} ${AtLeastWinVista}
			!insertmacro DetailPrint "$(StatusCheckingSSL)"
			!insertmacro DownloadRequest "${WSUS_SERVER_HTTPS}/ClientWebService/ping.bin" NONE \
				`/TIMEOUTCONNECT 0 /TIMEOUTRECONNECT 0`
			Pop $0
			Call DownloadWaitSilent
			Pop $0
			Pop $0
		${Else}
			StrCpy $0 ""
		${EndIf}

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
		WriteRegDword HKLM "${REGPATH_WUAUPOLICY}" "UseWUServer" 1

		; Restart service
		!insertmacro RestartWUAUService
	${EndIf}

	; ACTIVEX section
	SetOutPath $INSTDIR
	; Call MakeUninstallEntry

	; Add Control Panel entry
	; Category 5:  XP Performance and Maintenance, Vista System and Maintenance, 7+ System and Security
	; Category 10: XP SP2 Security Center, Vista Security, 7+ System and Security
	WriteRegStr   HKCR "${REGPATH_HKCR_CPLCLSID}"                    ""                "${NAME}"
	WriteRegStr   HKCR "${REGPATH_HKCR_CPLCLSID}"                    "LocalizedString" '@"$OUTDIR\LegacyUpdate.exe",-2'
	WriteRegStr   HKCR "${REGPATH_HKCR_CPLCLSID}"                    "InfoTip"         '@"$OUTDIR\LegacyUpdate.exe",-4'
	WriteRegStr   HKCR "${REGPATH_HKCR_CPLCLSID}\DefaultIcon"        ""                '"$OUTDIR\LegacyUpdate.exe",-100'
	WriteRegStr   HKCR "${REGPATH_HKCR_CPLCLSID}\Shell\Open\Command" ""                '"$OUTDIR\LegacyUpdate.exe"'
	WriteRegDword HKCR "${REGPATH_HKCR_CPLCLSID}\ShellFolder"        "Attributes"      0
	WriteRegDword HKCR "${REGPATH_HKCR_CPLCLSID}" "{305CA226-D286-468e-B848-2B2E8E697B74} 2" 5
	WriteRegStr   HKCR "${REGPATH_HKCR_CPLCLSID}" "System.ApplicationName"             "${CPL_APPNAME}"
	WriteRegStr   HKCR "${REGPATH_HKCR_CPLCLSID}" "System.ControlPanelCategory"        "5,10"
	WriteRegStr   HKCR "${REGPATH_HKCR_CPLCLSID}" "System.Software.TasksFileUrl"       '"$OUTDIR\LegacyUpdate.exe",-202'

	WriteRegStr   HKLM "${REGPATH_CPLNAMESPACE}" "" "${NAME}"

	; Install DLLs
	LegacyUpdateNSIS::CloseIEWindows

	; NOTE: Here we specifically check for amd64, because the DLL is amd64.
	; We still install to native Program Files on IA64, but with x86 binaries.
	File "..\${VSBUILD32}\LegacyUpdate.dll"
	${If} ${IsNativeAMD64}
		${If} ${FileExists} "LegacyUpdate32.dll"
			Delete "LegacyUpdate32.dll"
		${EndIf}
		Rename "LegacyUpdate.dll" "LegacyUpdate32.dll"
		File "..\x64\${VSBUILD64}\LegacyUpdate.dll"
	${EndIf}
	Call CopyLauncher

	; Register DLLs
	ExecWait '"$OUTDIR\LegacyUpdate.exe" /regserver $HWNDPARENT' $0
	${If} $0 != 0
		Abort
	${EndIf}

	; Create shortcut
	CreateShortcut "$COMMONSTARTMENU\${NAME}.lnk" \
		'"$OUTDIR\LegacyUpdate.exe"' '' \
		"$OUTDIR\LegacyUpdate.exe" 0 \
		SW_SHOWNORMAL "" \
		'@"$OUTDIR\LegacyUpdate.exe",-4'

	; Hide WU shortcuts
	${If} ${AtMostWinXP2003}
		${If} ${FileExists} "$COMMONSTARTMENU\Windows Update.lnk"
			CreateDirectory "$OUTDIR\Backup"
			Rename "$COMMONSTARTMENU\Windows Update.lnk" "$OUTDIR\Backup\Windows Update.lnk"
		${EndIf}

		${If} ${FileExists} "$COMMONSTARTMENU\Microsoft Update.lnk"
			CreateDirectory "$OUTDIR\Backup"
			Rename "$COMMONSTARTMENU\Microsoft Update.lnk" "$OUTDIR\Backup\Microsoft Update.lnk"
		${EndIf}
	${EndIf}

	; Add to trusted sites
	WriteRegDword HKLM "${REGPATH_ZONEDOMAINS}\${DOMAIN}"    "http"  2
	WriteRegDword HKLM "${REGPATH_ZONEDOMAINS}\${DOMAIN}"    "https" 2
	WriteRegDword HKLM "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}" "http"  2
	WriteRegDword HKLM "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}" "https" 2

	; Delete LegacyUpdate.dll in System32 from 1.0 installer
	${If} ${FileExists} $WINDIR\System32\LegacyUpdate.dll
		Delete $WINDIR\System32\LegacyUpdate.dll
	${EndIf}

	; Delete LegacyUpdate.inf from 1.0 installer
	${If} ${FileExists} $WINDIR\inf\LegacyUpdate.inf
		Delete $WINDIR\inf\LegacyUpdate.inf
	${EndIf}

	; If 32-bit Legacy Update exists, move it to 64-bit Program Files
	${If} ${RunningX64}
	${AndIf} ${FileExists} "$PROGRAMFILES32\Legacy Update\Backup"
		CreateDirectory "$PROGRAMFILES64\Legacy Update"
		Rename "$PROGRAMFILES32\Legacy Update\Backup" "$PROGRAMFILES64\Legacy Update\Backup"
		RMDir /r "$PROGRAMFILES32\Legacy Update"
	${EndIf}
${MementoSectionEnd}

${MementoSectionDone}

; Uninstaller
Section "-un.Legacy Update Server" un.WUSERVER
	; Clear WSUS server
	${If} ${AtMostWinVista}
		ReadRegStr $0 HKLM "${REGPATH_WUPOLICY}" "WUServer"
		${If} $0 == "${WSUS_SERVER}"
		${OrIf} $0 == "${WSUS_SERVER_HTTPS}"
			DeleteRegValue HKLM "${REGPATH_WUPOLICY}"   "WUServer"
			DeleteRegValue HKLM "${REGPATH_WUAUPOLICY}" "UseWUStatusServer"
		${EndIf}

		ReadRegStr $0 HKLM "${REGPATH_WUPOLICY}" "WUStatusServer"
		${If} $0 == "${WSUS_SERVER}"
		${OrIf} $0 == "${WSUS_SERVER_HTTPS}"
			DeleteRegValue HKLM "${REGPATH_WUPOLICY}"   "WUStatusServer"
			DeleteRegValue HKLM "${REGPATH_WUAUPOLICY}" "UseWUStatusServer"
		${EndIf}

		ReadRegDword $0 HKLM "${REGPATH_WUAUPOLICY}" "UseWUServer"

		DeleteRegValue HKLM "${REGPATH_WUPOLICY}"   "WUServer"
		DeleteRegValue HKLM "${REGPATH_WUPOLICY}"   "WUStatusServer"
		DeleteRegValue HKLM "${REGPATH_WU}"         "URL"
	${EndIf}
SectionEnd

Section "-un.Legacy Update website" un.ACTIVEX
	SetOutPath $INSTDIR

	; Delete shortcut
	Delete "$COMMONSTARTMENU\${NAME}.lnk"

	; Delete Control Panel entry
	DeleteRegKey HKLM "${REGPATH_CPLNAMESPACE}"
	DeleteRegKey HKCR "${REGPATH_HKCR_CPLCLSID}"

	; Restore shortcuts
	${If} ${FileExists} "$OUTDIR\Backup\Windows Update.lnk"
		Rename "$OUTDIR\Backup\Windows Update.lnk" "$COMMONSTARTMENU\Windows Update.lnk"
	${EndIf}

	${If} ${FileExists} "$OUTDIR\Backup\Microsoft Update.lnk"
		Rename "$OUTDIR\Backup\Microsoft Update.lnk" "$COMMONSTARTMENU\Microsoft Update.lnk"
	${EndIf}

	; Unregister DLLs
	ExecWait '"$OUTDIR\LegacyUpdate.exe" /unregserver $HWNDPARENT' $0
	${If} $0 != 0
		Abort
	${EndIf}

	; Delete files
	Delete "$OUTDIR\LegacyUpdate.exe"
	Delete "$OUTDIR\LegacyUpdate.dll"
	Delete "$OUTDIR\LegacyUpdate32.dll"

	; Remove from trusted sites
	DeleteRegKey HKCU "${REGPATH_ZONEDOMAINS}\${DOMAIN}"
	DeleteRegKey HKLM "${REGPATH_ZONEDOMAINS}\${DOMAIN}"
	DeleteRegKey HKCU "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}"
	DeleteRegKey HKLM "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}"

	; Restart service
	!insertmacro RestartWUAUService
SectionEnd

Section -Uninstall
	SetOutPath $INSTDIR

	; Delete folders
	RMDir /r "$OUTDIR"
	RMDir /r /REBOOTOK "${RUNONCEDIR}"

	; Delete uninstall entry
	DeleteRegKey HKLM "${REGPATH_UNINSTSUBKEY}"
SectionEnd

!define DESCRIPTION_REBOOTS ""
!define DESCRIPTION_SUPEULA ""
!define DESCRIPTION_MSLT    ""

!macro DESCRIPTION_STRING section
	!insertmacro MUI_DESCRIPTION_TEXT ${${section}} "$(Section${section}Desc)"
!macroend

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro DESCRIPTION_STRING W2KSP4
	!insertmacro DESCRIPTION_STRING IE6SP1
	!insertmacro DESCRIPTION_STRING XPSP3
	!insertmacro DESCRIPTION_STRING XPESP3
	!insertmacro DESCRIPTION_STRING WES09
	!insertmacro DESCRIPTION_STRING 2003SP2
	!insertmacro DESCRIPTION_STRING VISTASP2
	!insertmacro DESCRIPTION_STRING VISTASSU
	!insertmacro DESCRIPTION_STRING VISTAIE9
	!insertmacro DESCRIPTION_STRING WIN7SP1
	!insertmacro DESCRIPTION_STRING WIN7SSU
	!insertmacro DESCRIPTION_STRING WIN8SSU
	!insertmacro DESCRIPTION_STRING WIN81U1
	!insertmacro DESCRIPTION_STRING WIN81SSU
	!insertmacro DESCRIPTION_STRING WHS2011U4
	!insertmacro DESCRIPTION_STRING WUA
	!insertmacro DESCRIPTION_STRING ROOTCERTS
	!insertmacro DESCRIPTION_STRING WIN7MU
	!insertmacro DESCRIPTION_STRING ACTIVATE
	; !insertmacro DESCRIPTION_STRING LEGACYUPDATE
	; !insertmacro DESCRIPTION_STRING WUSERVER
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function OnMouseOverSection
	${If} $0 == ${LEGACYUPDATE}
		${If} ${AtMostWinXP2003}
			StrCpy $0 "$(SectionActiveX2KXPDesc)"
		${ElseIf} ${AtMostWin8.1}
			StrCpy $0 "$(SectionActiveXVista78Desc)"
		${Else}
			StrCpy $0 "$(SectionActiveXWin10Desc)"
		${EndIf}
		SendMessage $mui.ComponentsPage.DescriptionText ${WM_SETTEXT} 0 "STR:"
		EnableWindow $mui.ComponentsPage.DescriptionText 1
		SendMessage $mui.ComponentsPage.DescriptionText ${WM_SETTEXT} 0 "STR:$0"
	${EndIf}
FunctionEnd

Function .onInit
	${If} ${IsHelp}
		MessageBox MB_USERICON "$(MsgBoxUsage)"
		Quit
	${EndIf}

	SetShellVarContext all
	SetDetailsPrint listonly
	${If} "$PROGRAMFILES64" != "$PROGRAMFILES32"
		SetRegView 64
	${EndIf}
	!insertmacro EnsureAdminRights

	${If} ${IsRunOnce}
	${OrIf} ${IsPostInstall}
		Call OnRunOnceLogon
	${ElseIfNot} ${AtLeastWin10}
		GetWinVer $0 Build
		ReadRegDword $1 HKLM "${REGPATH_CONTROL_WINDOWS}" "CSDVersion"
		IntOp $1 $1 & 0xFF
		${If} $1 != 0
			StrCpy $1 1
		${EndIf}

		${If} $0 != ${WINVER_BUILD_2000}
		${AndIf} $0 != ${WINVER_BUILD_XP2002}
		${AndIf} $0 != ${WINVER_BUILD_XP2003}
		${AndIf} $0 != ${WINVER_BUILD_VISTA}
		${AndIf} $0 != ${WINVER_BUILD_VISTA_SP1}
		${AndIf} $0 != ${WINVER_BUILD_VISTA_SP2}
		${AndIf} $0 != ${WINVER_BUILD_VISTA_ESU}
		${AndIf} $0 != ${WINVER_BUILD_7}
		${AndIf} $0 != ${WINVER_BUILD_7_SP1}
		${AndIf} $0 != ${WINVER_BUILD_8}
		${AndIf} $0 != ${WINVER_BUILD_8.1}
		${AndIf} $0 != ${WINVER_BUILD_10}
			StrCpy $1 1
		${EndIf}

		${If} $1 == 1
			MessageBox MB_USERICON|MB_OKCANCEL "$(MsgBoxBetaOS)" /SD IDOK \
				IDOK +2
			Quit
		${EndIf}
	${EndIf}

	; Check for compatibility mode (GetVersionEx() and RtlGetNtVersionNumbers() disagreeing)
	GetWinVer $0 Major
	GetWinVer $1 Minor
	GetWinVer $2 Build
	System::Call '${RtlGetNtVersionNumbers}(.r3, .r4, .r5)'
	IntOp $5 $5 & 0xFFFF

	; Detect NNN4NT5
	ReadEnvStr $6 "_COMPAT_VER_NNN"
	${If} $6 != ""
		StrCpy $3 "?"
	${EndIf}

	; Windows 2000 lacks RtlGetNtVersionNumbers(), but there is no compatibility mode anyway.
	${If} "$3.$4.$5" != "0.0.0"
	${AndIf} "$0.$1.$2" != "$3.$4.$5"
		MessageBox MB_USERICON "$(MsgBoxCompatMode)" /SD IDOK
		SetErrorLevel 1
		Quit
	${EndIf}

	SetOutPath $PLUGINSDIR
	File Patches.ini

	SetOutPath "${RUNONCEDIR}"

	${MementoSectionRestore}

	${If} ${IsWin2000}
		; Determine whether Win2k prereqs need to be installed
		${IfNot} ${NeedsPatch} W2KSP4
		${AndIfNot} ${NeedsPatch} KB835732
			!insertmacro RemoveSection ${W2KSP4}
		${EndIf}

		${IfNot} ${NeedsPatch} IE6
			!insertmacro RemoveSection ${IE6SP1}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${W2KSP4}
		!insertmacro RemoveSection ${IE6SP1}
	${EndIf}

	${If} ${IsWinXP2002}
		${If} ${IsEmbedded}
			; Determine whether XP Embedded prereqs need to be installed
			; Windows XP Embedded (version 2001), including FLP and WEPOS, has a different service pack
			!insertmacro RemoveSection ${XPSP3}

			${IfNot} ${NeedsPatch} XPESP3
				!insertmacro RemoveSection ${XPESP3}
			${EndIf}
		${Else}
			; Determine whether XP prereqs need to be installed
			!insertmacro RemoveSection ${XPESP3}

			${IfNot} ${NeedsPatch} XPSP3
				!insertmacro RemoveSection ${XPSP3}
			${EndIf}
		${EndIf}

		ReadRegDword $0 HKLM "${REGPATH_POSREADY}" "Installed"
		${If} $0 == 1
			!insertmacro RemoveSection ${WES09}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${XPSP3}
		!insertmacro RemoveSection ${XPESP3}
		!insertmacro RemoveSection ${WES09}
	${EndIf}

	${If} ${IsWinXP2003}
		; Determine whether 2003 prereqs need to be installed
		${IfNot} ${NeedsPatch} 2003SP2
			!insertmacro RemoveSection ${2003SP2}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${2003SP2}
	${EndIf}

	${If} ${IsWinVista}
		; Determine whether Vista prereqs need to be installed
		${IfNot} ${NeedsPatch} VistaSP2
			!insertmacro RemoveSection ${VISTASP2}
		${EndIf}

		${IfNot} ${NeedsPatch} VistaPostSP2
			!insertmacro RemoveSection ${VISTASSU}
		${EndIf}

		${IfNot} ${NeedsPatch} IE9
			!insertmacro RemoveSection ${VISTAIE9}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${VISTASP2}
		!insertmacro RemoveSection ${VISTASSU}
		!insertmacro RemoveSection ${VISTAIE9}
	${EndIf}

	${If} ${IsWin7}
		; Determine whether 7 prereqs need to be installed
		${IfNot} ${NeedsPatch} Win7SP1
			!insertmacro RemoveSection ${WIN7SP1}
		${EndIf}

		${IfNot} ${IsHomeServer}
		${OrIfNot} ${NeedsPatch} KB2757011
			!insertmacro RemoveSection ${WHS2011U4}
		${EndIf}

		${IfNot} ${NeedsPatch} Win7PostSP1
			!insertmacro RemoveSection ${WIN7SSU}
		${EndIf}

		ClearErrors
		EnumRegKey $0 HKLM "${REGPATH_WU_SERVICES}\${WU_MU_SERVICE_ID}" 0
		${IfNot} ${Errors}
			!insertmacro RemoveSection ${WIN7MU}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${WIN7SP1}
		!insertmacro RemoveSection ${WHS2011U4}
		!insertmacro RemoveSection ${WIN7SSU}
		!insertmacro RemoveSection ${WIN7MU}
	${EndIf}

	${If} ${IsWin8}
		; Determine whether 8 prereqs need to be installed
		${IfNot} ${NeedsPatch} KB4598297
			!insertmacro RemoveSection ${WIN8SSU}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${WIN8SSU}
	${EndIf}

	${If} ${IsWin8.1}
		; Determine whether 8.1 prereqs need to be installed
		${IfNot} ${NeedsPatch} Win81Update1
			!insertmacro RemoveSection ${WIN81U1}
		${EndIf}

		${IfNot} ${NeedsPatch} KB3021910
			!insertmacro RemoveSection ${WIN81SSU}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${WIN81U1}
		!insertmacro RemoveSection ${WIN81SSU}
	${EndIf}

	Call DetermineWUAVersion
	${If} $0 == ""
		!insertmacro RemoveSection ${WUA}
	${EndIf}

	${If} ${AtLeastWinXP2002}
	${AndIf} ${AtMostWin8.1}
		; Check if the OS needs activation
		LegacyUpdateNSIS::IsActivated
		Pop $0
		${If} $0 == 1
			!insertmacro RemoveSection ${ACTIVATE}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${ACTIVATE}
	${EndIf}

	; ${IfNot} ${AtMostWinVista}
	; 	!insertmacro RemoveSection ${LEGACYUPDATE}
	; ${EndIf}

	; Try not to be too intrusive on Windows 10 and newer, which are (for now) fine
	${If} ${AtLeastWin10}
		!insertmacro RemoveSection ${ROOTCERTS}
	${EndIf}
FunctionEnd

Function ComponentsPageCheck
	; Skip the page if we're being launched with /runonce, /postinstall, or /passive
	${If} ${IsRunOnce}
	${OrIf} ${IsPostInstall}
	${OrIf} ${IsPassive}
		Abort
	${EndIf}

	; Skip if installer was invoked by IE, and all prerequisites are installed
	${If} ${IsActiveX}
	${AndIf} ${SectionIsSelected} ${LEGACYUPDATE}
		StrCpy $1 0
		${For} $0 ${PREREQS_START} ${PREREQS_END}
			${If} ${SectionIsSelected} $0
			${AndIf} $0 != ${PREREQS_START}
			${AndIf} $0 != ${PREREQS_END}
			${AndIf} $0 != ${LEGACYUPDATE}
			${AndIf} $0 != ${ROOTCERTS}
				StrCpy $1 1
				${Break}
			${EndIf}
		${Next}

		${If} $1 == 0
			Abort
		${EndIf}
	${EndIf}
FunctionEnd

Function PreDownload
	; Win2k
	${If} ${IsWin2000}
		Call DownloadW2KSP4
		Call DownloadKB835732
		Call DownloadIE6
	${EndIf}

	; XP 2002
	${If} ${IsWinXP2002}
	${AndIf} ${SectionIsSelected} ${XPSP3}
		Call DownloadXPSP1a
		Call DownloadXPSP3
	${EndIf}

	${If} ${IsWinXP2002}
	${AndIf} ${SectionIsSelected} ${XPESP3}
		Call DownloadXPESP3
	${EndIf}

	; XP 2003
	${If} ${IsWinXP2003}
	${AndIf} ${SectionIsSelected} ${2003SP2}
		Call Download2003SP2
	${EndIf}

	; Vista
	${If} ${IsWinVista}
		${If} ${NeedsPatch} VistaSP2
		${AndIfNot} ${IsPassive}
			MessageBox MB_USERICON "$(MsgBoxVistaSPInstall)" /SD IDOK
		${EndIf}

		Call DownloadVistaSP1
		Call DownloadVistaSP2
		Call DownloadKB3205638
		Call DownloadKB4012583
		Call DownloadKB4015195
		Call DownloadKB4015380
		Call DownloadKB4493730

		${If} ${SectionIsSelected} ${VISTAIE9}
			Call DownloadKB971512
			Call DownloadKB2117917
			Call DownloadIE9
		${EndIf}
	${EndIf}

	; 7
	${If} ${IsWin7}
		${If} ${IsHomeServer}
			Call DownloadKB2757011
		${Else}
			Call DownloadWin7SP1
		${EndIf}

		Call DownloadKB3138612
		Call DownloadKB4474419
		Call DownloadKB4490628
	${EndIf}

	; 8
	${If} ${IsWin8}
		Call DownloadKB4598297
	${EndIf}

	; 8.1
	${If} ${IsWin8.1}
		Call DownloadKB3021910
		Call DownloadClearCompressionFlag
		Call DownloadKB2919355
		Call DownloadKB2932046
		Call DownloadKB2959977
		Call DownloadKB2937592
		Call DownloadKB2934018
		Call DownloadKB3021910
	${EndIf}

	; General
	Call DownloadWUA

	${If} ${AtMostWin8.1}
	${AndIf} ${SectionIsSelected} ${ROOTCERTS}
		Call DownloadRoots
	${EndIf}
FunctionEnd

Function PostInstall
	; Handle first run flag if needed
	${If} ${FileExists} "$INSTDIR\LegacyUpdate.exe"
		ClearErrors
		ReadRegDword $0 HKLM "${REGPATH_LEGACYUPDATE_SETUP}" "ActiveXInstalled"
		${If} ${Errors}
			StrCpy $0 "/firstrun"
		${Else}
			StrCpy $0 ""
		${EndIf}
		WriteRegDword HKLM "${REGPATH_LEGACYUPDATE_SETUP}" "ActiveXInstalled" 1
	${EndIf}

	${IfNot} ${Silent}
	${AndIfNot} ${IsRunOnce}
		${If} ${FileExists} "$INSTDIR\LegacyUpdate.exe"
			Exec '"$INSTDIR\LegacyUpdate.exe" /launch $0'
		${ElseIf} ${AtLeastWin10}
			ExecShell "" "ms-settings:windowsupdate"
		${ElseIf} ${AtLeastWinVista}
			Exec '"$WINDIR\system32\wuauclt.exe" /ShowWUAutoScan'
		${EndIf}

		; Launch activation wizard if requested by the user
		${If} ${SectionIsSelected} ${ACTIVATE}
			${DisableX64FSRedirection}
			${If} ${AtLeastWinVista}
				Exec '"$WINDIR\system32\slui.exe"'
			${Else}
				Exec '"$WINDIR\system32\oobe\msoobe.exe" /a'
			${EndIf}
			${EnableX64FSRedirection}
		${EndIf}
	${EndIf}
FunctionEnd

Function CleanUp
	; Called only after all tasks have completed
	Call CleanUpRunOnce
	!insertmacro InhibitSleep 0

	${If} ${IsRunOnce}
		Call OnRunOnceDone
	${EndIf}

	${If} ${IsPostInstall}
	${OrIfNot} ${RebootFlag}
		Call CleanUpRunOnceFinal
	${EndIf}
FunctionEnd

Function .onInstSuccess
	${MementoSectionSave}

	; Reboot now if we need to. Nothing further in this function will be run if we do need to reboot.
	Call RebootIfRequired

	; If we're done, launch the update site
	Call PostInstall
	Call CleanUp
FunctionEnd

Function .onInstFailed
	${MementoSectionSave}
	Call CleanUp
FunctionEnd

Function .onSelChange
	${If} ${SectionIsSelected} ${WES09}
		; Check for SSE2.
		System::Call '${IsProcessorFeaturePresent}(${PF_XMMI64_INSTRUCTIONS_AVAILABLE}) .r0'
		${If} $0 == 0
			MessageBox MB_USERICON "$(MsgBoxWES09NotSSE2)" /SD IDOK
			!insertmacro UnselectSection ${WES09}
		${EndIf}
	${ElseIf} ${SectionIsSelected} ${ACTIVATE}
		; Make sure the service pack prerequisite is selected
		${If} ${IsWinXP2002}
		${AndIfNot} ${AtLeastServicePack} 3
		${AndIfNot} ${SectionIsSelected} ${XPSP3}
			MessageBox MB_USERICON "$(MsgBoxActivateXP2002NotSP3)" /SD IDOK
			!insertmacro SelectSection ${XPSP3}
		${ElseIf} ${IsWinXP2003}
		${AndIfNot} ${AtLeastServicePack} 2
		${AndIfNot} ${SectionIsSelected} ${2003SP2}
			MessageBox MB_USERICON "$(MsgBoxActivateXP2003NotSP2)" /SD IDOK
			!insertmacro SelectSection ${2003SP2}
		${EndIf}
	${EndIf}
FunctionEnd

Function un.onInit
	SetShellVarContext all
	SetDetailsPrint listonly

	; Hack to avoid bundling System.dll in uninstaller
	${If} "$PROGRAMFILES64" != "$PROGRAMFILES32"
		SetRegView 64
	${EndIf}
FunctionEnd

Function un.onUninstSuccess
	Quit
FunctionEnd
