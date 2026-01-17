Function IsAuditMode
	; 2k/XP
	ReadRegDword $0 HKLM "${REGPATH_SETUP}" "AuditInProgress"
	${If} $0 == 1
		Push 1
	${Else}
		; Vista+
		ReadRegDword $0 HKLM "${REGPATH_SETUP}\Status" "AuditBoot"
		${If} $0 == 1
			Push 1
		${Else}
			Push 0
		${EndIf}
	${EndIf}
FunctionEnd

!macro _IsAuditMode _a _b _t _f
	!insertmacro _LOGICLIB_TEMP
	Call IsAuditMode
	Pop $_LOGICLIB_TEMP
	StrCmp $_LOGICLIB_TEMP 1 `${_t}` `${_f}`
!macroend

!define IsAuditMode `"" IsAuditMode ""`

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
		${AndIfNot} ${IsAuditMode}
			; Running in setup mode. Winlogon will reboot for us.
			Quit
		${Else}
			; Regular reboot.
			Reboot
		${EndIf}
	${EndIf}
!macroend

!macro RunOnceOverwriteReg type root subkey name value
	ClearErrors
	ReadReg${type} $0 ${root} "${subkey}" "${name}"
	${IfNot} ${Errors}
		WriteReg${type} ${root} "${subkey}" "${name}_LegacyUpdateTemp" $0
	${EndIf}
	WriteReg${type} ${root} "${subkey}" "${name}" `${value}`
!macroend

!macro RunOnceRestoreReg type root subkey name fallback
	ClearErrors
	ReadReg${type} $0 ${root} "${subkey}" "${name}_LegacyUpdateTemp"
	${If} ${Errors}
!if "${fallback}" == "-"
		DeleteRegValue ${root} "${subkey}" "${name}"
!else
		WriteReg${type} ${root} "${subkey}" "${name}" `${fallback}`
!endif
	${Else}
		WriteReg${type} ${root} "${subkey}" "${name}" $0
		DeleteRegValue ${root} "${subkey}" "${name}_LegacyUpdateTemp"
	${EndIf}
!macroend

Function CleanUpRunOnce
	; The reboot has happened, so remove reboot flag if any
	DeleteRegValue HKLM "${REGPATH_LEGACYUPDATE_SETUP}" "RebootPending"

	; Restore setup keys
	; Be careful here. Doing this wrong can cause SYSTEM_LICENSE_VIOLATION bootloops!
	!insertmacro RunOnceRestoreReg Str   HKLM "${REGPATH_SETUP}" "CmdLine"   ""
	!insertmacro RunOnceRestoreReg Dword HKLM "${REGPATH_SETUP}" "SetupType" ${SETUP_TYPE_NORMAL}
	DeleteRegValue HKLM "${REGPATH_SETUP}" "SetupShutdownRequired"

	${If} ${Abort}
		Call CleanUpRunOnceFinal
	${EndIf}
FunctionEnd

Function CleanUpRunOnceFinal
	; Enable keys we disabled if needed
	${If} ${IsWinXP2002}
		!insertmacro RunOnceRestoreReg Dword HKLM "${REGPATH_SECURITYCENTER}" "FirstRunDisabled" "-"
	${EndIf}

	${If} ${AtLeastWin8}
		!insertmacro RunOnceRestoreReg Dword HKLM "${REGPATH_POLICIES_SYSTEM}" "EnableFirstLogonAnimation" "-"
	${EndIf}

	; Delete runonce stuff
	RMDir /r /REBOOTOK "${RUNONCEDIR}"

	; Delete IE6 temp files
	RMDir /r /REBOOTOK "$WINDIR\Windows Update Setup Files"
FunctionEnd

!if ${NT4} == 0
Function CopyLauncher
	${If} ${IsNativeAMD64}
		File /ONAME=LegacyUpdate.exe "..\launcher\obj\LegacyUpdate64.exe"
	${Else}
		File /ONAME=LegacyUpdate.exe "..\launcher\obj\LegacyUpdate32.exe"
	${EndIf}
FunctionEnd
!endif

Var /GLOBAL RunOnce.UseFallback

Function PrepareRunOnce
	${IfNot} ${RebootFlag}
		Return
	${EndIf}

	; Set flag in case the user decides to restart later, so we can jump straight to the restart page
	WriteRegDword HKLM "${REGPATH_LEGACYUPDATE_SETUP}" "RebootPending" 1

	!if ${NT4} == 0
	${IfNot} ${IsRunOnce}
		; Copy to runonce path to ensure installer is accessible by the temp user
		CreateDirectory "${RUNONCEDIR}"
		SetOutPath "${RUNONCEDIR}"
		CopyFiles /SILENT "$EXEPATH" "${RUNONCEDIR}\LegacyUpdateSetup.exe"
		Call CopyLauncher

		; Remove mark of the web to prevent "Open File - Security Warning" dialog
		System::Call '${DeleteFile}("${RUNONCEDIR}\LegacyUpdateSetup.exe:Zone.Identifier")'
	${EndIf}
	!endif

	!if ${NT4} == 1
		StrCpy $1 "${RUNONCEDIR}\LegacyUpdateSetup.exe"
	!else
		StrCpy $1 "${RUNONCEDIR}\LegacyUpdate.exe"
	!endif

	${If} $RunOnce.UseFallback == 1
	${OrIf} ${IsAuditMode}
		WriteRegStr HKLM "${REGPATH_RUNONCE}" "LegacyUpdateRunOnce" '"$1" /runonce'
	${Else}
		; Somewhat documented in KB939857:
		; https://web.archive.org/web/20090723061647/http://support.microsoft.com/kb/939857
		; See also Wine winternl.h
		!insertmacro RunOnceOverwriteReg Str   HKLM "${REGPATH_SETUP}" "CmdLine" '"$1" /runonce'
		!insertmacro RunOnceOverwriteReg Dword HKLM "${REGPATH_SETUP}" "SetupType" ${SETUP_TYPE_NOREBOOT}
		WriteRegDword HKLM "${REGPATH_SETUP}" "SetupShutdownRequired" ${SETUP_SHUTDOWN_REBOOT}
	${EndIf}

	; Temporarily disable Security Center first run if needed
	${If} ${IsWinXP2002}
	${AndIfNot} ${AtLeastServicePack} 2
		${VerbosePrint} "Disabling Security Center first run"
		!insertmacro RunOnceOverwriteReg Dword HKLM "${REGPATH_SECURITYCENTER}" "FirstRunDisabled" 1
	${EndIf}

	; Temporarily disable logon animation if needed
	${If} ${AtLeastWin8}
		${VerbosePrint} "Disabling first logon animation"
		!insertmacro RunOnceOverwriteReg Dword HKLM "${REGPATH_POLICIES_SYSTEM}" "EnableFirstLogonAnimation" 0
	${EndIf}
FunctionEnd

!macro -RebootIfRequired
	${If} ${RebootFlag}
		${If} ${Silent}
		${OrIf} ${IsRunOnce}
			Call RebootIfRequired
		${EndIf}
		Return
	${EndIf}
!macroend

!define RebootIfRequired `!insertmacro -RebootIfRequired`

Function RebootIfRequired
	${MementoSectionSave}
	${If} ${RebootFlag}
		; Reboot now
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
FunctionEnd

!macro SetMarquee state
	Push $0
	FindWindow $0 "#32770" "" $HWNDPARENT
	FindWindow $0 "msctls_progress32" "" $0
!if ${state} == 1
	${NSD_AddStyle} $0 ${PBS_MARQUEE}
	SendMessage $0 ${PBM_SETMARQUEE} 1 100
!else
	${NSD_RemoveStyle} $0 ${PBS_MARQUEE}
!endif
	Pop $0
!macroend

!if ${NT4} == 0
Function PollCbsInstall
	${IfNot} ${AtLeastWinVista}
		Return
	${EndIf}

	ReadRegDword $0 HKLM "${REGPATH_CBS}" "ExecuteState"
	${If} $0 == ${CBS_EXECUTE_STATE_NONE}
	${OrIf} $0 == ${CBS_EXECUTE_STATE_NONE2}
		Return
	${EndIf}

	${VerbosePrint} "Packages are still installing [$0]"
	${DetailPrint} "$(StatusCbsInstalling)"

	; Set marquee progress bar
	!insertmacro SetMarquee 1

	${While} 1 == 1
		; Are we in a RebootInProgress phase?
		ClearErrors
		EnumRegKey $1 HKLM "${REGPATH_CBS_REBOOTINPROGRESS}" 0
		${IfNot} ${Errors}
			; Prepare runonce now and spin forever. TrustedInstaller will reboot on its own.
			SetRebootFlag true
			Call PrepareRunOnce
			${VerbosePrint} "System will restart automatically"
			${While} 1 == 1
				Sleep 10000
			${EndWhile}
		${EndIf}

		; Poll TrustedInstaller execution state
		${If} ${IsWinVista}
		${AndIf} ${AtMostServicePack} 1
			; Special case for Vista pre-SP1 servicing stack, which doesn't have ExecuteState
			LegacyUpdateNSIS::CheckCCPProgress
			Pop $0
			${If} $0 == 2818 ; 0x0b02 (02 0b in registry)
				${Break}
			${EndIf}
		${Else}
			ReadRegDword $0 HKLM "${REGPATH_CBS}" "ExecuteState"
			${If} $0 == ${CBS_EXECUTE_STATE_NONE}
			${OrIf} $0 == ${CBS_EXECUTE_STATE_NONE2}
				${Break}
			${EndIf}
		${EndIf}

		Sleep 1000
	${EndWhile}

	; Revert progress bar
	!insertmacro SetMarquee 0
FunctionEnd
!endif

Function RebootIfCbsRebootPending
	${IfNot} ${AtLeastWinVista}
		Return
	${EndIf}

	StrCpy $1 0

	ClearErrors
	ReadRegDword $0 HKLM "${REGPATH_CBS}" "ExecuteState"
	${IfNot} ${Errors}
	${AndIf} $0 != ${CBS_EXECUTE_STATE_NONE}
	${AndIf} $0 != ${CBS_EXECUTE_STATE_NONE2}
		StrCpy $1 1
	${EndIf}

	ClearErrors
	EnumRegKey $0 HKLM "${REGPATH_CBS_REBOOTPENDING}" 0
	EnumRegKey $0 HKLM "${REGPATH_CBS_PACKAGESPENDING}" 0
	${IfNot} ${Errors}
		StrCpy $1 1
	${EndIf}

	${If} $1 == 1
		${VerbosePrint} "Restarting to install previously pending packages"
		SetRebootFlag true
		${RebootIfRequired}
	${EndIf}
FunctionEnd

Function OnRunOnceDone
	${If} ${IsRunOnce}
	${AndIfNot} ${Abort}
		; Set up postinstall runonce
		${If} ${IsAuditMode}
			${VerbosePrint} "Running postinstall"
			Exec '"${RUNONCEDIR}\LegacyUpdate.exe" /runonce postinstall'
		${Else}
			${VerbosePrint} "Preparing postinstall"
			WriteRegStr HKLM "${REGPATH_RUNONCE}" "LegacyUpdatePostInstall" '"${RUNONCEDIR}\LegacyUpdate.exe" /runonce postinstall'

			System::Call '${GetUserName}(.r0, ${NSIS_MAX_STRLEN}) .r1'
			${If} $0 == "SYSTEM"
				; Configure winlogon to proceed to the logon dialog
				Call CleanUpRunOnce
			${EndIf}
		${EndIf}
	${EndIf}
FunctionEnd
