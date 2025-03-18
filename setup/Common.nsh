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

!define IsPassive `"" HasFlag "/passive"`
!define IsActiveX `"" HasFlag "/activex"`
!define IsHelp    `"" HasFlag "/?"`
!define IsVerbose `"" HasFlag "/v"`

!if ${DEBUG} == 1
!define TestRunOnce `"" HasFlag "/testrunonce"`
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

!macro InhibitSleep state
!if ${state} == 1
	System::Call '${SetThreadExecutionState}(${ES_CONTINUOUS}|${ES_SYSTEM_REQUIRED})'
!else
	System::Call '${SetThreadExecutionState}(${ES_CONTINUOUS})'
!endif
!macroend
