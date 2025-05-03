!include Constants.nsh

Name         "${NAME}"
Caption      "${NAME}"
BrandingText "${NAME} ${VERSION} - ${DOMAIN}"
OutFile      "LegacyUpdateNT-${VERSION}.exe"
InstallDir   "$PROGRAMFILES32\Legacy Update"
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
ReserveFile "banner-wordmark-nt4.bmp"
ReserveFile "banner-wordmark-nt4-low.bmp"
ReserveFile "PatchesNT4.ini"

!define RUNONCEDIR "$COMMONPROGRAMDATA\Legacy Update"

!define MUI_UI                       "modern_aerowizard.exe"
!define MUI_UI_HEADERIMAGE           "modern_aerowizard.exe"
!define MUI_COMPONENTSPAGE_CHECKBITMAP "${NSISDIR}\Contrib\Graphics\Checks\classic.bmp"

!define MUI_CUSTOMFUNCTION_ABORT     CleanUp

!define MUI_ICON                     "..\LegacyUpdate\icon.ico"
!define MUI_UNICON                   "..\LegacyUpdate\icon.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP       "banner-wordmark-nt4.bmp"
!define MUI_HEADERIMAGE_UNBITMAP     "banner-wordmark-nt4.bmp"

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
!include DownloadNT4.nsh
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

!insertmacro MUI_PAGE_COMPONENTS

; Page custom ActiveXPage

!define MUI_PAGE_HEADER_TEXT         "Performing Actions"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW OnShow

!insertmacro MUI_PAGE_INSTFILES

!include Strings.nsh

Function OnShow
	Call AeroWizardOnShow
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

SectionGroup /e "$(SectionWDU)" NT4WDU
	${MementoSection} "$(SectionWDUYes)" NT4WDUYes
		; No-op; used during IE5.5 install
	${MementoSectionEnd}

	${MementoSection} "$(SectionWDUNo)" NT4WDUNo
		; No-op; used during IE5.5 install
	${MementoSectionEnd}
SectionGroupEnd

SectionGroup /e "Windows NT 4.0 $(Updates)" NT4Updates
	Section "Windows NT 4.0 $(SP) 6a" NT4SP6A
		SectionIn Ro
		Call InstallNT4SP6A ; SP6I386
		Call RebootIfRequired
	SectionEnd

	Section "Windows NT 4.0 Security Rollup Package" NT4ROLLUP
		SectionIn Ro
		Call InstallNT4Rollup ; Q299444
	SectionEnd

	Section "Windows NT 4.0 $(PostSP) 6a $(Updates)" NT4POSTSP
		SectionIn Ro
		; Workstation/Server
		Call InstallKB243649
		Call InstallKB304158
		Call InstallKB314147
		Call InstallKB318138
		Call InstallKB320206
		Call InstallKB326830
		Call InstallKB329115
		Call InstallKB810833
		Call InstallKB815021
		Call InstallKB817606
		Call InstallKB819696
		; Server-only
		Call InstallKB823182
		Call InstallKB823803
		Call InstallKB824105
		Call InstallKB824141
		Call InstallKB824146
		Call InstallKB825119
		Call InstallKB828035
		Call InstallKB828741
		Call InstallNT4KB835732
		Call InstallKB839645
		Call InstallKB841533
		Call InstallKB841872
		Call InstallKB870763
		Call InstallKB873339
		Call InstallKB873350
		Call InstallKB885249
		Call InstallKB885834
		Call InstallKB885835
		Call InstallKB885836
		Call InstallKB891711
		Call RebootIfRequired
	SectionEnd
SectionGroupEnd

SectionGroup /e "$(Runtimes)" NT4Runtimes
	${MementoSection} "$(MSI)" NT4MSI
		Call InstallMSI
	${MementoSectionEnd}

	${MementoSection} "Microsoft $(VC) and $(VB) $(Runtimes)" NT4VCRT
		Call InstallNT4VCRT
		Call InstallNT4VB6
		Call InstallNT4MFCOLE
	${MementoSectionEnd}

	${MementoSection} "$(IE) 6.0 $(SP) 1" NT4IE6SP1
		Call InstallIE6
		Call RebootIfRequired
	${MementoSectionEnd}

	${MementoSection} "$(WMP) 6.4" NT4WMP64
		Call InstallNT4WMP64
	${MementoSectionEnd}

	${MementoSection} "$(DX) 5 $(Unofficial)" NT4DX5
		Call InstallNT4DX5
	${MementoSectionEnd}
SectionGroupEnd

${MementoSection} "$(SectionRootCerts)" ROOTCERTS
	Call ConfigureCrypto

	${IfNot} ${IsPostInstall}
		Call UpdateRoots
	${EndIf}
${MementoSectionEnd}

${MementoSection} "$(SectionNT4USB)" NT4USB
	Call InstallNT4USB
${MementoSectionEnd}

${MementoUnselectedSection} "$(SectionSPCleanup)" SPCLEANUP
	Call CleanUpSPUninstall
${MementoSectionEnd}

Section - PREREQS_END
SectionEnd

${MementoSectionDone}

!macro DESCRIPTION_STRING section
	!insertmacro MUI_DESCRIPTION_TEXT ${${section}} "$(Section${section}Desc)"
!macroend

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro DESCRIPTION_STRING NT4WDU
	!insertmacro DESCRIPTION_STRING NT4WDUYes
	!insertmacro DESCRIPTION_STRING NT4WDUNo
	!insertmacro DESCRIPTION_STRING NT4SP6A
	!insertmacro DESCRIPTION_STRING NT4ROLLUP
	!insertmacro DESCRIPTION_STRING NT4POSTSP
	!insertmacro DESCRIPTION_STRING NT4MSI
	!insertmacro DESCRIPTION_STRING NT4VCRT
	!insertmacro DESCRIPTION_STRING NT4IE6SP1
	!insertmacro DESCRIPTION_STRING NT4WMP64
	!insertmacro DESCRIPTION_STRING NT4DX5
	!insertmacro DESCRIPTION_STRING ROOTCERTS
	!insertmacro DESCRIPTION_STRING SPCLEANUP
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function .onInit
	Call InitChecks

	${MementoSectionRestore}

	SetOutPath $PLUGINSDIR
	File Patches.ini
	SetOutPath "${RUNONCEDIR}"
FunctionEnd

Function ComponentsPageCheck
	; Skip the page if we're being launched with /runonce, /postinstall, or /passive
	${If} ${IsRunOnce}
	${OrIf} ${IsPostInstall}
	${OrIf} ${IsPassive}
		Abort
	${EndIf}
FunctionEnd

Function PreDownload
	${If} ${SectionIsSelected} ${SPCLEANUP}
		StrCpy $SPCleanup 1
	${EndIf}

	${If} ${SectionIsSelected} ${NT4IE6SP1}
		Call DownloadIE6
	${EndIf}

	${If} ${SectionIsSelected} ${ROOTCERTS}
		Call DownloadRoots
	${EndIf}
FunctionEnd

Function PostInstall
	MessageBox MB_OK|MB_USERICON \
		"$(MsgBoxNT4PostInstall)" \
		/SD IDOK
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
	; Nothing for now
FunctionEnd
