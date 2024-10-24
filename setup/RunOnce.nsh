!define IsRunOnce     `"" HasFlag "/runonce"`
!define IsPostInstall `"" HasFlag "/postinstall"`
!define NoRestart     `"" HasFlag "/norestart"`

!macro PromptReboot
	!insertmacro InhibitSleep 0
	SetErrorLevel ${ERROR_SUCCESS_REBOOT_REQUIRED}

	${If} ${NoRestart}
		; Prompt for reboot
		${IfNot} ${Silent}
		${AndIfNot} ${IsPassive}
			System::Call '${RestartDialog}($HWNDPARENT, "", ${EWX_REBOOT})'
		${EndIf}
		Quit
	${Else}
		; Reboot immediately
		System::Call '${GetUserName}(.r0, ${NSIS_MAX_STRLEN}) .r1'
		${If} ${IsRunOnce}
		${AndIf} $0 == "SYSTEM"
			; Running in setup mode. Quit with success, which makes winlogon happy.
			SetErrorLevel ${ERROR_SUCCESS}
			Quit
		${Else}
			; Regular reboot.
			Reboot
		${EndIf}
	${EndIf}
!macroend

!macro RunOnceOverwriteDword root subkey name value
	ClearErrors
	ReadRegDword $0 ${root} "${subkey}" "${name}"
	${IfNot} ${Errors}
		WriteRegDword ${root} "${subkey}" "${name}_LegacyUpdateTemp" $0
	${EndIf}
	WriteRegDword ${root} "${subkey}" "${name}" ${value}
!macroend

!macro RunOnceRestoreDword root subkey name
	ClearErrors
	ReadRegDword $0 ${root} "${subkey}" "${name}_LegacyUpdateTemp"
	${If} ${Errors}
		DeleteRegValue ${root} "${subkey}" "${name}"
	${Else}
		WriteRegDword ${root} "${subkey}" "${name}" $0
		DeleteRegValue ${root} "${subkey}" "${name}_LegacyUpdateTemp"
	${EndIf}
!macroend

Function CleanUpRunOnce
	; Restore setup keys
	; Be careful here. Doing this wrong can cause SYSTEM_LICENSE_VIOLATION bootloops!
	WriteRegStr    HKLM "${REGPATH_SETUP}" "CmdLine" ""
	WriteRegDword  HKLM "${REGPATH_SETUP}" "SetupType" 0
	DeleteRegValue HKLM "${REGPATH_SETUP}" "SetupShutdownRequired"

	${If} ${Abort}
		Call CleanUpRunOnceFinal
	${EndIf}
FunctionEnd

Function CleanUpRunOnceFinal
	; Enable keys we disabled if needed
	${If} ${IsWinXP2002}
		!insertmacro RunOnceRestoreDword HKLM "${REGPATH_SECURITYCENTER}" "FirstRunDisabled"
	${EndIf}

	${If} ${AtLeastWin8}
		!insertmacro RunOnceRestoreDword HKLM "${REGPATH_POLICIES_SYSTEM}" "EnableFirstLogonAnimation"
	${EndIf}

	; Delete runonce stuff
	RMDir /r /REBOOTOK "${RUNONCEDIR}"
FunctionEnd

Function CopyLauncher
	${If} ${IsNativeAMD64}
		File /ONAME=LegacyUpdate.exe "..\launcher\obj\LegacyUpdate64.exe"
	${Else}
		File /ONAME=LegacyUpdate.exe "..\launcher\obj\LegacyUpdate32.exe"
	${EndIf}
FunctionEnd

Function RebootIfRequired
	${MementoSectionSave}
	${If} ${RebootFlag}
		${DetailPrint} "Preparing to restart..."

		${IfNot} ${IsRunOnce}
			; Copy to runonce path to ensure installer is accessible by the temp user
			CreateDirectory "${RUNONCEDIR}"
			SetOutPath "${RUNONCEDIR}"
			CopyFiles /SILENT "$EXEPATH" "${RUNONCEDIR}\LegacyUpdateSetup.exe"
			Call CopyLauncher

			; Remove mark of the web to prevent "Open File - Security Warning" dialog
			Delete "${RUNONCEDIR}\LegacyUpdateSetup.exe:Zone.Identifier"
		${EndIf}

		; Somewhat documented in KB939857:
		; https://web.archive.org/web/20090723061647/http://support.microsoft.com/kb/939857
		; See also Wine winternl.h
		WriteRegStr   HKLM "${REGPATH_SETUP}" "CmdLine" '"${RUNONCEDIR}\LegacyUpdate.exe" /runonce'
		WriteRegDword HKLM "${REGPATH_SETUP}" "SetupType" ${SETUP_TYPE_NOREBOOT}
		WriteRegDword HKLM "${REGPATH_SETUP}" "SetupShutdownRequired" ${SETUP_SHUTDOWN_REBOOT}

		; Temporarily disable Security Center first run if needed
		${If} ${IsWinXP2002}
		${AndIfNot} ${AtLeastServicePack} 2
			${VerbosePrint} "Disabling Security Center first run"
			!insertmacro RunOnceOverwriteDword HKLM "${REGPATH_SECURITYCENTER}" "FirstRunDisabled" 1
		${EndIf}

		; Temporarily disable logon animation if needed
		${If} ${AtLeastWin8}
			${VerbosePrint} "Disabling first logon animation"
			!insertmacro RunOnceOverwriteDword HKLM "${REGPATH_POLICIES_SYSTEM}" "EnableFirstLogonAnimation" 0
		${EndIf}

		; Reboot now
		!insertmacro PromptReboot
	${Else}
		; Restore setup keys
		Call CleanUpRunOnce
	${EndIf}
FunctionEnd

Function OnRunOnceLogon
	; To be safe in case we crash, immediately restore setup keys. We'll set them again if needed.
	Call CleanUpRunOnce

	; If we're in the middle of installing a service pack, let it keep doing its thing. We'll register
	; for setup again, and try again on next boot.
	ClearErrors
	EnumRegKey $0 HKLM "${REGPATH_CBS_PKGSPENDING}" 0
	${IfNot} ${Errors}
		${VerbosePrint} "Packages still pending - deferring to next reboot"
		SetRebootFlag true
		Call RebootIfRequired
	${EndIf}
FunctionEnd

Function OnRunOnceDone
	${If} ${IsRunOnce}
	${AndIfNot} ${Abort}
		; Set up postinstall runonce
		${VerbosePrint} "Preparing postinstall"
		WriteRegStr HKLM "${REGPATH_RUNONCE}" "LegacyUpdatePostInstall" '"${RUNONCEDIR}\LegacyUpdateSetup.exe" /postinstall'

		System::Call '${GetUserName}(.r0, ${NSIS_MAX_STRLEN}) .r1'
		${If} $0 == "SYSTEM"
			; Configure winlogon to proceed to the logon dialog
			Call CleanUpRunOnce
		${EndIf}
	${EndIf}
FunctionEnd
