Var Dialog
Var Dialog.Y
Var Dialog.YesIcon
Var Dialog.NoIcon
Var Dialog.YesRadio
Var Dialog.NoRadio

!macro NSD_SetIconFromModule CONTROL MODULE IMAGE HANDLE
	Push $R0
	Push $R1
	System::Call '${LoadLibraryEx}("${MODULE}", 0, ${LOAD_LIBRARY_AS_DATAFILE}) .R0'
	System::Call '${LoadImage}($R0, "${IMAGE}", ${IMAGE_ICON}, 0, 0, ${LR_DEFAULTSIZE}) .R1'
	System::Call '${FreeLibrary}($R0)'
	SendMessage ${CONTROL} ${STM_SETIMAGE} ${IMAGE_ICON} $R1
	StrCpy $R0 $R1
	Pop $R1
	Exch $R0
	Pop ${HANDLE}
!macroend

!define NSD_SetIconFromModule `!insertmacro NSD_SetIconFromModule`

Function ActiveXPage
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

	; Skip if ActiveX install (user already opted for it)
	${If} ${IsActiveX}
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
	${NSD_SetIconFromInstaller} $0 $Dialog.YesIcon

	${NSD_CreateRadioButton} 30u $Dialog.Y -30u 10u "$(ActiveXPageYesTitle)"
	Pop $Dialog.YesRadio
	${AeroWizardDialogControl} $Dialog.YesRadio
	${NSD_OnClick} $Dialog.YesRadio ActiveXPageSelectionChanged

	${If} ${AtLeastWin10}
		StrCpy $0 "$(ActiveXPageYesTextWin10)"
	${ElseIf} ${AtLeastWinVista}
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

	${If} ${AtLeastWin10}
		${NSD_SetIconFromModule} $0 "$WINDIR\System32\shell32.dll" "#16826" $Dialog.NoIcon
		StrCpy $1 "$(ActiveXPageNoTitleWin10)"
		StrCpy $2 "$(ActiveXPageNoTextWin10)"
	${ElseIf} ${AtLeastWinVista}
		${NSD_SetIconFromModule} $0 "$WINDIR\System32\shell32.dll" "#47" $Dialog.NoIcon
		StrCpy $1 "$(ActiveXPageNoTitleVista)"
		StrCpy $2 "$(ActiveXPageNoTextVista)"
	${Else}
		${NSD_SetIconFromModule} $0 "$WINDIR\System32\wupdmgr.exe" "APPICON" $Dialog.NoIcon
		StrCpy $1 "$(ActiveXPageNoTitle2KXP)"
		StrCpy $2 "$(ActiveXPageNoText2KXP)"
	${EndIf}

	${NSD_CreateRadioButton} 30u $Dialog.Y -30u 10u "$1"
	Pop $Dialog.NoRadio
	${AeroWizardDialogControl} $Dialog.NoRadio
	${NSD_OnClick} $Dialog.NoRadio ActiveXPageSelectionChanged

	${If} ${SectionIsSelected} ${LEGACYUPDATE}
		${NSD_Check} $Dialog.YesRadio
		${NSD_SetFocus} $Dialog.YesRadio
	${Else}
		${NSD_Check} $Dialog.NoRadio
		${NSD_SetFocus} $Dialog.NoRadio
	${EndIf}

	IntOp $Dialog.Y $Dialog.Y + 17

	${NSD_CreateLabel} 41u $Dialog.Y -41u 24u "$2"
	Pop $0
	${AeroWizardDialogControl} $0

	nsDialogs::Show
	Call AeroWizardOnShow

	${NSD_FreeIcon} $Dialog.YesIcon
	${NSD_FreeIcon} $Dialog.NoIcon
FunctionEnd

Function ActiveXPageSelectionChanged
	Pop $0
	${NSD_GetState} $Dialog.YesRadio $0
	${If} $0 == ${BST_CHECKED}
		!insertmacro SelectSection ${LEGACYUPDATE}
	${Else}
		!insertmacro UnselectSection ${LEGACYUPDATE}
	${EndIf}
FunctionEnd
