Function RebootPage
	; Only show if reboot needed
	${IfNot} ${RebootFlag}
		Abort
	${EndIf}

	; Not needed in runonce
	${If} ${IsRunOnce}
		Abort
	${EndIf}

	; If /norestart passed, skip creating the page
	${If} ${NoRestart}
		StrCpy $0 -1
	${Else}
		!insertmacro MUI_HEADER_TEXT "$(RebootPageTitle)" ""

		LegacyUpdateNSIS::RebootPageCreate \
			"$(RebootPageText)" \
			"$(RebootPageTimer)" \
			"$(RebootPageRestart)" \
			"$(RebootPageLater)"

		Call AeroWizardOnShow

		LegacyUpdateNSIS::RebootPageShow
		Pop $0
	${EndIf}

	${If} $0 == 0
	${OrIf} $0 == 1
		; Reboot
		Call RebootIfRequired
	${Else}
		; Later
		${MementoSectionSave}
		Call PrepareRunOnce
		SetErrorLevel ${ERROR_SUCCESS_REBOOT_REQUIRED}
		Quit
	${EndIf}
FunctionEnd
