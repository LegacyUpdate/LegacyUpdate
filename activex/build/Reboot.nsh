!define REGPATH_RUNONCE "Software\Microsoft\Windows\CurrentVersion\RunOnce"

!define EWX_REBOOT 0x02

!define ERROR_SUCCESS_REBOOT_REQUIRED 3010

!macro CheckRebootRequired
	IfRebootFlag 0 end
		WriteRegStr HKLM "${REGPATH_RUNONCE}" "Legacy Update" '"$EXEPATH"'
		Call PromptReboot
	end:
!macroend

Function PromptReboot
	${If} ${IsWin2000}
		Reboot
	${Else}
		System::Call 'Shell32::RestartDialog(p $HWNDPARENT, t "Windows will be restarted to complete installation of prerequisite components. Setup will resume after the restart.", i ${EWX_REBOOT})'
	${EndIf}
	SetErrorLevel ${ERROR_SUCCESS_REBOOT_REQUIRED}
FunctionEnd
