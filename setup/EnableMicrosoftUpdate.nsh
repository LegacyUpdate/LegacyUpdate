Function EnableMicrosoftUpdate
	File EnableMicrosoftUpdate.vbs
	!insertmacro DetailPrint "Enabling Microsoft Update..."
	; Call upon EnableMicrosoftUpdate.vbs to do our job here...
	ExecWait `$SYSDIR\WScript.exe EnableMicrosoftUpdate.vbs /b /nologo`
FunctionEnd