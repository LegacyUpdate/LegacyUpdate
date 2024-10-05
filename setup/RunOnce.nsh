!define IsRunOnce     `"" HasFlag "/runonce"`
!define IsPostInstall `"" HasFlag "/postinstall"`
!define NoRestart     `"" HasFlag "/norestart"`

!macro -PromptReboot
	!insertmacro InhibitSleep 0
	SetErrorLevel ${ERROR_SUCCESS_REBOOT_REQUIRED}

	${If} ${NoRestart}
		; Prompt for reboot
		${IfNot} ${Silent}
			System::Call '${RestartDialog}($HWNDPARENT, \
				"Windows will restart to complete installation of prerequisite components. Setup will resume after the restart.", \
				${EWX_REBOOT})'
		${EndIf}
		Quit
	${Else}
		; Reboot immediately
		System::Call '${GetUserName}(.r0, ${NSIS_MAX_STRLEN}) .r1'
		${If} ${IsRunOnce}
		${AndIf} $0 == "SYSTEM"
			; Running in setup mode. Quit with success, which will cause winlogon to reboot.
			SetErrorLevel ${ERROR_SUCCESS}
			Quit
		${Else}
			; Regular reboot.
			Reboot
		${EndIf}
	${EndIf}
!macroend

!macro -CleanUpRunOnce
	; Restore setup keys
	; Be careful here. Doing this wrong can cause SYSTEM_LICENSE_VIOLATION bootloops!
	WriteRegStr    HKLM "${REGPATH_SETUP}" "CmdLine" ""
	WriteRegDword  HKLM "${REGPATH_SETUP}" "SetupType" 0
	DeleteRegValue HKLM "${REGPATH_SETUP}" "SetupShutdownRequired"
!macroend

Function CleanUpRunOnce
	!insertmacro -CleanUpRunOnce
FunctionEnd

Function un.CleanUpRunOnce
	; Empty
FunctionEnd

Function CleanUpRunOnceFinal
	; Enable logon animation again if needed
	${If} ${AtLeastWin8}
		ClearErrors
		ReadRegDword $0 HKLM "${REGPATH_POLICIES_SYSTEM}" "EnableFirstLogonAnimation_LegacyUpdateTemp"
		${If} ${Errors}
			DeleteRegValue HKLM "${REGPATH_POLICIES_SYSTEM}" "EnableFirstLogonAnimation"
		${Else}
			WriteRegDword HKLM "${REGPATH_POLICIES_SYSTEM}" "EnableFirstLogonAnimation" $0
		${EndIf}
	${EndIf}
FunctionEnd

Function CopyLauncher
	${If} ${IsNativeAMD64}
		File "/ONAME=$OUTDIR\LegacyUpdate.exe" "..\launcher\obj\LegacyUpdate64.exe"
	${Else}
		File "/ONAME=$OUTDIR\LegacyUpdate.exe" "..\launcher\obj\LegacyUpdate32.exe"
	${EndIf}
FunctionEnd

Function un.CopyLauncher
	; Empty
FunctionEnd

!macro -RebootIfRequired un
	${If} ${RebootFlag}
		!insertmacro DetailPrint "Preparing to restart..."

		${IfNot} ${IsRunOnce}
			; Copy to runonce path to ensure installer is accessible by the temp user
			CreateDirectory "$RunOnceDir"
			SetOutPath "$RunOnceDir"
			CopyFiles /SILENT "$EXEPATH" "$RunOnceDir\LegacyUpdateSetup.exe"
			Call ${un}CopyLauncher

			; Remove mark of the web to prevent "Open File - Security Warning" dialog
			System::Call '${DeleteFile}("$RunOnceDir\LegacyUpdateSetup.exe:Zone.Identifier")'
		${EndIf}

		; Somewhat documented in KB939857:
		; https://web.archive.org/web/20090723061647/http://support.microsoft.com/kb/939857
		; See also Wine winternl.h
		WriteRegStr   HKLM "${REGPATH_SETUP}" "CmdLine" '"$RunOnceDir\LegacyUpdate.exe" /runonce'
		WriteRegDword HKLM "${REGPATH_SETUP}" "SetupType" ${SETUP_TYPE_NOREBOOT}
		WriteRegDword HKLM "${REGPATH_SETUP}" "SetupShutdownRequired" ${SETUP_SHUTDOWN_REBOOT}

		; Temporarily disable logon animation if needed
		${If} ${AtLeastWin8}
			ClearErrors
			ReadRegDword $0 HKLM "${REGPATH_POLICIES_SYSTEM}" "EnableFirstLogonAnimation"
			${IfNot} ${Errors}
				WriteRegDword HKLM "${REGPATH_POLICIES_SYSTEM}" "EnableFirstLogonAnimation_LegacyUpdateTemp" $0
			${EndIf}
			WriteRegDword HKLM "${REGPATH_POLICIES_SYSTEM}" "EnableFirstLogonAnimation" 0
		${EndIf}

		; Reboot now
		!insertmacro -PromptReboot
	${Else}
		; Restore setup keys
		Call ${un}CleanUpRunOnce
	${EndIf}
!macroend

Function RebootIfRequired
	${MementoSectionSave}
	!insertmacro -RebootIfRequired ""
FunctionEnd

Function un.RebootIfRequired
	!insertmacro -RebootIfRequired "un."
FunctionEnd

Function OnRunOnceLogon
	; To be safe in case we crash, immediately restore setup keys. We'll set them again if needed.
	Call CleanUpRunOnce

	; If we're in the middle of installing a service pack, let it keep doing its thing. We'll register
	; for setup again, and try again on next boot.
	ClearErrors
	EnumRegKey $0 HKLM "${REGPATH_CBS_PKGSPENDING}" 0
	${IfNot} ${Errors}
		SetRebootFlag true
		Call RebootIfRequired
	${EndIf}
FunctionEnd

Function OnRunOnceDone
	${If} ${IsRunOnce}
	${AndIfNot} ${Abort}
		; Set up postinstall runonce
		WriteRegStr HKLM "${REGPATH_RUNONCE}" "LegacyUpdatePostInstall" '"$RunOnceDir\LegacyUpdateSetup.exe" /postinstall'

		System::Call '${GetUserName}(.r0, ${NSIS_MAX_STRLEN}) .r1'
		${If} $0 == "SYSTEM"
			; Configure winlogon to proceed to the logon dialog
			Call CleanUpRunOnce
		${EndIf}
	${EndIf}
FunctionEnd
