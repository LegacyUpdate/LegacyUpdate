!addplugindir /x86-unicode x86-unicode

!if ${SIGN} == 1
	!finalize       '../build/sign.sh "%1"'
	!uninstfinalize '../build/sign.sh "%1"'
!endif

!define IsNativeIA64 '${IsNativeMachineArchitecture} ${IMAGE_FILE_MACHINE_IA64}'

Function GetArch
	${If} ${IsNativeIA32}
		Push "x86"
	${ElseIf} ${IsNativeAMD64}
		Push "x64"
	${ElseIf} ${IsNativeIA64}
		Push "ia64"
	${Else}
		Push ""
	${EndIf}
FunctionEnd

!macro _HasFlag _a _b _t _f
	!insertmacro _LOGICLIB_TEMP
	${GetParameters} $_LOGICLIB_TEMP
	ClearErrors
	${GetOptions} $_LOGICLIB_TEMP `${_b}` $_LOGICLIB_TEMP
	IfErrors `${_f}` `${_t}`
!macroend

!macro DetailPrint text
	SetDetailsPrint both
	DetailPrint "${text}"
	SetDetailsPrint listonly
!macroend

!macro DownloadRequest url local extra
	NSxfer::Request \
		/TIMEOUTCONNECT 60000 \
		/TIMEOUTRECONNECT 60000 \
		/OPTCONNECTTIMEOUT 60000 \
		/OPTRECEIVETIMEOUT 60000 \
		/OPTSENDTIMEOUT 60000 \
		/URL "${url}" \
		/LOCAL "${local}" \
		/INTERNETFLAGS ${INTERNET_FLAG_RELOAD}|${INTERNET_FLAG_NO_CACHE_WRITE}|${INTERNET_FLAG_KEEP_CONNECTION}|${INTERNET_FLAG_NO_COOKIES}|${INTERNET_FLAG_NO_UI} \
		/SECURITYFLAGS ${SECURITY_FLAG_STRENGTH_STRONG} \
		${extra} \
		/END
!macroend

!macro DownloadWait id mode
	NSxfer::Wait \
		/ID ${id} \
		/MODE ${mode} \
		/STATUSTEXT \
			"{TIMEREMAINING} left - {RECVSIZE} of {FILESIZE} ({SPEED})" \
			"{TIMEREMAINING} left - {TOTALRECVSIZE} of {TOTALFILESIZE} ({SPEED})" \
		/ABORT "Legacy Update" "Cancelling will terminate Legacy Update setup." \
		/END
	NSxfer::Query \
		/ID ${id} \
		/ERRORCODE /ERRORTEXT \
		/END
!macroend

!macro -Download name url filename
	!insertmacro DetailPrint "Downloading ${name}..."
	!insertmacro DownloadRequest "${url}" "${filename}" ""
	Pop $0
	!insertmacro DownloadWait $0 PAGE
	Pop $1
	Pop $0
	${If} $0 != "OK"
		${If} $1 != ${ERROR_INTERNET_OPERATION_CANCELLED}
			MessageBox MB_USERICON "${name} failed to download.$\r$\n$\r$\n$0 ($1)" /SD IDOK
		${EndIf}
		Delete /REBOOTOK "${filename}"
		SetErrorLevel 1
		Abort
	${EndIf}
!macroend

!macro ExecWithErrorHandling name command iswusa
	ExecWait '${command}' $0
	${If} $0 == ${ERROR_SUCCESS_REBOOT_REQUIRED}
		SetRebootFlag true
	${ElseIf} $0 == ${ERROR_INSTALL_USEREXIT}
		SetErrorLevel ${ERROR_INSTALL_USEREXIT}
		Abort
	${ElseIf} ${iswusa} == 1
	${AndIf} $0 == 1
		; wusa exits with 1 if the patch is already installed. Treat this as success.
		DetailPrint "Installation skipped - already installed"
		Return
	${ElseIf} ${iswusa} == 1
	${AndIf} $0 == ${WU_S_ALREADY_INSTALLED}
		DetailPrint "Installation skipped - already installed"
		Return
	${ElseIf} ${iswusa} == 1
	${AndIf} $0 == ${WU_E_NOT_APPLICABLE}
		DetailPrint "Installation skipped - not applicable"
		Return
	${ElseIf} $0 != 0
		LegacyUpdateNSIS::MessageForHresult $0
		Pop $1
		MessageBox MB_USERICON "${name} failed to install.$\r$\n$\r$\n$1 ($0)" /SD IDOK
		SetErrorLevel $0
		Abort
	${EndIf}
!macroend

!macro Download name url filename
	${If} ${FileExists} "$EXEDIR\${filename}"
		${If} $OUTDIR != "$EXEDIR"
			SetOutPath "$EXEDIR"
		${EndIf}
		StrCpy $0 "$EXEDIR\${filename}"
	${Else}
		${If} $OUTDIR != "$RunOnceDir"
			SetOutPath "$RunOnceDir"
		${EndIf}
		${IfNot} ${FileExists} "$RunOnceDir\${filename}"
			!insertmacro -Download '${name}' '${url}' '$RunOnceDir\${filename}'
		${EndIf}
		StrCpy $0 "$RunOnceDir\${filename}"
	${EndIf}
!macroend

!macro Install name filename args
	!insertmacro DetailPrint "Installing ${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '"$0" ${args}' 0
!macroend

!macro InstallSP name filename
	; SPInstall.exe /norestart seems to be broken. We let it do a delayed restart, then cancel it.
	!insertmacro DetailPrint "Extracting ${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '"$0" /X:"$PLUGINSDIR\${filename}"' 0
	!insertmacro DetailPrint "Installing ${name}..."
	!insertmacro ExecWithErrorHandling '${name}' '"$PLUGINSDIR\${filename}\spinstall.exe" /unattend /nodialog /warnrestart:600' 0

	; If we successfully abort a shutdown, we'll get exit code 0, so we know a reboot is required.
	ExecWait "$WINDIR\system32\shutdown.exe /a" $0
	${If} $0 == 0
		SetRebootFlag true
	${EndIf}
!macroend

!macro DownloadMSU kbid name url
	!insertmacro Download '${name} (${kbid})' '${url}' '${kbid}.msu'
!macroend

!macro InstallMSU kbid name
	; Stop AU service before running wusa so it doesn't try checking for updates online first (which
	; may never complete before we install our patches).
	!insertmacro DetailPrint "Installing ${name} (${kbid})..."
	SetDetailsPrint none
	ExecShellWait "" "$WINDIR\system32\net.exe" "stop wuauserv" SW_HIDE
	SetDetailsPrint listonly
	!insertmacro ExecWithErrorHandling '${name} (${kbid})' '$WINDIR\system32\wusa.exe /quiet /norestart "$0"' 1
!macroend

!macro EnsureAdminRights
	${IfNot} ${AtLeastWin2000}
		MessageBox MB_USERICON "Legacy Update requires at least Windows 2000." /SD IDOK
		SetErrorLevel ${ERROR_OLD_WIN_VERSION}
		Quit
	${EndIf}

	System::Call '${IsUserAnAdmin}() .r0'
	${If} $0 != 1
		MessageBox MB_USERICON "Log on as an administrator to install Legacy Update." /SD IDOK
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

!macro TryWithRetry command error
	ClearErrors
	${command}
	IfErrors 0 +3
		MessageBox MB_RETRYCANCEL|MB_USERICON \
			'${error}$\r$\n$\r$\nIf Internet Explorer is open, close it and click Retry.' \
			/SD IDCANCEL \
			IDRETRY -3
		Abort
!macroend

!macro TryFile file oname
	!insertmacro TryWithRetry `File "/ONAME=${oname}" "${file}"` 'Unable to write to "${oname}".'
!macroend

!macro TryDelete file
	!insertmacro TryWithRetry `Delete "${file}"` 'Unable to delete "${file}".'
!macroend

!macro TryRename src dest
	!insertmacro TryWithRetry `Rename "${src}" "${dest}"` 'Unable to write to "${dest}".'
!macroend

!macro RegisterDLL un arch file
	${If} "${un}" == "Un"
		StrCpy $0 "/u"
	${Else}
		StrCpy $0 ""
	${EndIf}

	${If} "${arch}" == "x64"
		${DisableX64FSRedirection}
	${EndIf}

	ClearErrors
	ExecWait '"$WINDIR\system32\regsvr32.exe" /s $0 "${file}"'
	${If} ${Errors}
		; Do it again non-silently so the user can see the error.
		ExecWait '"$WINDIR\system32\regsvr32.exe" $0 "${file}"'
		${If} "${arch}" == "x64"
			${EnableX64FSRedirection}
		${EndIf}
		Abort
	${EndIf}

	${If} "${arch}" == "x64"
		${EnableX64FSRedirection}
	${EndIf}
!macroend
