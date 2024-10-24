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

Function GetArch
	Var /GLOBAL Arch
	${If} $Arch == ""
		${If} ${IsNativeIA32}
			StrCpy $Arch "x86"
		${ElseIf} ${IsNativeAMD64}
			StrCpy $Arch "x64"
		${ElseIf} ${IsNativeIA64}
			StrCpy $Arch "ia64"
		${Else}
			StrCpy $Arch ""
		${EndIf}
	${EndIf}
	Push $Arch
FunctionEnd

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

!macro _NeedsPatch _a _b _t _f
	!insertmacro _LOGICLIB_TEMP
	Call Needs${_b}
	Pop $_LOGICLIB_TEMP
	StrCmp $_LOGICLIB_TEMP 1 `${_t}` `${_f}`
!macroend

!define NeedsPatch `"" NeedsPatch`

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

Var /GLOBAL Download.ID

Function DownloadRequest
	; TODO: This is broken on XP for some reason
	; Var /GLOBAL Download.UserAgent
	; ${If} $Download.UserAgent == ""
	; 	GetWinVer $R8 Major
	; 	GetWinVer $R9 Minor
	; 	StrCpy $Download.UserAgent "Mozilla/4.0 (${NAME} ${VERSION}; Windows NT $R8.$R9)"
	; ${EndIf}
	; /HEADER "User-Agent: $Download.UserAgent"

	NSxfer::Request \
		/TIMEOUTCONNECT 60000 \
		/TIMEOUTRECONNECT 60000 \
		/OPTCONNECTTIMEOUT 60000 \
		/OPTRECEIVETIMEOUT 60000 \
		/OPTSENDTIMEOUT 60000 \
		/URL "$R0" \
		/LOCAL "$R1" \
		/INTERNETFLAGS ${INTERNET_FLAG_RELOAD}|${INTERNET_FLAG_NO_CACHE_WRITE}|${INTERNET_FLAG_KEEP_CONNECTION}|${INTERNET_FLAG_NO_COOKIES}|${INTERNET_FLAG_NO_UI} \
		/SECURITYFLAGS ${SECURITY_FLAG_STRENGTH_STRONG} \
		$R2 \
		/END
	Pop $Download.ID
FunctionEnd

!macro DownloadRequest url local extra
	StrCpy $R0 "${url}"
	StrCpy $R1 "${local}"
	StrCpy $R2 "${extra}"
	Call DownloadRequest
!macroend

Function DownloadWaitSilent
	NSxfer::Wait /ID $Download.ID /MODE SILENT /END
	NSxfer::Query /ID $Download.ID /ERRORCODE /ERRORTEXT /END
FunctionEnd

Function DownloadWait
	NSxfer::Wait /ID $Download.ID /MODE PAGE \
		/STATUSTEXT "$(DownloadStatusSingle)" "$(DownloadStatusMulti)" \
		/ABORT "$(^Name)" "$(MsgBoxDownloadAbort)" \
		/END
	NSxfer::Query /ID $Download.ID /ERRORCODE /ERRORTEXT /END
FunctionEnd

!macro -Download name url filename verbose
!if ${verbose} == 1
	${DetailPrint} "$(Downloading)${name}..."
!endif
	${If} ${IsVerbose}
		${DetailPrint} "$(Downloading)${name}..."
		${VerbosePrint} "From: ${url}"
		${VerbosePrint} "To: ${filename}"
	${EndIf}
	!insertmacro DownloadRequest "${url}" "${filename}" ""
!if ${verbose} == 1
	Call DownloadWait
!else
	${If} ${IsVerbose}
		Call DownloadWait
	${Else}
		Call DownloadWaitSilent
	${EndIf}
!endif
	Pop $1
	Pop $0
	${If} $0 != "OK"
		${If} $1 != ${ERROR_INTERNET_OPERATION_CANCELLED}
			StrCpy $2 "${name}"
			MessageBox MB_USERICON "$(MsgBoxDownloadFailed)" /SD IDOK
		${EndIf}
		Delete /REBOOTOK "${filename}"
		SetErrorLevel 1
		Abort
	${EndIf}
!macroend

!macro Download name url filename verbose
	${IfNot} ${FileExists} "${RUNONCEDIR}\${filename}"
		${If} ${FileExists} "$EXEDIR\${filename}"
			CopyFiles /SILENT "$EXEDIR\${filename}" "${RUNONCEDIR}\${filename}"
		${Else}
			!insertmacro -Download '${name}' '${url}' '${RUNONCEDIR}\${filename}' ${verbose}
		${EndIf}
	${EndIf}
	StrCpy $0 "${RUNONCEDIR}\${filename}"
!macroend

Var /GLOBAL Exec.Command
Var /GLOBAL Exec.Patch
Var /GLOBAL Exec.Name

Function ExecWithErrorHandling
	${VerbosePrint} "$(^Exec)$Exec.Command"
	LegacyUpdateNSIS::ExecToLog `$Exec.Command`
	Pop $R0
	${VerbosePrint} "$(ExitCode)$R0"

	${If} $R0 == ${ERROR_SUCCESS_REBOOT_REQUIRED}
		${VerbosePrint} "$(RestartRequired)"
		SetRebootFlag true
	${ElseIf} $R0 == ${ERROR_INSTALL_USEREXIT}
		SetErrorLevel ${ERROR_INSTALL_USEREXIT}
		Abort
	${ElseIf} $R0 == ${WU_S_ALREADY_INSTALLED}
		${DetailPrint} "$(AlreadyInstalled)"
	${ElseIf} $R0 == ${WU_E_NOT_APPLICABLE}
		${DetailPrint} "$(NotApplicable)"
	${ElseIf} $R0 != 0
		StrCpy $0 $R0
		LegacyUpdateNSIS::MessageForHresult $R0
		Pop $1
		${DetailPrint} "$1 ($0)"
		StrCpy $2 "$Exec.Name"
		MessageBox MB_USERICON "$(MsgBoxInstallFailed)" /SD IDOK
		SetErrorLevel $R0
		Abort
	${EndIf}
FunctionEnd

!macro ExecWithErrorHandling name command
	StrCpy $Exec.Command '${command}'
	StrCpy $Exec.Name '${name}'
	Call ExecWithErrorHandling
!macroend

!macro Install name filename args
	${DetailPrint} "$(Installing)${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '"$0" ${args}'
!macroend

!macro InstallSP name filename
	; SPInstall.exe /norestart seems to be broken. We let it do a delayed restart, then cancel it.
	${DetailPrint} "$(Extracting)${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '"$0" /X:"$PLUGINSDIR\${filename}"'
	${DetailPrint} "$(Installing)${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '"$WINDIR\system32\cmd.exe" /c "$PLUGINSDIR\${filename}\spinstall.exe" /unattend /nodialog /warnrestart:600'

	; If we successfully abort a shutdown, we'll get exit code 0, so we know a reboot is required.
	LegacyUpdateNSIS::Exec '"$WINDIR\system32\shutdown.exe" /a'
	Pop $0
	${If} $0 == 0
		SetRebootFlag true
	${EndIf}
!macroend

!macro DownloadMSU kbid name url
	!insertmacro Download '${name} (${kbid})' '${url}' '${kbid}.msu' 1
!macroend

Function InstallMSU
	${DetailPrint} "$(Extracting)$Exec.Name..."
	${IfNot} ${IsVerbose}
		SetDetailsPrint none
	${EndIf}
	CreateDirectory "$PLUGINSDIR\$Exec.Patch"
	CreateDirectory "$PLUGINSDIR\$Exec.Patch\Temp"
	StrCpy $Exec.Command '"$WINDIR\system32\expand.exe" -F:* "$0" "$PLUGINSDIR\$Exec.Patch"'
	Call ExecWithErrorHandling
	${IfNot} ${IsVerbose}
		SetDetailsPrint lastused
	${EndIf}

	${DetailPrint} "$(Installing)$Exec.Name..."
	${DisableX64FSRedirection}
	FindFirst $0 $1 "$PLUGINSDIR\$Exec.Patch\*.xml"
	${Do}
		${If} $1 == ""
			FindClose $R0
			${Break}
		${EndIf}

		; We prefer Dism, but need to fall back to Pkgmgr for Vista.
		${If} ${IsWinVista}
			StrCpy $Exec.Command '"$WINDIR\system32\pkgmgr.exe" \
				/n:"$PLUGINSDIR\$Exec.Patch\$R1" \
				/s:"$PLUGINSDIR\$Exec.Patch\Temp" \
				/quiet /norestart'
		${Else}
			StrCpy $Exec.Command '"$WINDIR\system32\dism.exe" \
				/Online \
				/Apply-Unattend:"$PLUGINSDIR\$Exec.Patch\$1" \
				/ScratchDir:"$PLUGINSDIR\$Exec.Patch\Temp" \
				/LogPath:"$TEMP\LegacyUpdate-Dism.log" \
				/Quiet /NoRestart'
		${EndIf}
		Call ExecWithErrorHandling

		FindNext $0 $1
	${Loop}
	${EnableX64FSRedirection}
FunctionEnd

!macro InstallMSU kbid name
	StrCpy $Exec.Patch '${kbid}'
	StrCpy $Exec.Name '${name} (${kbid})'
	Call InstallMSU
!macroend

!macro EnsureAdminRights
	${IfNot} ${AtLeastWin2000}
		MessageBox MB_USERICON|MB_OKCANCEL "$(MsgBoxOldWinVersion)" /SD IDCANCEL \
			IDCANCEL +2
		ExecShell "" "${WUR_WEBSITE}"
		SetErrorLevel ${ERROR_OLD_WIN_VERSION}
		Quit
	${EndIf}

	LegacyUpdateNSIS::IsAdmin
	Pop $0
	${If} $0 == 0
		MessageBox MB_USERICON "$(MsgBoxElevationRequired)" /SD IDOK
		SetErrorLevel ${ERROR_ELEVATION_REQUIRED}
		Quit
	${EndIf}
!macroend

!macro InhibitSleep state
!if ${state} == 1
	System::Call '${SetThreadExecutionState}(${ES_CONTINUOUS}|${ES_SYSTEM_REQUIRED})'
!else
	System::Call '${SetThreadExecutionState}(${ES_CONTINUOUS})'
!endif
!macroend
