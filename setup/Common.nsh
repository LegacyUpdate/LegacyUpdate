!addplugindir /x86-unicode x86-unicode

SetPluginUnload alwaysoff

!packhdr          upx.tmp 'upx --lzma -9 upx.tmp'

!if ${SIGN} == 1
	!finalize       '../build/sign.sh "%1"'
	!uninstfinalize '../build/sign.sh "%1"'
!endif

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
!define IsHelp    `"" HasFlag "/?"`

!macro _NeedsPatch _a _b _t _f
	!insertmacro _LOGICLIB_TEMP
	Call Needs${_b}
	Pop $_LOGICLIB_TEMP
	StrCmp $_LOGICLIB_TEMP 1 `${_t}` `${_f}`
!macroend

!define NeedsPatch `"" NeedsPatch`

!macro DetailPrint text
	SetDetailsPrint both
	DetailPrint "${text}"
	SetDetailsPrint listonly
!macroend

Var /GLOBAL Download.ID

Function DownloadRequest
	; TODO: This is broken on XP for some reason
	; Var /GLOBAL Download.UserAgent
	; ${If} $Download.UserAgent == ""
	; 	GetWinVer $8 Major
	; 	GetWinVer $9 Minor
	; 	StrCpy $Download.UserAgent "Mozilla/4.0 (${NAME} ${VERSION}; Windows NT $8.$9)"
	; ${EndIf}
	; /HEADER "User-Agent: $Download.UserAgent"

	NSxfer::Request \
		/TIMEOUTCONNECT 60000 \
		/TIMEOUTRECONNECT 60000 \
		/OPTCONNECTTIMEOUT 60000 \
		/OPTRECEIVETIMEOUT 60000 \
		/OPTSENDTIMEOUT 60000 \
		/URL "$0" \
		/LOCAL "$1" \
		/INTERNETFLAGS ${INTERNET_FLAG_RELOAD}|${INTERNET_FLAG_NO_CACHE_WRITE}|${INTERNET_FLAG_KEEP_CONNECTION}|${INTERNET_FLAG_NO_COOKIES}|${INTERNET_FLAG_NO_UI} \
		/SECURITYFLAGS ${SECURITY_FLAG_STRENGTH_STRONG} \
		$2 \
		/END
	Pop $Download.ID
FunctionEnd

!macro DownloadRequest url local extra
	Push $0
	Push $1
	Push $2
	StrCpy $0 "${url}"
	StrCpy $1 "${local}"
	StrCpy $2 "${extra}"
	Call DownloadRequest
	Pop $2
	Pop $1
	Pop $0
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
	${If} ${verbose} == 1
		!insertmacro DetailPrint "$(Downloading)${name}..."
	${EndIf}
	!insertmacro DownloadRequest "${url}" "${filename}" ""
	${If} ${verbose} == 1
		Call DownloadWait
	${Else}
		Call DownloadWaitSilent
	${EndIf}
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
	${If} ${FileExists} "$EXEDIR\${filename}"
		CopyFiles /SILENT "$EXEDIR\${filename}" "${RUNONCEDIR}\${filename}"
	${ElseIfNot} ${FileExists} "${RUNONCEDIR}\${filename}"
		!insertmacro -Download '${name}' '${url}' '${RUNONCEDIR}\${filename}' ${verbose}
	${EndIf}
	StrCpy $0 "${RUNONCEDIR}\${filename}"
!macroend

Var /GLOBAL Exec.Command
Var /GLOBAL Exec.Name

Function ExecWithErrorHandling
	Push $0
	LegacyUpdateNSIS::ExecToLog '$Exec.Command'
	Pop $0

	${If} $0 == ${ERROR_SUCCESS_REBOOT_REQUIRED}
		SetRebootFlag true
	${ElseIf} $0 == ${ERROR_INSTALL_USEREXIT}
		SetErrorLevel ${ERROR_INSTALL_USEREXIT}
		Abort
	${ElseIf} $0 == ${WU_S_ALREADY_INSTALLED}
		!insertmacro DetailPrint "$(AlreadyInstalled)"
	${ElseIf} $0 == ${WU_E_NOT_APPLICABLE}
		!insertmacro DetailPrint "$(NotApplicable)"
	${ElseIf} $0 != 0
		LegacyUpdateNSIS::MessageForHresult $0
		Pop $1
		StrCpy $2 "$Exec.Name"
		MessageBox MB_USERICON "$(MsgBoxInstallFailed)" /SD IDOK
		SetErrorLevel $0
		Abort
	${EndIf}
	Pop $0
FunctionEnd

!macro ExecWithErrorHandling name command
	StrCpy $Exec.Command '${command}'
	StrCpy $Exec.Name '${name}'
	Call ExecWithErrorHandling
!macroend

!macro Install name filename args
	!insertmacro DetailPrint "$(Installing)${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '"$0" ${args}'
!macroend

!macro InstallSP name filename
	; SPInstall.exe /norestart seems to be broken. We let it do a delayed restart, then cancel it.
	!insertmacro DetailPrint "$(Extracting)${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '"$0" /X:"$PLUGINSDIR\${filename}"'
	!insertmacro DetailPrint "$(Installing)${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '"$WINDIR\system32\cmd.exe" /c "$PLUGINSDIR\${filename}\spinstall.exe" /unattend /nodialog /warnrestart:600'

	; If we successfully abort a shutdown, we'll get exit code 0, so we know a reboot is required.
	ExecWait '"$WINDIR\system32\shutdown.exe" /a' $0
	${If} $0 == 0
		SetRebootFlag true
	${EndIf}
!macroend

!macro DownloadMSU kbid name url
	!insertmacro Download '${name} (${kbid})' '${url}' '${kbid}.msu' 1
!macroend

!macro InstallMSU kbid name
	; Stop AU service before running wusa so it doesn't try checking for updates online first (which
	; may never complete before we install our patches).
	!insertmacro DetailPrint "$(Extracting)${name} (${kbid})..."
	SetDetailsPrint none
	CreateDirectory "$PLUGINSDIR\${kbid}"
	CreateDirectory "$PLUGINSDIR\${kbid}\Temp"
	!insertmacro ExecWithErrorHandling '${name} (${kbid})' '"$WINDIR\system32\expand.exe" -F:* "$0" "$PLUGINSDIR\${kbid}"'
	SetDetailsPrint lastused

	!insertmacro DetailPrint "$(Installing)${name} (${kbid})..."
	${DisableX64FSRedirection}
	FindFirst $0 $1 "$PLUGINSDIR\${kbid}\*.xml"
	${Do}
		${If} $1 == ""
			FindClose $0
			${Break}
		${EndIf}

		; We prefer Dism, but need to fall back to Pkgmgr for Vista.
		${If} ${IsWinVista}
			!insertmacro ExecWithErrorHandling '${name} (${kbid})' '"$WINDIR\system32\pkgmgr.exe" \
				/n:"$PLUGINSDIR\${kbid}\$1" \
				/s:"$PLUGINSDIR\${kbid}\Temp" \
				/quiet /norestart'
		${Else}
			!insertmacro ExecWithErrorHandling '${name} (${kbid})' '"$WINDIR\system32\dism.exe" \
				/Online \
				/Apply-Unattend:"$PLUGINSDIR\${kbid}\$1" \
				/ScratchDir:"$PLUGINSDIR\${kbid}\Temp" \
				/LogPath:"$TEMP\LegacyUpdate-Dism.log" \
				/Quiet /NoRestart'
		${EndIf}

		FindNext $0 $1
	${Loop}
	${EnableX64FSRedirection}
!macroend

!macro EnsureAdminRights
	${IfNot} ${AtLeastWin2000}
		MessageBox MB_USERICON|MB_OKCANCEL "$(MsgBoxOldWinVersion)" /SD IDCANCEL \
			IDCANCEL +2
		ExecShell "" "${WUR_WEBSITE}"
		SetErrorLevel ${ERROR_OLD_WIN_VERSION}
		Quit
	${EndIf}

	System::Call '${IsUserAnAdmin}() .r0'
	${If} $0 == 0
		MessageBox MB_USERICON "$(MsgBoxElevationRequired)" /SD IDOK
		SetErrorLevel ${ERROR_ELEVATION_REQUIRED}
		Quit
	${EndIf}
!macroend

!macro InhibitSleep state
	${If} ${state} == 1
		System::Call '${SetThreadExecutionState}(${ES_CONTINUOUS}|${ES_SYSTEM_REQUIRED})'
	${Else}
		System::Call '${SetThreadExecutionState}(${ES_CONTINUOUS})'
	${EndIf}
!macroend
