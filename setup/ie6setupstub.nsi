; Stub file used to replace the ie6setup_w2k.exe file that was previously hosted by Legacy Update.
; This was a full copy of IE6 SP1, but only for English installations of Windows 2000.
; This is expected to ONLY be run by Legacy Update 1.5 and earlier. It should not be used directly.

!define MUI_UI              "modern_aerowizard.exe"
!define MUI_UI_HEADERIMAGE  "modern_aerowizard.exe"

!define MUI_CUSTOMFUNCTION_GUIINIT   OnShow

!include Constants.nsh

Name         "${NAME}"
Caption      "${NAME} - Internet Explorer 6 Downloader"
BrandingText "${NAME} ${VERSION} - ${DOMAIN}"
OutFile      "ie6setupstub-${VERSION}.exe"

Unicode True
RequestExecutionLevel Admin
AutoCloseWindow true

VIAddVersionKey /LANG=1033 "ProductName"     "${NAME} - Internet Explorer 6 Downloader"
VIAddVersionKey /LANG=1033 "ProductVersion"  "${LONGVERSION}"
VIAddVersionKey /LANG=1033 "CompanyName"     "Hashbang Productions"
VIAddVersionKey /LANG=1033 "LegalCopyright"  "© Hashbang Productions. All rights reserved."
VIAddVersionKey /LANG=1033 "FileDescription" "${NAME} - Internet Explorer 6 Downloader"
VIAddVersionKey /LANG=1033 "FileVersion"     "${LONGVERSION}"
VIProductVersion ${LONGVERSION}
VIFileVersion    ${LONGVERSION}

!define MUI_ICON   "..\icon.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP   "setupbanner.bmp"

!include FileFunc.nsh
!include LogicLib.nsh
!include MUI2.nsh
!include Win\WinNT.nsh
!include WinCore.nsh
!include WinVer.nsh
!include WordFunc.nsh
!include x64.nsh

!include Common.nsh
!include AeroWizard.nsh
!include Download2KXP.nsh

!insertmacro GetParameters
!insertmacro GetOptions

!define MUI_PAGE_HEADER_TEXT "Performing Actions"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW OnShow

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Function OnShow
	Call AeroWizardOnShow
FunctionEnd

Section "Internet Explorer 6.0 Service Pack 1" IE6SP1
	SectionIn Ro
	Call DownloadIE6
	Call InstallIE6
SectionEnd

Function .onInit
	SetShellVarContext All
	!insertmacro EnsureAdminRights
	SetDetailsPrint listonly

	${IfNot} ${IsWin2000}
		MessageBox MB_USERICON "This tool is intended only for use on Windows 2000." /SD IDOK
		Quit
	${EndIf}

	${If} ${NeedsPatch} IE6
		MessageBox MB_USERICON "Internet Explorer 6 Service Pack 1 is already installed." /SD IDOK
		Quit
	${EndIf}

	SetOutPath $PLUGINSDIR
	File Patches.ini

	SetErrorLevel ${ERROR_SUCCESS_REBOOT_REQUIRED}
FunctionEnd
