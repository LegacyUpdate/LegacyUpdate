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
	DeleteRegValue HKLM "${REGPATH_SETUP}" "SetupShutdownRequired"
	!insertmacro RunOnceRestoreDword HKLM "${REGPATH_SETUP}" "SetupType"

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

Function PrepareRunOnce
	${If} ${RebootFlag}
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
		WriteRegDword HKLM "${REGPATH_SETUP}" "SetupShutdownRequired" ${SETUP_SHUTDOWN_REBOOT}
		!insertmacro RunOnceOverwriteDword HKLM "${REGPATH_SETUP}" "SetupType" ${SETUP_TYPE_NOREBOOT}

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
	${EndIf}
FunctionEnd

Function RebootIfRequired
	${MementoSectionSave}
	${If} ${RebootFlag}
		; Give the user a moment to understand we're rebooting
		${DetailPrint} "$(StatusRestarting)"
		Sleep 2000

		; Now reboot
		Call PrepareRunOnce
		!insertmacro PromptReboot
	${Else}
		; Restore setup keys
		Call CleanUpRunOnce
	${EndIf}
FunctionEnd

Function OnRunOnceLogon
	; To be safe in case we crash, immediately restore setup keys. We'll set them again if needed.
	Call CleanUpRunOnce

	; If we're in the middle of installing a service pack, the system will reboot without notice. Be prepared for that
	; to happen.
	ClearErrors
	EnumRegKey $1 HKLM "${REGPATH_CBS_REBOOTINPROGRESS}" 0
	${IfNot} ${Errors}
		; System will reboot without notice. Be prepared for that to happen.
		${VerbosePrint} "CBS reboot is pending"
		Call PrepareRunOnce
	${EndIf}
FunctionEnd

!macro SetMarquee state
	Push $0
	FindWindow $0 "#32770" "" $HWNDPARENT
	FindWindow $0 "msctls_progress32" "" $0
!if ${state} == 1
	${NSD_RemoveExStyle} $0 ${PBS_SMOOTH}
	${NSD_AddStyle} $0 ${PBS_MARQUEE}
	SendMessage $0 ${PBM_SETMARQUEE} 1 100
!else
	${NSD_RemoveExStyle} $1 ${PBS_MARQUEE}
	${NSD_AddStyle} $0 ${PBS_SMOOTH}
!endif
	Pop $0
!macroend

Function PollCbsInstall
	ReadRegDWORD $0 HKLM "${REGPATH_CBS}" "ExecuteState"
	${If} $0 <= 0
	${OrIf} $0 == 0xffffffff
		Return
	${EndIf}

	${VerbosePrint} "Packages are still installing [$0]"
	${DetailPrint} "$(StatusCbsInstalling)"

	; Set marquee progress bar
	!insertmacro SetMarquee 1

	; Are we in a RebootInProgress phase?
	ClearErrors
	EnumRegKey $1 HKLM "${REGPATH_CBS_REBOOTINPROGRESS}" 0
	${IfNot} ${Errors}
		; Spin forever. TrustedInstaller will reboot on its own.
		${While} 1 == 1
			Sleep 10000
		${EndWhile}
	${EndIf}

	; Poll ExecuteState, waiting for TrustedInstaller to be done.
	${While} 1 == 1
		ReadRegDWORD $0 HKLM "${REGPATH_CBS}" "ExecuteState"
		${If} $0 <= 0
		${OrIf} $0 == 0xffffffff
			${Break}
		${EndIf}

		Sleep 1000
	${EndWhile}

	; Revert progress bar
	!insertmacro SetMarquee 0
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
