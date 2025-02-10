!macro TODO thing
	Function Install${thing}
		DetailPrint "TODO: ${thing}"
	FunctionEnd
!macroend

!insertmacro TODO "NT4SP6A"
!insertmacro TODO "NT4Rollup"

; Workstation/Server
!insertmacro TODO "KB243649"
!insertmacro TODO "KB304158"
!insertmacro TODO "KB314147"
!insertmacro TODO "KB318138"
!insertmacro TODO "KB320206"
!insertmacro TODO "KB326830"
!insertmacro TODO "KB329115"
!insertmacro TODO "KB810833"
!insertmacro TODO "KB815021"
!insertmacro TODO "KB817606"
!insertmacro TODO "KB819696"

; Server-only
!insertmacro TODO "KB823182"
!insertmacro TODO "KB823803"
!insertmacro TODO "KB824105"
!insertmacro TODO "KB824141"
!insertmacro TODO "KB824146"
!insertmacro TODO "KB825119"
!insertmacro TODO "KB828035"
!insertmacro TODO "KB828741"
!insertmacro TODO "NT4KB835732"
!insertmacro TODO "KB839645"
!insertmacro TODO "KB841533"
!insertmacro TODO "KB841872"
!insertmacro TODO "KB870763"
!insertmacro TODO "KB873339"
!insertmacro TODO "KB873350"
!insertmacro TODO "KB885249"
!insertmacro TODO "KB885834"
!insertmacro TODO "KB885835"
!insertmacro TODO "KB885836"
!insertmacro TODO "KB891711"

; Runtimes
!insertmacro NeedsFileVersionHandler "MSI" "msiexec.exe" "3.1.4000.2435"

Function InstallMSI
	${If} ${NeedsPatch} MSI
		${DetailPrint} "$(Installing)$(MSI)..."
		File "patches\instmsiw.exe"
		!insertmacro ExecWithErrorHandling '$(MSI)' '"$PLUGINSDIR\instmsiw.exe" /c:"msiinst.exe /delayrebootq"'
	${EndIf}
FunctionEnd

!insertmacro TODO "NT4VCRT"
!insertmacro TODO "NT4VB6"
!insertmacro TODO "NT4MFCOLE"
!insertmacro TODO "NT4WMP64"
!insertmacro TODO "NT4DX5"

; Cleanup
Function CleanUpSPUninstall
	DetailPrint "TODO: CleanUpSPUninstall"
FunctionEnd
