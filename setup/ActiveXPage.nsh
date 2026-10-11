Function ActiveXPage
	; Skip if already installed
	${If} ${FileExists} "$INSTDIR\LegacyUpdate.dll"
		!insertmacro SelectSection ${LEGACYUPDATE}
		Abort
	${EndIf}

	; Skip the page if we're being launched with /runonce, /postinstall, or /passive
	${If} ${IsRunOnce}
	${OrIf} ${IsPostInstall}
	${OrIf} ${IsPassive}
		Abort
	${EndIf}

	; Skip if not possible
	${If} ${IsWin2000}
	${AndIf} ${IsDatacenter}
		Abort
	${EndIf}

	!insertmacro MUI_HEADER_TEXT "$(ActiveXPageTitle)" ""

	${If} ${AtLeastWin10}
		StrCpy $0 "$(ActiveXPageYesTextWin10)"
		StrCpy $1 "$(ActiveXPageNoTitleWin10)"
		StrCpy $2 "$(ActiveXPageNoTextWin10)"
	${ElseIf} ${AtLeastWinVista}
		StrCpy $0 "$(ActiveXPageYesTextVista)"
		StrCpy $1 "$(ActiveXPageNoTitleVista)"
		StrCpy $2 "$(ActiveXPageNoTextVista)"
	${Else}
		StrCpy $0 "$(ActiveXPageYesText2KXP)"
		StrCpy $1 "$(ActiveXPageNoTitle2KXP)"
		StrCpy $2 "$(ActiveXPageNoText2KXP)"
	${EndIf}

	${If} ${SectionIsSelected} ${LEGACYUPDATE}
		StrCpy $3 1
	${Else}
		StrCpy $3 0
	${EndIf}

	LegacyUpdateNSIS::ActiveXPageCreate \
		"$(ActiveXPageText)" \
		"$(ActiveXPageYesTitle)" \
		"$0" \
		"$1" \
		"$2" \
		$3

	Call AeroWizardOnShow

	LegacyUpdateNSIS::ActiveXPageShow
	Pop $0

	${If} $0 == 1
		!insertmacro SelectSection ${LEGACYUPDATE}
	${Else}
		!insertmacro UnselectSection ${LEGACYUPDATE}
	${EndIf}
FunctionEnd
