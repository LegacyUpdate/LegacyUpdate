!define ERROR_INSTALL_USEREXIT 1602

!macro DetailPrint text
	SetDetailsPrint both
	DetailPrint "${text}"
	SetDetailsPrint listonly
!macroend

!macro Download name url filename
	NSISdl::download "${url}" "${filename}"
	Pop $0
	${If} $0 != "success"
		${If} $0 != "cancel"
			MessageBox MB_OK|MB_USERICON "${name} failed to download.$\r$\n$\r$\nError code: $0" /SD IDOK
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
	!insertmacro DetailPrint "Downloading ${name}..."
	!insertmacro Download "${name}" "${url}" "${filename}"
	!insertmacro DetailPrint "Installing ${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '${filename} ${args}'
	Delete "${filename}"
!macroend
