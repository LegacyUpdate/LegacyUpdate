Var Dialog
Var Dialog.Y

Function ActiveXPage
	; Skip in runonce
	${If} ${IsRunOnce}
	${OrIf} ${IsPostInstall}
		Abort
	${EndIf}

	; Skip if ActiveX install (user already opted for it)
	${If} ${IsActiveX}
		Abort
	${EndIf}

	; Skip if not required
	${If} ${AtLeastWin7}
	; TODO: Fix ordering of setup.nsi so we can do this
	; ${OrIfNot} ${SectionIsSelected} ${LEGACYUPDATE}
		Abort
	${EndIf}

	!insertmacro MUI_HEADER_TEXT "$(ActiveXPageTitle)" ""

	nsDialogs::Create 1018
	Pop $Dialog

	${AeroWizardDialogControl} $Dialog

	StrCpy $Dialog.Y 0

	${NSD_CreateLabel} 0 $Dialog.Y -1u 24u "$(ActiveXPageText)"
	Pop $0
	${AeroWizardDialogControl} $0

	IntOp $Dialog.Y $Dialog.Y + 60

	${NSD_CreateIcon} 3u $Dialog.Y 4u 4u ""
	Pop $0
	${AeroWizardDialogControl} $0
	${NSD_SetIconFromInstaller} $0 103

	${NSD_CreateRadioButton} 30u $Dialog.Y -30u 10u "$(ActiveXPageYesTitle)"
	Pop $0
	${AeroWizardDialogControl} $0
	${NSD_OnClick} $0 ActiveXPageSelectionChanged
	SendMessage $0 ${BM_SETCHECK} ${BST_CHECKED} 0

	${NSD_SetFocus} $0

	${If} ${AtLeastWinVista}
		StrCpy $0 "$(ActiveXPageYesTextVista)"
	${Else}
		StrCpy $0 "$(ActiveXPageYesText2KXP)"
	${EndIf}

	IntOp $Dialog.Y $Dialog.Y + 17

	${NSD_CreateLabel} 41u $Dialog.Y -41u 24u "$0"
	Pop $0
	${AeroWizardDialogControl} $0

	IntOp $Dialog.Y $Dialog.Y + 50

	${NSD_CreateIcon} 3u $Dialog.Y 4u 4u ""
	Pop $0
	${AeroWizardDialogControl} $0

	${If} ${AtLeastWinVista}
		${NSD_SetIcon} $0 "$WINDIR\System32\wucltux.dll" $1
		StrCpy $1 "$(ActiveXPageNoTitleVista)"
		StrCpy $2 "$(ActiveXPageNoTextVista)"
	${Else}
		${NSD_SetIcon} $0 "res://$WINDIR\System32\wupdmgr.exe/#Icon/APPICON" $1
		StrCpy $1 "$(ActiveXPageNoTitle2KXP)"
		StrCpy $2 "$(ActiveXPageNoText2KXP)"
	${EndIf}

	${NSD_CreateRadioButton} 30u $Dialog.Y -30u 10u "$1"
	Pop $0
	${AeroWizardDialogControl} $0
	${NSD_OnClick} $0 ActiveXPageSelectionChanged

	IntOp $Dialog.Y $Dialog.Y + 17

	${NSD_CreateLabel} 41u $Dialog.Y -41u 24u "$2"
	Pop $0
	${AeroWizardDialogControl} $0

	nsDialogs::Show
	Call AeroWizardOnShow
FunctionEnd

Function ActiveXPageSelectionChanged
	Pop $0
	${NSD_GetState} $0 $0

	; TODO: Fix ordering of setup.nsi so we can do this
	${If} $0 == ${BST_CHECKED}
		; !insertmacro UnselectSection ${ACTIVEX}
	${Else}
		; !insertmacro SelectSection ${ACTIVEX}
	${EndIf}
FunctionEnd
