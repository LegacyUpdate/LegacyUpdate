!define MUI_UI              "modern_aerowizard.exe"
!define MUI_UI_HEADERIMAGE  "modern_aerowizard.exe"

!define MUI_CUSTOMFUNCTION_GUIINIT   OnShow
!define MUI_CUSTOMFUNCTION_UNGUIINIT un.OnShow
!define MUI_CUSTOMFUNCTION_ABORT     CleanUpRunOnce

!define NAME        "Legacy Update"
!define VERSION     "1.4"
!define LONGVERSION "1.4.0.0"
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
!define REGPATH_WINLOGON           "Software\Microsoft\Windows NT\CurrentVersion\Winlogon"
!define REGPATH_POSREADY           "System\WPA\PosReady"

Name         "${NAME}"
Caption      "Install ${NAME}"
BrandingText "${NAME} ${VERSION} - ${DOMAIN}"
OutFile      "LegacyUpdate-${VERSION}.exe"
InstallDir   "$ProgramFiles\$(^Name)"
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
VIFileVersion    ${LONGVERSION}

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
!include Win\WinError.nsh
!include Win\WinNT.nsh
!include WinMessages.nsh
!include WinVer.nsh
!include WordFunc.nsh
!include x64.nsh

!include Common.nsh
!include AeroWizard.nsh
!include Download2KXP.nsh
!include DownloadVista7.nsh
!include DownloadWUA.nsh
!include RunOnce.nsh
!include UpdateRoots.nsh

!insertmacro GetParameters
!insertmacro GetOptions

!define MUI_PAGE_HEADER_TEXT "Welcome to Legacy Update"
!define MUI_COMPONENTSPAGE_TEXT_TOP "Select what you would like Legacy Update to do. An internet connection is required to download additional components from Microsoft. Your computer will restart automatically if needed. Close all other programs before continuing."
!define MUI_PAGE_CUSTOMFUNCTION_PRE  ComponentsPageCheck
!define MUI_PAGE_CUSTOMFUNCTION_SHOW OnShow
!define MUI_PAGE_FUNCTION_GUIINIT   OnShow

!insertmacro MUI_PAGE_COMPONENTS

!define MUI_PAGE_HEADER_TEXT "Performing Actions"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW OnShow

!insertmacro MUI_PAGE_INSTFILES

!define MUI_PAGE_HEADER_TEXT "Uninstall Legacy Update"
!define MUI_UNCONFIRMPAGE_TEXT_TOP "Legacy Update will be uninstalled. Your Windows Update configuration will be reset to directly use Microsoft servers."
!define MUI_PAGE_CUSTOMFUNCTION_SHOW un.OnShow

!insertmacro MUI_UNPAGE_CONFIRM

!define MUI_PAGE_HEADER_TEXT "Performing Actions"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW un.OnShow

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

!macro RestartWUAUService
	!insertmacro DetailPrint "Restarting Windows Update service..."
	SetDetailsPrint none
	ExecShellWait "" "net" "stop wuauserv" SW_HIDE
	ExecShellWait "" "net" "start wuauserv" SW_HIDE
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

Function PostInstall
	${IfNot} ${Silent}
	${AndIfNot} ${IsRunOnce}
		${If} ${FileExists} "$INSTDIR\LegacyUpdate.dll"
			Exec '$SYSDIR\rundll32.exe "$INSTDIR\LegacyUpdate.dll",LaunchUpdateSite'
		${ElseIf} ${AtLeastWinVista}
			Exec '$SYSDIR\wuauclt.exe /ShowWUAutoScan'
		${EndIf}

		; Clean up temporary setup exe if we created it (likely on next reboot)
		${If} ${FileExists} "$INSTDIR\LegacyUpdateSetup.exe"
			Delete /REBOOTOK "$INSTDIR\LegacyUpdateSetup.exe"
		${EndIf}
	${EndIf}
FunctionEnd

; Win2k prerequisities
Section "Windows 2000 Service Pack 4" W2KSP4
	SectionIn Ro
	Call DownloadW2KSP4
	Call DownloadKB835732
	Call RebootIfRequired
SectionEnd

Section "Internet Explorer 6.0 Service Pack 1" IE6SP1
	SectionIn Ro
	Call DownloadIE6
	Call RebootIfRequired
SectionEnd

; XP 2002 prerequisities
${MementoSection} "Windows XP Service Pack 3" XPSP3
	Call DownloadXPSP2
	Call RebootIfRequired
	Call DownloadXPSP3
	Call RebootIfRequired
${MementoSectionEnd}

; XP 2003 prerequisities
Section "Windows XP/Server 2003 Service Pack 2" 2003SP2
	Call Download2003SP2
	Call RebootIfRequired
SectionEnd

; Vista prerequisities
Section "Windows Vista Service Pack 2" VISTASP2
	SectionIn Ro
	Call DownloadVistaSP1
	Call RebootIfRequired
	Call DownloadVistaSP2
	Call RebootIfRequired
SectionEnd

Section "Windows Vista Post-Service Pack 2 Updates" VISTAPOSTSP2
	SectionIn Ro
	Call DownloadKB3205638
	Call DownloadKB4012583
	Call DownloadKB4015195
	Call DownloadKB4015380
	Call RebootIfRequired
SectionEnd

; 7 prerequisities
Section "Windows 7 Service Pack 1" WIN7SP1
	SectionIn Ro
	Call DownloadWin7SP1
	Call RebootIfRequired
SectionEnd

Section "Windows Update Agent update" WIN7WUA
	SectionIn Ro
	Call DownloadKB3138612
	Call RebootIfRequired
SectionEnd

; Shared prerequisites
Section "Windows Update Agent update" WUA
	SectionIn Ro
	Call DownloadWUA
SectionEnd

${MementoUnselectedSection} "Enable Windows Embedded 2009 updates" WES09
	WriteRegDword HKLM "${REGPATH_POSREADY}" "Installed" 1
${MementoSectionEnd}

${MementoSection} "Update root certificates store" ROOTCERTS
	Call UpdateRoots
${MementoSectionEnd}

; Main installation
${MementoSection} "Legacy Update" LEGACYUPDATE
	SetOutPath $INSTDIR
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
	${If} ${RunningX64}
		WriteRegStr HKCR "${REGPATH_CPLCLSID}" "System.Software.TasksFileUrl" "$OUTDIR\LegacyUpdate.dll,-203"
	${Else}
		WriteRegStr HKCR "${REGPATH_CPLCLSID}" "System.Software.TasksFileUrl" "$OUTDIR\LegacyUpdate.dll,-202"
	${EndIf}
	WriteRegStr   HKLM "${REGPATH_CPLNAMESPACE}" "" "${NAME}"

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

	; Hide WU shortcuts
	; TODO: How can we consistently find the shortcuts for non-English installs?
	${If} ${AtMostWin2003}
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

	; Delete old LegacyUpdate.dll in System32
	${If} ${FileExists} $WINDIR\System32\LegacyUpdate.dll
		UnRegDLL $WINDIR\System32\LegacyUpdate.dll
		Delete $WINDIR\System32\LegacyUpdate.dll
	${EndIf}

	; Delete old LegacyUpdate.inf
	${If} ${FileExists} $WINDIR\inf\LegacyUpdate.inf
		Delete $WINDIR\inf\LegacyUpdate.inf
	${EndIf}

	; Set WSUS server
	${If} ${AtMostWinVista}
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
		WriteRegDword HKLM "${REGPATH_WUAUPOLICY}" "UseWUServer" 1

		; Restart service
		!insertmacro RestartWUAUService
	${EndIf}
${MementoSectionEnd}

${MementoSection} "Activate Windows" ACTIVATE
	ExecShell "" "$SYSDIR\oobe\msoobe.exe" "/a"
${MementoSectionEnd}

${MementoSectionDone}

Section -Uninstall
	; Delete shortcut
	${UnpinShortcut} "$COMMONSTARTMENU\${NAME}.lnk"
	Delete "$COMMONSTARTMENU\${NAME}.lnk"

	; Delete Control Panel entry
	DeleteRegKey HKLM "${REGPATH_CPLNAMESPACE}"
	DeleteRegKey HKCR "${REGPATH_CPLCLSID}"

	; Restore shortcuts
	${If} ${FileExists} "$COMMONSTARTMENU\Windows Update.lnk"
		Rename "$INSTDIR\Backup\Windows Update.lnk" "$COMMONSTARTMENU\Windows Update.lnk"
	${EndIf}

	${If} ${FileExists} "$INSTDIR\Backup\Microsoft Update.lnk"
		Rename "$INSTDIR\Backup\Microsoft Update.lnk" "$COMMONSTARTMENU\Microsoft Update.lnk"
	${EndIf}

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
	Delete "$INSTDIR\LegacyUpdateSetup.exe"
	!insertmacro DeleteFileOrAskAbort "$INSTDIR\LegacyUpdate.dll"
	RMDir "$INSTDIR"
	RMDir "$INSTDIR\Backup"
	DeleteRegKey HKLM "${REGPATH_UNINSTSUBKEY}"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${W2KSP4}       "Updates Windows 2000 to Service Pack 4, as required to install the Windows Update Agent.$\r$\nYour computer will restart automatically to complete installation. By installing, you are agreeing to the Supplemental End User License Agreement for this update."
	!insertmacro MUI_DESCRIPTION_TEXT ${XPSP3}        "Updates Windows XP to Service Pack 3. Required if you would like to activate Windows online. Your computer will restart automatically to complete installation. By installing, you are agreeing to the Supplemental End User License Agreement for this update."
	!insertmacro MUI_DESCRIPTION_TEXT ${WES09}        "Configures Windows to appear as Windows Embedded POSReady 2009 to Windows Update, enabling access to Windows XP security updates released between 2014 and 2019. Please note that Microsoft officially advises against doing this."
	!insertmacro MUI_DESCRIPTION_TEXT ${2003SP2}      "Updates Windows XP x64 Edition or Windows Server 2003 to Service Pack 2. Required if you would like to activate Windows online. Your computer will restart automatically to complete installation. By installing, you are agreeing to the Supplemental End User License Agreement for for this update."
	!insertmacro MUI_DESCRIPTION_TEXT ${VISTASP2}     "Updates Windows Vista or Windows Server 2008 to Service Pack 2, as required to install the Windows Update Agent. Your computer will restart automatically to complete installation. By installing, you are agreeing to the Microsoft Software License Terms for this update."
	!insertmacro MUI_DESCRIPTION_TEXT ${VISTAPOSTSP2} "Updates Windows Vista or Windows Server 2008 with additional updates required to resolve issues with the Windows Update Agent.$\r$\nYour computer will restart automatically to complete installation."
	!insertmacro MUI_DESCRIPTION_TEXT ${WIN7SP1}      "Updates Windows 7 or Windows Server 2008 R2 to Service Pack 1, as required to install the Windows Update Agent. Your computer will restart automatically to complete installation. By installing, you are agreeing to the Microsoft Software License Terms for this update."
	!insertmacro MUI_DESCRIPTION_TEXT ${WIN7WUA}      "Updates Windows 7 or Windows Server 2008 R2 with additional updates required to resolve issues with the Windows Update Agent.$\r$\nYour computer will restart automatically to complete installation."
	!insertmacro MUI_DESCRIPTION_TEXT ${IE6SP1}       "Updates Internet Explorer to 6.0 SP1, as required for Legacy Update.$\r$\nYour computer will restart automatically to complete installation."
	!insertmacro MUI_DESCRIPTION_TEXT ${WUA}          "Updates the Windows Update Agent to the latest version, as required for Legacy Update."
	!insertmacro MUI_DESCRIPTION_TEXT ${ROOTCERTS}    "Updates the root certificate store to the latest from Microsoft. Root certificates are used to verify the security of encrypted (https) connections. This fixes connection issues with some websites."
	!insertmacro MUI_DESCRIPTION_TEXT ${LEGACYUPDATE} "Installs Legacy Update, enabling access to the full Windows Update interface via the legacyupdate.net website. Windows Update will be configured to use the Legacy Update proxy server."
	!insertmacro MUI_DESCRIPTION_TEXT ${ACTIVATE}     "Your copy of Windows is not activated. If you update the root certificates store, Windows Product Activation can be completed over the internet. Legacy Update can start the activation wizard after installation so you can activate your copy of Windows."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function .onInit
	SetShellVarContext All
	${If} ${RunningX64}
		SetRegView 64
	${EndIf}
	!insertmacro EnsureAdminRights
	SetDetailsPrint listonly

	${If} ${IsRunOnce}
	${OrIf} ${IsPostInstall}
		Call OnRunOnceLogon
	${EndIf}

	SetOutPath $PLUGINSDIR
	File Patches.ini

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
			!insertmacro RemoveSection ${VISTAPOSTSP2}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${VISTASP2}
		!insertmacro RemoveSection ${VISTAPOSTSP2}
	${EndIf}

	${If} ${IsWin7}
		; Determine whether 7 prereqs need to be installed
		Call NeedsWin7SP1
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${WIN7SP1}
		${EndIf}

		Call NeedsKB3138612
		Pop $0
		${If} $0 == 0
			!insertmacro RemoveSection ${WIN7WUA}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${WIN7SP1}
		!insertmacro RemoveSection ${WIN7WUA}
	${EndIf}

	Call DetermineWUAVersion
	${If} $0 == ""
		!insertmacro RemoveSection ${WUA}
	${EndIf}

	${If} ${IsWinXP}
	${OrIf} ${IsWin2003}
		; Assume not activated if the activation tray icon process is running
		FindProcDLL::FindProc "wpabaln.exe"
		${If} $R0 != 1
			!insertmacro RemoveSection ${ACTIVATE}
		${EndIf}
	${Else}
		!insertmacro RemoveSection ${ACTIVATE}
	${EndIf}

	; Try not to be too intrusive on Windows 8 and newer, which are (for now) fine
	${If} ${AtLeastWin8}
		!insertmacro RemoveSection ${ROOTCERTS}

		!insertmacro TaskDialog `'Legacy Update'` \
			`'Legacy Update is intended for earlier versions of Windows'` \
			`'Visit legacyupdate.net for more information.$\r$\n$\r$\nContinue anyway?'` \
			${TDCBF_YES_BUTTON}|${TDCBF_NO_BUTTON} \
			${TD_WARNING_ICON}
		${If} $0 != ${IDYES}
			Quit
		${EndIf}
	${EndIf}
FunctionEnd

Function .onInstSuccess
	${MementoSectionSave}

	; Reboot now if we need to. Nothing further in this function will be run if we do need to reboot.
	Call RebootIfRequired

	; If we're done, launch the update site
	Call PostInstall
	Call CleanUpRunOnce
FunctionEnd

Function .onInstFailed
	Call CleanUpRunOnce
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
	!insertmacro DetailPrint "Done"
	Call un.RebootIfRequired
	${IfNot} ${RebootFlag}
		Quit
	${EndIf}
FunctionEnd
