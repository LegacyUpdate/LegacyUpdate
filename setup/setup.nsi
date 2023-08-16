!include Constants.nsh

Name         "${NAME}"
Caption      "${NAME}"
BrandingText "${NAME} ${VERSION} - ${DOMAIN}"
OutFile      "LegacyUpdate-${VERSION}.exe"
InstallDir   "$PROGRAMFILES\${NAME}"
InstallDirRegKey HKLM "${REGPATH_LEGACYUPDATE_SETUP}" "InstallDir"

Unicode               true
RequestExecutionLevel Admin
AutoCloseWindow       true
ManifestSupportedOS   all
ManifestDPIAware      true
AllowSkipFiles        off
SetCompressor         /SOLID lzma

VIAddVersionKey /LANG=1033 "ProductName"     "${NAME}"
VIAddVersionKey /LANG=1033 "ProductVersion"  "${LONGVERSION}"
VIAddVersionKey /LANG=1033 "CompanyName"     "Hashbang Productions"
VIAddVersionKey /LANG=1033 "LegalCopyright"  "© Hashbang Productions. All rights reserved."
VIAddVersionKey /LANG=1033 "FileDescription" "${NAME}"
VIAddVersionKey /LANG=1033 "FileVersion"     "${LONGVERSION}"
VIProductVersion ${LONGVERSION}
VIFileVersion    ${LONGVERSION}

!define MUI_UI                       "modern_aerowizard.exe"
!define MUI_UI_HEADERIMAGE           "modern_aerowizard.exe"

!define MUI_CUSTOMFUNCTION_GUIINIT   OnShow
!define MUI_CUSTOMFUNCTION_UNGUIINIT un.OnShow
!define MUI_CUSTOMFUNCTION_ABORT     CleanUp

!define MUI_ICON                     "..\LegacyUpdate\icon.ico"
!define MUI_UNICON                   "..\LegacyUpdate\icon.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP       "setupbanner.bmp"
!define MUI_HEADERIMAGE_UNBITMAP     "setupbanner.bmp"

!define MUI_TEXT_ABORT_TITLE         "Installation Failed"

!define MEMENTO_REGISTRY_ROOT        HKLM
!define MEMENTO_REGISTRY_KEY         "${REGPATH_LEGACYUPDATE_SETUP}"

Var /GLOBAL InstallDir
Var /GLOBAL RunOnceDir

!include FileFunc.nsh
!include Integration.nsh
!include LogicLib.nsh
!include Memento.nsh
!include MUI2.nsh
!include Sections.nsh
!include Win\COM.nsh
!include Win\WinError.nsh
!include Win\WinNT.nsh
!include WinMessages.nsh
!include WinCore.nsh
!include WinVer.nsh
!include WordFunc.nsh
!include x64.nsh

!include Common.nsh
!include AeroWizard.nsh
!include Download2KXP.nsh
!include DownloadVista7.nsh
!include Download8.nsh
!include DownloadWUA.nsh
!include EnableMU.nsh
!include RunOnce.nsh
!include UpdateRoots.nsh

!insertmacro GetParameters
!insertmacro GetOptions

!define MUI_PAGE_HEADER_TEXT         "Welcome to Legacy Update"
!define MUI_COMPONENTSPAGE_TEXT_TOP  "Select what you would like Legacy Update to do. An internet connection is required to download additional components from Microsoft. Your computer will restart automatically if needed. Close all other programs before continuing."
!define MUI_PAGE_CUSTOMFUNCTION_PRE  ComponentsPageCheck
!define MUI_PAGE_CUSTOMFUNCTION_SHOW OnShow
!define MUI_PAGE_FUNCTION_GUIINIT    OnShow
!define MUI_CUSTOMFUNCTION_ONMOUSEOVERSECTION OnMouseOverSection

!insertmacro MUI_PAGE_COMPONENTS

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

!insertmacro MUI_LANGUAGE "English"

!macro RestartWUAUService
	!insertmacro DetailPrint "Restarting Windows Update service..."
	SetDetailsPrint none
	ExecShellWait "" "$WINDIR\system32\net.exe" "stop wuauserv" SW_HIDE
	ExecShellWait "" "$WINDIR\system32\net.exe" "start wuauserv" SW_HIDE
	SetDetailsPrint listonly
!macroend

Function ComponentsPageCheck
	; Skip the page if we're being launched via RunOnce
	${If} ${IsRunOnce}
	${OrIf} ${IsPostInstall}
		Abort
	${EndIf}
FunctionEnd

Function OnShow
	Call AeroWizardOnShow
FunctionEnd

Function un.OnShow
	Call un.AeroWizardOnShow
FunctionEnd

Section -BeforeInstall
	!insertmacro InhibitSleep 1
SectionEnd

Section -PreDownload
	${IfNot} ${IsPostInstall}
		Call PreDownload
	${EndIf}
SectionEnd

; Win2k prerequisities
Section "Windows 2000 Service Pack 4" W2KSP4
	SectionIn Ro
	Call InstallW2KSP4
	Call InstallKB835732
	Call RebootIfRequired
SectionEnd

Section "Internet Explorer 6.0 Service Pack 1" IE6SP1
	SectionIn Ro
	Call InstallIE6
	Call RebootIfRequired
SectionEnd

; XP 2002 prerequisities
${MementoSection} "Windows XP Service Pack 3" XPSP3
	Call InstallXPSP2
	Call RebootIfRequired
	Call InstallXPSP3
	Call RebootIfRequired
${MementoSectionEnd}

; XP 2003 prerequisities
${MementoSection} "Windows XP/Server 2003 Service Pack 2" 2003SP2
	Call Install2003SP2
	Call RebootIfRequired
${MementoSectionEnd}

; Vista prerequisities
Section "Windows Vista Service Pack 2" VISTASP2
	SectionIn Ro
	Call InstallVistaSP1
	Call RebootIfRequired
	Call InstallVistaSP2
	Call RebootIfRequired
SectionEnd

Section "Windows Servicing Stack update" VISTASSU
	SectionIn Ro
	Call InstallKB3205638
	Call InstallKB4012583
	Call InstallKB4015195
	Call InstallKB4015380
	Call InstallKB4493730
	Call RebootIfRequired
SectionEnd

${MementoSection} "Internet Explorer 9" VISTAIE9
	Call InstallKB971512
	Call InstallKB2117917
	Call RebootIfRequired
	Call InstallIE9
	Call RebootIfRequired
${MementoSectionEnd}

; 7 prerequisities
Section "Windows 7 Service Pack 1" WIN7SP1
	SectionIn Ro
	Call InstallWin7SP1
	Call RebootIfRequired
SectionEnd

Section "Windows Servicing Stack update" WIN7SSU
	SectionIn Ro
	Call InstallKB4474419
	Call InstallKB4490628
	Call RebootIfRequired
SectionEnd

; 8 prerequisities
Section "Windows Servicing Stack update" WIN8SSU
	SectionIn Ro
	Call InstallKB4598297
	Call RebootIfRequired
SectionEnd

Section "Windows 8.1" WIN81UPGRADE
	; No-op; we'll launch the support site in post-install.
SectionEnd

; 8.1 prerequisities
Section "Windows 8.1 Update 1" WIN81UPDATE1
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

Section "Windows Servicing Stack update" WIN81SSU
	SectionIn Ro
	Call InstallKB3021910
	Call RebootIfRequired
SectionEnd

; Shared prerequisites
Section "Windows Update Agent update" WUA
	SectionIn Ro
	Call InstallWUA
SectionEnd

${MementoUnselectedSection} "Enable Windows Embedded 2009 updates" WES09
	WriteRegDword HKLM "${REGPATH_POSREADY}" "Installed" 1
${MementoSectionEnd}

${MementoSection} "Update root certificates store" ROOTCERTS
	Call ConfigureCrypto
	Call UpdateRoots
${MementoSectionEnd}

${MementoSection} "Enable Microsoft Update" WIN7MU
	Call EnableMicrosoftUpdate
	!insertmacro RestartWUAUService
${MementoSectionEnd}

; Main installation
${MementoSection} "Legacy Update" LEGACYUPDATE
	SetOutPath $InstallDir
	WriteUninstaller "Uninstall.exe"

	; Add uninstall entry
	WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayName" "${NAME}"
	WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayIcon" '"$OUTDIR\Uninstall.exe",0'
	WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayVersion" "${VERSION}"
	WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "Publisher" "${NAME}"
	WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "URLInfoAbout" "${WEBSITE}"
	WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "UninstallString" '"$OUTDIR\Uninstall.exe"'
	WriteRegStr   HKLM "${REGPATH_UNINSTSUBKEY}" "QuietUninstallString" '"$OUTDIR\Uninstall.exe" /S'
	WriteRegDword HKLM "${REGPATH_UNINSTSUBKEY}" "NoModify" 1
	WriteRegDword HKLM "${REGPATH_UNINSTSUBKEY}" "NoRepair" 1

	; Add Control Panel entry
	; Category 5:  XP Performance and Maintenance, Vista System and Maintenance, 7+ System and Security
	; Category 10: XP SP2 Security Center, Vista Security, 7+ System and Security
	WriteRegStr   HKCR "${REGPATH_CPLCLSID}" "" "${NAME}"
	WriteRegStr   HKCR "${REGPATH_CPLCLSID}" "LocalizedString" '@"$OUTDIR\LegacyUpdate.dll",-2'
	WriteRegStr   HKCR "${REGPATH_CPLCLSID}" "InfoTip" '@"$OUTDIR\LegacyUpdate.dll",-4'
	WriteRegStr   HKCR "${REGPATH_CPLCLSID}\DefaultIcon" "" '"$OUTDIR\LegacyUpdate.dll",0'
	WriteRegStr   HKCR "${REGPATH_CPLCLSID}\Shell\Open\Command" "" 'rundll32.exe "$OUTDIR\LegacyUpdate.dll",LaunchUpdateSite'
	WriteRegDword HKCR "${REGPATH_CPLCLSID}\ShellFolder" "Attributes" 0
	WriteRegDword HKCR "${REGPATH_CPLCLSID}" "{305CA226-D286-468e-B848-2B2E8E697B74} 2" 5
	WriteRegStr   HKCR "${REGPATH_CPLCLSID}" "System.ApplicationName" "${CPL_APPNAME}"
	WriteRegStr   HKCR "${REGPATH_CPLCLSID}" "System.ControlPanelCategory" "5,10"
	WriteRegStr   HKCR "${REGPATH_CPLCLSID}" "System.Software.TasksFileUrl" "$OUTDIR\LegacyUpdate.dll,-202"
	WriteRegStr   HKLM "${REGPATH_CPLNAMESPACE}" "" "${NAME}"

	; Install DLL, with detection for it being in use by IE
	; NOTE: Here we specifically check for amd64, because the DLL is amd64.
	; We still install to native Program Files on IA64, but with x86 binaries.
	SetOverwrite try
	!insertmacro TryFile "..\Release\LegacyUpdate.dll" "$OUTDIR\LegacyUpdate.dll"
	${If} ${IsNativeAMD64}
		${If} ${FileExists} "$OUTDIR\LegacyUpdate32.dll"
			!insertmacro TryDelete "$OUTDIR\LegacyUpdate32.dll"
		${EndIf}
		!insertmacro TryRename "$OUTDIR\LegacyUpdate.dll" "$OUTDIR\LegacyUpdate32.dll"
		!insertmacro TryFile "..\x64\Release\LegacyUpdate.dll" "$OUTDIR\LegacyUpdate.dll"
	${EndIf}
	SetOverwrite on

	; Register DLL
	${If} ${IsNativeAMD64}
		!insertmacro RegisterDLL "" x64 "$OUTDIR\LegacyUpdate.dll"
		!insertmacro RegisterDLL "" x86 "$OUTDIR\LegacyUpdate32.dll"
	${Else}
		!insertmacro RegisterDLL "" x86 "$OUTDIR\LegacyUpdate.dll"
	${EndIf}

	; Create shortcut
	CreateShortcut "$COMMONSTARTMENU\${NAME}.lnk" \
		"$SYSDIR\rundll32.exe" '"$OUTDIR\LegacyUpdate.dll",LaunchUpdateSite' \
		"$OUTDIR\LegacyUpdate.dll" 0 \
		SW_SHOWNORMAL "" \
		'@"$OUTDIR\LegacyUpdate.dll",-4'

	; Hide WU shortcuts
	; TODO: How can we consistently find the shortcuts for non-English installs?
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
	WriteRegDword HKCU "${REGPATH_ZONEDOMAINS}\${DOMAIN}"    "http"  2
	WriteRegDword HKCU "${REGPATH_ZONEDOMAINS}\${DOMAIN}"    "https" 2
	WriteRegDword HKCU "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}" "http"  2
	WriteRegDword HKCU "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}" "https" 2

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

	; Set WSUS server
	${If} ${AtMostWinVista}
		; Check if Schannel is going to work with modern TLS
		!insertmacro DetailPrint "Checking SSL connectivity..."
		!insertmacro DownloadRequest "${WSUS_SERVER_HTTPS}/ClientWebService/ping.bin" NONE \
			`/TIMEOUTCONNECT 0 /TIMEOUTRECONNECT 0`
		Pop $0
		!insertmacro DownloadWait $0 SILENT
		Pop $0
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
		WriteRegDword HKLM "${REGPATH_WUAUPOLICY}" "UseWUServer" 1

		; Restart service
		!insertmacro RestartWUAUService
	${EndIf}
${MementoSectionEnd}

${MementoSection} "Activate Windows" ACTIVATE
	ExecShell "" "$WINDIR\system32\oobe\msoobe.exe" "/a"
${MementoSectionEnd}

${MementoSectionDone}

Section -Uninstall
	SetOutPath $InstallDir

	; Delete shortcut
	${UnpinShortcut} "$COMMONSTARTMENU\${NAME}.lnk"
	Delete "$COMMONSTARTMENU\${NAME}.lnk"

	; Delete Control Panel entry
	DeleteRegKey HKLM "${REGPATH_CPLNAMESPACE}"
	DeleteRegKey HKCR "${REGPATH_CPLCLSID}"

	; Restore shortcuts
	${If} ${FileExists} "$OUTDIR\Backup\Windows Update.lnk"
		Rename "$OUTDIR\Backup\Windows Update.lnk" "$COMMONSTARTMENU\Windows Update.lnk"
	${EndIf}

	${If} ${FileExists} "$OUTDIR\Backup\Microsoft Update.lnk"
		Rename "$OUTDIR\Backup\Microsoft Update.lnk" "$COMMONSTARTMENU\Microsoft Update.lnk"
	${EndIf}

	; Unregister DLLS
	${If} ${IsNativeAMD64}
		!insertmacro RegisterDLL Un x64 "$OUTDIR\LegacyUpdate.dll"
		!insertmacro RegisterDLL Un x86 "$OUTDIR\LegacyUpdate32.dll"
	${Else}
		!insertmacro RegisterDLL Un x86 "$OUTDIR\LegacyUpdate.dll"
	${EndIf}

	; Delete DLLs
	SetOverwrite try
	!insertmacro TryDelete "$OUTDIR\LegacyUpdate.dll"
	!insertmacro TryDelete "$OUTDIR\LegacyUpdate32.dll"
	SetOverwrite on

	; Clear WSUS server
	${If} ${AtMostWinVista}
		DeleteRegValue HKLM "${REGPATH_WUPOLICY}"   "WUServer"
		DeleteRegValue HKLM "${REGPATH_WUPOLICY}"   "WUStatusServer"
		DeleteRegValue HKLM "${REGPATH_WUAUPOLICY}" "UseWUStatusServer"
		DeleteRegValue HKLM "${REGPATH_WU}"         "URL"
	${EndIf}

	; Remove from trusted sites
	DeleteRegKey HKCU "${REGPATH_ZONEDOMAINS}\${DOMAIN}"
	DeleteRegKey HKCU "${REGPATH_ZONEESCDOMAINS}\${DOMAIN}"

	; Restart service
	!insertmacro RestartWUAUService

	; Delete folders
	RMDir /r "$OUTDIR"
	RMDir /r /REBOOTOK "$RunOnceDir"

	; Delete uninstall entry
	DeleteRegKey HKLM "${REGPATH_UNINSTSUBKEY}"
SectionEnd

!define DESCRIPTION_REBOOTS "Your computer will restart automatically to complete installation."
!define DESCRIPTION_SUPEULA "By installing, you are agreeing to the Supplemental End User License Agreement for this update."
!define DESCRIPTION_MSLT    "By installing, you are agreeing to the Microsoft Software License Terms for this update."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${W2KSP4}       "Updates Windows 2000 to Service Pack 4, as required to install the Windows Update Agent.$\r$\n${DESCRIPTION_REBOOTS} ${DESCRIPTION_SUPEULA}"
	!insertmacro MUI_DESCRIPTION_TEXT ${IE6SP1}       "Updates Internet Explorer to 6.0 SP1, as required for Legacy Update.$\r$\n${DESCRIPTION_REBOOTS} ${DESCRIPTION_SUPEULA}"
	!insertmacro MUI_DESCRIPTION_TEXT ${XPSP3}        "Updates Windows XP to Service Pack 3. Required if you would like to activate Windows online. ${DESCRIPTION_REBOOTS} ${DESCRIPTION_SUPEULA}"
	!insertmacro MUI_DESCRIPTION_TEXT ${WES09}        "Configures Windows to appear as Windows Embedded POSReady 2009 to Windows Update, enabling access to Windows XP security updates released between 2014 and 2019. Please note that Microsoft officially advises against doing this."
	!insertmacro MUI_DESCRIPTION_TEXT ${2003SP2}      "Updates Windows XP x64 Edition or Windows Server 2003 to Service Pack 2. Required if you would like to activate Windows online. ${DESCRIPTION_REBOOTS} ${DESCRIPTION_SUPEULA}"
	!insertmacro MUI_DESCRIPTION_TEXT ${VISTASP2}     "Updates Windows Vista or Windows Server 2008 to Service Pack 2, as required to install the Windows Update Agent. ${DESCRIPTION_REBOOTS} ${DESCRIPTION_MSLT}"
	!insertmacro MUI_DESCRIPTION_TEXT ${VISTASSU}     "Updates Windows Vista or Windows Server 2008 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n${DESCRIPTION_REBOOTS}"
	!insertmacro MUI_DESCRIPTION_TEXT ${VISTAIE9}     "Updates Internet Explorer to 9.0.$\r$\n${DESCRIPTION_REBOOTS} ${DESCRIPTION_MSLT}"
	!insertmacro MUI_DESCRIPTION_TEXT ${WIN7SP1}      "Updates Windows 7 or Windows Server 2008 R2 to Service Pack 1, as required to install the Windows Update Agent. ${DESCRIPTION_REBOOTS} ${DESCRIPTION_MSLT}"
	!insertmacro MUI_DESCRIPTION_TEXT ${WIN7SSU}      "Updates Windows 7 or Windows Server 2008 R2 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n${DESCRIPTION_REBOOTS}"
	!insertmacro MUI_DESCRIPTION_TEXT ${WIN8SSU}      "Updates Windows 8 or Windows Server 2012 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n${DESCRIPTION_REBOOTS}"
	!insertmacro MUI_DESCRIPTION_TEXT ${WIN81UPGRADE} "Windows 8 can be updated to Windows 8.1. This process involves a manual download. After Legacy Update setup completes, a Microsoft website will be opened with more information."
	!insertmacro MUI_DESCRIPTION_TEXT ${WIN81UPDATE1} "Updates Windows 8.1 to Update 1, as required to resolve issues with the Windows Update Agent. Also required to upgrade to Windows 10.$\r$\n${DESCRIPTION_REBOOTS}"
	!insertmacro MUI_DESCRIPTION_TEXT ${WIN81SSU}     "Updates Windows 8.1 or Windows Server 2012 R2 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n${DESCRIPTION_REBOOTS}"
	!insertmacro MUI_DESCRIPTION_TEXT ${WUA}          "Updates the Windows Update Agent to the latest version, as required for Legacy Update."
	!insertmacro MUI_DESCRIPTION_TEXT ${ROOTCERTS}    "Updates the root certificate store to the latest from Microsoft, and enables additional modern security features. Root certificates are used to verify the security of encrypted (https) connections. This fixes connection issues with some websites."
	!insertmacro MUI_DESCRIPTION_TEXT ${WIN7MU}       "Configures Windows to install updates for Microsoft Office and other Microsoft software."
	!insertmacro MUI_DESCRIPTION_TEXT ${ACTIVATE}     "Your copy of Windows is not activated. If you update the root certificates store, Windows Product Activation can be completed over the internet. Legacy Update can start the activation wizard after installation so you can activate your copy of Windows."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function OnMouseOverSection
	${If} $0 == ${LEGACYUPDATE}
		${If} ${AtMostWinXP2003}
			StrCpy $0 "Installs Legacy Update, enabling access to the full Windows Update interface via the legacyupdate.net website. Windows Update will be configured to use the Legacy Update proxy server."
		${ElseIf} ${AtMostWinVista}
			StrCpy $0 "Installs Legacy Update, enabling access to the full Windows Update interface via the legacyupdate.net website, and Windows Update Control Panel. Windows Update will be configured to use the Legacy Update proxy server."
		${Else}
			StrCpy $0 "Installs the Legacy Update ActiveX control, enabling access to the classic Windows Update interface via the legacyupdate.net website."
		${EndIf}
		SendMessage $mui.ComponentsPage.DescriptionText ${WM_SETTEXT} 0 "STR:"
		EnableWindow $mui.ComponentsPage.DescriptionText 1
		SendMessage $mui.ComponentsPage.DescriptionText ${WM_SETTEXT} 0 "STR:$0"
	${EndIf}
FunctionEnd

!macro Init
	SetShellVarContext All
	${If} ${RunningX64}
		SetRegView 64
		StrCpy $InstallDir "$PROGRAMFILES64\${NAME}"
	${Else}
		StrCpy $InstallDir "$PROGRAMFILES32\${NAME}"
	${EndIf}
	StrCpy $RunOnceDir "$COMMONPROGRAMDATA\Legacy Update"
	!insertmacro EnsureAdminRights
	SetDetailsPrint listonly
!macroend

Function .onInit
	!insertmacro Init

	${If} ${IsRunOnce}
	${OrIf} ${IsPostInstall}
		Call OnRunOnceLogon
	${EndIf}

	SetOutPath $PLUGINSDIR
	File Patches.ini

	SetOutPath $RunOnceDir

	${MementoSectionRestore}

	${If} ${IsWin2000}
		; Determine whether Win2k prereqs need to be installed
		Call NeedsW2KSP4
		Call NeedsKB835732
		Pop $0
		Pop $1
		${If} $0 == 0
		${AndIf} $1 == 0
			!insertmacro RemoveSection ${W2KSP4}
		${EndIf}

		Call NeedsIE6
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${IE6SP1}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${W2KSP4}
		!insertmacro RemoveSection ${IE6SP1}
	${EndIf}

	${If} ${IsWinXP2002}
		; Determine whether XP prereqs need to be installed
		Call NeedsXPSP3
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${XPSP3}
		${EndIf}

		ReadRegDword $0 HKLM "${REGPATH_POSREADY}" "Installed"
		${If} $0 == 1
			!insertmacro RemoveSection ${WES09}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${XPSP3}
		!insertmacro RemoveSection ${WES09}
	${EndIf}

	${If} ${IsWinXP2003}
		; Determine whether 2003 prereqs need to be installed
		Call Needs2003SP2
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${2003SP2}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${2003SP2}
	${EndIf}

	${If} ${IsWinVista}
		; Determine whether Vista prereqs need to be installed
		Call NeedsVistaSP2
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${VISTASP2}
		${EndIf}

		Call NeedsVistaPostSP2
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${VISTASSU}
		${EndIf}

		Call NeedsIE9
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${VISTAIE9}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${VISTASP2}
		!insertmacro RemoveSection ${VISTASSU}
		!insertmacro RemoveSection ${VISTAIE9}
	${EndIf}

	${If} ${IsWin7}
		; Determine whether 7 prereqs need to be installed
		Call NeedsWin7SP1
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${WIN7SP1}
		${EndIf}

		Call NeedsWin7SHA2
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${WIN7SSU}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${WIN7SP1}
		!insertmacro RemoveSection ${WIN7SSU}
		!insertmacro RemoveSection ${WIN7MU}
	${EndIf}

	${If} ${IsWin8}
		; Determine whether 8 prereqs need to be installed
		${IfNot} ${IsWin8}
			!insertmacro RemoveSection ${WIN81UPGRADE}
		${EndIf}

		Call NeedsKB4598297
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${WIN8SSU}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${WIN81UPGRADE}
		!insertmacro RemoveSection ${WIN8SSU}
	${EndIf}

	${If} ${IsWin8.1}
		; Determine whether 8.1 prereqs need to be installed
		Call NeedsWin81Update1
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${WIN81UPDATE1}
		${EndIf}

		Call NeedsKB3021910
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${WIN81SSU}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${WIN81UPDATE1}
		!insertmacro RemoveSection ${WIN81SSU}
	${EndIf}

	Call DetermineWUAVersion
	${If} $0 == ""
		!insertmacro RemoveSection ${WUA}
	${EndIf}

	${If} ${IsWinXP2002}
	${OrIf} ${IsWinXP2003}
		; Assume not activated if the activation tray icon process is running
		FindProc::FindProc "wpabaln.exe"
		${If} $R0 != 1
			!insertmacro RemoveSection ${ACTIVATE}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${ACTIVATE}
	${EndIf}

	; Try not to be too intrusive on Windows 10 and newer, which are (for now) fine
	${If} ${AtLeastWin10}
		!insertmacro RemoveSection ${ROOTCERTS}
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
		Call DownloadXPSP2
		Call DownloadXPSP3
	${EndIf}

	; XP 2003
	${If} ${IsWinXP2003}
	${AndIf} ${SectionIsSelected} ${2003SP2}
		Call Download2003SP2
	${EndIf}

	; Vista
	${If} ${IsWinVista}
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
		Call DownloadWin7SP1
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
	${IfNot} ${Silent}
	${AndIfNot} ${IsRunOnce}
		${If} ${FileExists} "$InstallDir\LegacyUpdate.dll"
			Exec '$SYSDIR\rundll32.exe "$InstallDir\LegacyUpdate.dll",LaunchUpdateSite firstrun'
		${ElseIf} ${AtLeastWinVista}
			Exec '$SYSDIR\wuauclt.exe /ShowWUAutoScan'
		${EndIf}

		; Launch Windows 8.1 upgrade site if requested by the user
		${If} ${SectionIsSelected} ${WIN81UPGRADE}
			ExecShell "" "${WIN81UPGRADE_URL}"
		${EndIf}
	${EndIf}
FunctionEnd

Function CleanUp
	Call CleanUpRunOnce
	!insertmacro InhibitSleep 0

	${If} ${IsPostInstall}
	${OrIfNot} ${RebootFlag}
		RMDir /r /REBOOTOK "$RunOnceDir"
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
	Call CleanUp
FunctionEnd

Function .onSelChange
	${If} ${SectionIsSelected} ${WES09}
		; Check for SSE2.
		System::Call 'kernel32::IsProcessorFeaturePresent(i ${PF_XMMI64_INSTRUCTIONS_AVAILABLE}) i .r0'
		${If} $0 == 0
			MessageBox MB_USERICON "Your processor does not support the Streaming SIMD Extensions 2 (SSE2) instruction set, which is required to install Windows Embedded 2009 updates released after May 2018. Processors that initially implemented SSE2 instructions include the Intel Pentium 4, Pentium M, and AMD Athlon 64.$\r$\n$\r$\nTo protect your Windows installation from becoming corrupted by incompatible updates, this option will be disabled." /SD IDOK
			!insertmacro UnselectSection ${WES09}
		${EndIf}
	${EndIf}
FunctionEnd

Function un.onInit
	!insertmacro Init
FunctionEnd

Function un.onUninstSuccess
	!insertmacro DetailPrint "Done"
	Call un.RebootIfRequired
	${IfNot} ${RebootFlag}
		Quit
	${EndIf}
FunctionEnd
