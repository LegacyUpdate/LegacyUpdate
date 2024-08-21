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
				"Windows will be restarted to complete installation of prerequisite components. Setup will resume after the restart.", \
				${EWX_REBOOT})'
		${EndIf}
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
	WriteRegStr    HKLM "${REGPATH_SYSSETUP}" "CmdLine" ""
	WriteRegDword  HKLM "${REGPATH_SYSSETUP}" "SetupType" 0
	DeleteRegValue HKLM "${REGPATH_SYSSETUP}" "SetupShutdownRequired"
!macroend

Function CleanUpRunOnce
	!insertmacro -CleanUpRunOnce
FunctionEnd

Function un.CleanUpRunOnce
	; Empty
FunctionEnd

!macro -RebootIfRequired un
	${If} ${RebootFlag}
		!insertmacro DetailPrint "Preparing to restart..."

		${IfNot} ${IsRunOnce}
		${AndIfNot} ${NoRestart}
			; Copy to runonce path to ensure installer is accessible by the temp user
			CreateDirectory "$RunOnceDir"
			CopyFiles /SILENT "$EXEPATH" "$RunOnceDir\LegacyUpdateSetup.exe"

			; Remove mark of the web to prevent "Open File - Security Warning" dialog
			System::Call '${DeleteFile}("$RunOnceDir\LegacyUpdateSetup.exe:Zone.Identifier")'
		${EndIf}

		; Somewhat documented in KB939857:
		; https://web.archive.org/web/20090723061647/http://support.microsoft.com/kb/939857
		; See also Wine winternl.h
		WriteRegStr   HKLM "${REGPATH_SYSSETUP}" "CmdLine" '"$RunOnceDir\LegacyUpdateSetup.exe" /runonce'
		WriteRegDword HKLM "${REGPATH_SYSSETUP}" "SetupType" ${SETUP_TYPE_NOREBOOT}
		WriteRegDword HKLM "${REGPATH_SYSSETUP}" "SetupShutdownRequired" ${SETUP_SHUTDOWN_REBOOT}
		!insertmacro -PromptReboot
		Quit
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
		Quit
	${EndIf}
FunctionEnd
