Function RebootPage
	; Only show if reboot needed
	${IfNot} ${RebootFlag}
		Abort
	${EndIf}

	; Not needed in runonce
	${If} ${IsRunOnce}
		Abort
	${EndIf}

	!insertmacro MUI_HEADER_TEXT "Restarting Windows" ""

	LegacyUpdateNSIS::RebootPageCreate \
		"Your computer will not be up to date until you restart it. Please save any open files, photos or documents and restart now.$\n$\nSetup will resume after restarting. Your computer may restart several times to complete installation." \
		"Restarting in " \
		"Restart" \
		"Later"

	Call AeroWizardOnShow

	LegacyUpdateNSIS::RebootPageShow
	Pop $0
	${If} $0 == 0
	${OrIf} $0 == 1
		; Reboot
		Call RebootIfRequired
	${Else}
		; Later
		Call PrepareRunOnce
		Quit
	${EndIf}
FunctionEnd
