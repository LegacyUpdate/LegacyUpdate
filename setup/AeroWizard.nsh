!macro SetFont font parent control
	GetDlgItem $0 ${parent} ${control}
	SendMessage $0 ${WM_SETFONT} ${font} 0
!macroend

!macro SetControlColor parent control color
	GetDlgItem $0 ${parent} ${control}
	SetCtlColors $0 ${color} SYSCLR:WINDOW
!macroend

!macro SetBackground parent control
	!insertmacro SetControlColor ${parent} ${control} SYSCLR:WINDOWTEXT
!macroend

; This is a PE resource because there's no benefit to LZMA compressing it
PEAddResource "banner-wordmark.png"      "PNG" "#1337"
PEAddResource "banner-wordmark-glow.png" "PNG" "#1338"

Var /GLOBAL ChildHwnd
Var /GLOBAL AeroWizard.Font

!macro -AeroWizardOnShow
	; Get the child window where the wizard page is
	FindWindow $ChildHwnd "#32770" "" $HWNDPARENT

	; Set font
	${If} ${AtLeastWinVista}
		; Aero wizard style
		${If} ${AtLeastWin11}
			; Semi-Fluent style
			CreateFont $3 "Segoe UI Variable Display Semibold" 14 600
			!insertmacro SetControlColor $HWNDPARENT 1037 SYSCLR:WINDOWTEXT
		${Else}
			; Aero style
			CreateFont $3 "Segoe UI" 12 400
			!insertmacro SetControlColor $HWNDPARENT 1037 0x003399
		${EndIf}
		CreateFont $AeroWizard.Font "Segoe UI" 8 400
		!insertmacro SetFont $3 $HWNDPARENT 1037
		!insertmacro SetFont $AeroWizard.Font $HWNDPARENT 1028
		${For} $4 1 3
			!insertmacro SetFont $AeroWizard.Font $HWNDPARENT $4
		${Next}
		${For} $4 1000 1043
			!insertmacro SetFont $AeroWizard.Font $ChildHwnd $4
		${Next}
	${Else}
		; Wizard97ish style
		${If} ${FileExists} "$FONTS\framd.ttf"
			CreateFont $2 "Franklin Gothic Medium" 13 400
			!insertmacro SetControlColor $HWNDPARENT 1037 0x003399
		${Else}
			CreateFont $2 "Verdana" 12 800
			!insertmacro SetControlColor $HWNDPARENT 1037 SYSCLR:WINDOWTEXT
		${EndIf}
		!insertmacro SetFont $2 $HWNDPARENT 1037
		CreateFont $AeroWizard.Font "MS Shell Dlg 2" 8 400
	${EndIf}

	; Set white background
	SetCtlColors $HWNDPARENT SYSCLR:WINDOWTEXT SYSCLR:WINDOW
	SetCtlColors $ChildHwnd  SYSCLR:WINDOWTEXT SYSCLR:WINDOW
	${For} $4 1000 1043
		!insertmacro SetBackground $ChildHwnd $4
	${Next}

	; Set up banner and glass
	LegacyUpdateNSIS::DialogInit

	; Activate taskbar progress bar plugin
	LegacyUpdateNSIS::InitTaskbarProgress
!macroend

Function AeroWizardOnShow
	!insertmacro -AeroWizardOnShow
FunctionEnd

Function un.AeroWizardOnShow
	!insertmacro -AeroWizardOnShow
FunctionEnd

!macro -AeroWizardDialogControl hwnd
	SendMessage ${hwnd} ${WM_SETFONT} $AeroWizard.Font 0
	SetCtlColors ${hwnd} SYSCLR:WINDOWTEXT SYSCLR:WINDOW
!macroend

!define AeroWizardDialogControl '!insertmacro -AeroWizardDialogControl'
