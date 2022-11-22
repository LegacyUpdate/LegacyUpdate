!define REGPATH_RUNONCE "Software\Microsoft\Windows\CurrentVersion\RunOnce"

!define EWX_REBOOT 0x02

!define IsNativeIA64 '${IsNativeMachineArchitecture} ${IMAGE_FILE_MACHINE_IA64}'

!macro DetailPrint text
	SetDetailsPrint both
	DetailPrint "${text}"
	SetDetailsPrint listonly
!macroend

!macro Download name url filename
	!insertmacro DetailPrint "Downloading ${name}..."
	inetc::get "${url}" "${filename}" /end
	Pop $0
	${If} $0 != "OK"
		${If} $0 != "Cancelled"
			MessageBox MB_OK|MB_USERICON "${name} failed to download.$\r$\n$\r$\n$0" /SD IDOK
		${EndIf}
		SetErrorLevel 1
		Abort
	${EndIf}
!macroend

!macro ExecWithErrorHandling name command
	ExecWait '${command}' $0
	${If} $0 == ${ERROR_SUCCESS_REBOOT_REQUIRED}
		SetRebootFlag true
	${ElseIf} $0 == ${ERROR_INSTALL_USEREXIT}
		SetErrorLevel ${ERROR_INSTALL_USEREXIT}
		Abort
	${ElseIf} $0 != 0
		MessageBox MB_OK|MB_USERICON "${name} failed to install.$\r$\n$\r$\nError code: $0" /SD IDOK
		SetErrorLevel $0
		Abort
	${EndIf}
!macroend

!macro DownloadAndInstall name url filename args
	!insertmacro Download "${name}" "${url}" "${filename}"
	!insertmacro DetailPrint "Installing ${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '${filename} ${args}'
	Delete "${filename}"
!macroend

!macro EnsureAdminRights
	UserInfo::GetAccountType
	Pop $0
	${If} $0 != "admin" ; Require admin rights on NT4+
		MessageBox MB_USERICON "Log on as an administrator to install Legacy Update." /SD IDOK
		SetErrorLevel ERROR_ELEVATION_REQUIRED
		Quit
	${EndIf}
!macroend

!macro DeleteFileOrAskAbort path
	ClearErrors
	Delete "${path}"
	IfErrors 0 +3
		MessageBox MB_RETRYCANCEL|MB_USERICON 'Unable to delete "${path}".$\r$\n$\r$\nIf Internet Explorer is open, close it and click Retry.' /SD IDCANCEL IDRETRY -3
		Abort
!macroend

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
			System::Call 'Shell32::RestartDialog(p $HWNDPARENT, \
				t "Windows will be restarted to complete installation of prerequisite components. Setup will resume after the restart.", \
				i ${EWX_REBOOT})'
		${EndIf}
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
