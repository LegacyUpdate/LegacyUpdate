!addplugindir /x86-unicode x86-unicode

SetPluginUnload alwaysoff

!if ${DEBUG} == 0
	!packhdr        upx.tmp 'upx --lzma -9 upx.tmp'
!endif

!if ${SIGN} == 1
	!finalize       '../build/sign.sh "%1"'
	!uninstfinalize '../build/sign.sh "%1"'
!endif

!macro -Trace msg
	!if ${DEBUG} == 1
		!insertmacro _LOGICLIB_TEMP
		!ifdef __FUNCTION__
			StrCpy $_LOGICLIB_TEMP "${__FUNCTION__}"
		!else
			StrCpy $_LOGICLIB_TEMP "${__SECTION__}"
		!endif
		MessageBox MB_OK `${__FILE__}(${__LINE__}): $_LOGICLIB_TEMP: ${msg}`
	!endif
!macroend
!define TRACE '!insertmacro -Trace'

!define IsNativeIA64 '${IsNativeMachineArchitecture} ${IMAGE_FILE_MACHINE_IA64}'

; Avoid needing to use System.dll for this
!define /redef RunningX64 `"$PROGRAMFILES64" != "$PROGRAMFILES32"`

!macro _HasFlag _a _b _t _f
	!insertmacro _LOGICLIB_TEMP
	${GetParameters} $_LOGICLIB_TEMP
	ClearErrors
	${GetOptions} $_LOGICLIB_TEMP `${_b}` $_LOGICLIB_TEMP
	IfErrors `${_f}` `${_t}`
!macroend

!define IsPassive     `"" HasFlag "/passive"`
!define IsActiveX     `"" HasFlag "/activex"`
!define IsRunOnce     `"" HasFlag "/runonce"`
!define IsPostInstall `"" HasFlag "/postinstall"`
!define NoRestart     `"" HasFlag "/norestart"`
!define IsHelp        `"" HasFlag "/?"`

!if ${DEBUG} == 1
!define IsVerbose   `1 == 1`
!define TestRunOnce `"" HasFlag "/testrunonce"`
!else
!define IsVerbose   `"" HasFlag "/v"`
!endif

!macro -DetailPrint level text
!if ${level} == 0
	${If} ${IsVerbose}
		DetailPrint "${text}"
	${EndIf}
!else
	SetDetailsPrint both
	DetailPrint "${text}"
	SetDetailsPrint listonly
!endif
!macroend

!define VerbosePrint `!insertmacro -DetailPrint 0`
!define DetailPrint  `!insertmacro -DetailPrint 1`

Var /GLOBAL SetupMutexHandle
Var /GLOBAL TermsrvUserModeChanged

Function CheckSetupMutex
	!define SetupMutexName "Global\$(^Name)"
	System::Call '${OpenMutex}(${SYNCHRONIZE}, 0, "${SetupMutexName}") .r0'
	${If} $0 != 0
		System::Call '${CloseHandle}($0)'
		${VerbosePrint} "Setup is already running"
		MessageBox MB_USERICON "$(MsgBoxSetupAlreadyRunning)" /SD IDOK
		SetErrorLevel 1
		Quit
	${EndIf}

	System::Call '${CreateMutex}(0, 1, "${SetupMutexName}") .r0'
	StrCpy $SetupMutexHandle $0
FunctionEnd

Function InitChecks
	SetShellVarContext all
	${If} ${IsVerbose}
		SetDetailsPrint both
	${Else}
		SetDetailsPrint listonly
	${EndIf}

	${If} ${RunningX64}
		SetRegView 64
	${EndIf}

!if ${NT4} == 1
	${IfNot} ${IsWinNT4}
		MessageBox MB_USERICON|MB_OKCANCEL "$(MsgBoxNeedsNT4)" /SD IDCANCEL \
			IDCANCEL +2
		ExecShell "" "${WEBSITE}"
		SetErrorLevel ${ERROR_OLD_WIN_VERSION}
		Quit
	${EndIf}
!else
	${IfNot} ${AtLeastWin2000}
		MessageBox MB_USERICON|MB_OKCANCEL "$(MsgBoxOldWinVersion)" /SD IDCANCEL \
			IDCANCEL +2
		ExecShell "" "${WUR_WEBSITE}"
		SetErrorLevel ${ERROR_OLD_WIN_VERSION}
		Quit
	${EndIf}
!endif

	${If} ${IsHelp}
		MessageBox MB_USERICON "$(MsgBoxUsage)"
		Quit
	${EndIf}

	Call CheckSetupMutex

	ClearErrors
	LegacyUpdateNSIS::IsAdmin
	${If} ${Errors}
		MessageBox MB_USERICON "$(MsgBoxPluginFailed)" /SD IDOK
		SetErrorLevel 1
		Quit
	${EndIf}

	Pop $0
	${If} $0 == 0
		MessageBox MB_USERICON "$(MsgBoxElevationRequired)" /SD IDOK
		SetErrorLevel ${ERROR_ELEVATION_REQUIRED}
		Quit
	${EndIf}

	${If} ${IsRunOnce}
	${OrIf} ${IsPostInstall}
		Call OnRunOnceLogon
	${ElseIfNot} ${AtLeastWin10}
		GetWinVer $0 Build
		ReadRegDword $1 HKLM "${REGPATH_CONTROL_WINDOWS}" "CSDVersion"
		IntOp $1 $1 & 0xFF
		${If} $1 != 0
			${VerbosePrint} "Unexpected service pack: $1"
			StrCpy $1 1
		${EndIf}

!if ${NT4} == 1
		${If} $0 != ${WINVER_BUILD_NT4}
!else
		${If} $0 != ${WINVER_BUILD_2000}
		${AndIf} $0 != ${WINVER_BUILD_XP2002}
		${AndIf} $0 != ${WINVER_BUILD_XP2003}
		${AndIf} $0 != ${WINVER_BUILD_VISTA}
		${AndIf} $0 != ${WINVER_BUILD_VISTA_SP1}
		${AndIf} $0 != ${WINVER_BUILD_VISTA_SP2}
		${AndIf} $0 != ${WINVER_BUILD_VISTA_ESU}
		${AndIf} $0 != ${WINVER_BUILD_7}
		${AndIf} $0 != ${WINVER_BUILD_7_SP1}
		${AndIf} $0 != ${WINVER_BUILD_8}
		${AndIf} $0 != ${WINVER_BUILD_8.1}
!endif
			${VerbosePrint} "Unexpected build: $0"
			StrCpy $1 1
		${EndIf}

		${If} $1 == 1
			MessageBox MB_USERICON|MB_OKCANCEL "$(MsgBoxBetaOS)" /SD IDOK \
				IDOK +3
			SetErrorLevel 1
			Quit
		${EndIf}

!if ${NT4} == 0
		; Detect One-Core-API
		ReadRegDword $0 HKLM "${REGPATH_HOTFIX_OCAB}" "Installed"
		${If} $0 == 1
			${VerbosePrint} "One-Core-API detected"
			MessageBox MB_USERICON|MB_OKCANCEL "$(MsgBoxOneCoreAPI)" /SD IDOK \
				IDOK +3
			SetErrorLevel 1
			Quit
		${EndIf}
!endif
	${EndIf}

!if ${NT4} == 0
	; Detect NNN4NT5
	ReadEnvStr $0 "_COMPAT_VER_NNN"
	${If} $0 != ""
		${VerbosePrint} "NNN4NT5 detected"
		MessageBox MB_USERICON "$(MsgBoxNNN4NT5)" /SD IDOK
		SetErrorLevel 1
		Quit
	${EndIf}

	; Check for compatibility mode (GetVersionEx() and RtlGetNtVersionNumbers() disagreeing)
	GetWinVer $0 Major
	GetWinVer $1 Minor
	GetWinVer $2 Build
	System::Call '${RtlGetNtVersionNumbers}(.r3, .r4, .r5)'
	IntOp $5 $5 & 0xFFFF

	; Windows 2000 lacks RtlGetNtVersionNumbers(), but there is no compatibility mode anyway.
	${If} "$3.$4.$5" != "0.0.0"
	${AndIf} "$0.$1.$2" != "$3.$4.$5"
		${VerbosePrint} "Compatibility mode detected. Fake: $0.$1.$2, Actual: $3.$4.$5"
		MessageBox MB_USERICON "$(MsgBoxCompatMode)" /SD IDOK
		SetErrorLevel 1
		Quit
	${EndIf}
!endif

	; Check for a service pack or hotfix install in progress
	StrCpy $1 0
	System::Call '${OpenMutex}(${SYNCHRONIZE}, 0, "Global\ServicePackOrHotfix") .r0'
	${If} $0 != 0
		System::Call '${CloseHandle}($0)'
		StrCpy $1 1
	${Else}
		; This mutex string is also used by Vista SP2 and 7 SP1, and yes, it has a typo
		System::Call '${OpenMutex}(${SYNCHRONIZE}, 0, "Global\Microsoft® Windows® Vista Sevice Pack 1 Installer") .r0'
		${If} $0 != 0
			System::Call '${CloseHandle}($0)'
			StrCpy $1 1
		${EndIf}
	${EndIf}

	${If} $1 == 1
		${VerbosePrint} "Found a service pack or hotfix installer running"
		MessageBox MB_USERICON "$(MsgBoxInstallInProgress)" /SD IDOK
		SetErrorLevel 1
		Quit
	${EndIf}

	; Check for Terminal Services execute mode
	${If} ${IsServerOS}
	${AndIf} ${IsTerminalServer}
		ReadRegDword $0 HKLM "${REGPATH_CONTROL_TERMSRV}" "TSAppCompat"
		System::Call '${TermsrvAppInstallMode}() .r1'
		${If} $0 == 1
		${AndIf} $1 == 0
			${IfNot} ${IsRunOnce}
			${AndIfNot} ${IsPostInstall}
				MessageBox MB_USERICON|MB_OKCANCEL "$(MsgBoxTermsrvAppInstallMode)" /SD IDOK \
					IDOK +3
				SetErrorLevel 1
				Quit
			${EndIf}

			StrCpy $TermsrvUserModeChanged 1
			System::Call '${SetTermsrvAppInstallMode}(1)'
		${EndIf}
	${EndIf}
FunctionEnd

Function RevertTermsrvUserMode
	${If} $TermsrvUserModeChanged == 1
		System::Call '${SetTermsrvAppInstallMode}(0)'
	${EndIf}
FunctionEnd

!macro InhibitSleep state
!if ${state} == 1
	System::Call '${SetThreadExecutionState}(${ES_CONTINUOUS}|${ES_SYSTEM_REQUIRED})'
!else
	System::Call '${SetThreadExecutionState}(${ES_CONTINUOUS})'
!endif
!macroend

!macro -DeleteWithErrorHandling file
	ClearErrors
	Delete "${file}"
	IfErrors 0 +4
		StrCpy $0 "${file}"
		MessageBox MB_USERICON|MB_RETRYCANCEL "$(MsgBoxCopyFailed)" /SD IDCANCEL \
			IDRETRY -4
		Abort
!macroend

!define DeleteWithErrorHandling `!insertmacro -DeleteWithErrorHandling`
