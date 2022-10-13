!define REGPATH_RUNONCE "Software\Microsoft\Windows\CurrentVersion\RunOnce"

!define EWX_REBOOT 0x02

!define ERROR_SUCCESS_REBOOT_REQUIRED 3010

!macro -PromptReboot
	SetErrorLevel ${ERROR_SUCCESS_REBOOT_REQUIRED}

	${GetParameters} $0
	ClearErrors
	${GetOptions} $0 /norestart $1
	${If} ${Errors}
		; Reboot immediately
		Reboot
	${Else}
		; Prompt for reboot
		IfSilent +2
			System::Call 'Shell32::RestartDialog(p $HWNDPARENT, t "Windows will be restarted to complete installation of prerequisite components. Setup will resume after the restart.", i ${EWX_REBOOT})'
	${EndIf}
!macroend

!macro -RebootIfRequired
	${If} ${RebootFlag}
		${GetParameters} $0
		WriteRegStr HKLM "${REGPATH_RUNONCE}" "Legacy Update" '"$EXEPATH" $0 /runonce'
		!insertmacro -PromptReboot
		Quit
	${EndIf}
!macroend

Function RebootIfRequired
	${MementoSectionSave}
	!insertmacro -RebootIfRequired
FunctionEnd

Function un.RebootIfRequired
	!insertmacro -RebootIfRequired
FunctionEnd
