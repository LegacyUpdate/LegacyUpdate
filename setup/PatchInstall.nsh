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

!macro _NeedsPatch _a _b _t _f
	!insertmacro _LOGICLIB_TEMP
	Call Needs${_b}
	Pop $_LOGICLIB_TEMP
	StrCmp $_LOGICLIB_TEMP 1 `${_t}` `${_f}`
!macroend

!define NeedsPatch `"" NeedsPatch`

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
	${If} $1 < 200
	${OrIf} $1 >= 300
		${If} $1 != ${ERROR_INTERNET_OPERATION_CANCELLED}
			StrCpy $2 "${name}"
			MessageBox MB_USERICON "$(MsgBoxDownloadFailed)" /SD IDOK
		${EndIf}
		Delete /REBOOTOK "${filename}"
		SetErrorLevel 1
		Abort
	${EndIf}
!macroend

!macro Download name url filename hash verbose
	${IfNot} ${FileExists} "${RUNONCEDIR}\${filename}"
		${If} ${FileExists} "$EXEDIR\${filename}"
			CopyFiles /SILENT "$EXEDIR\${filename}" "${RUNONCEDIR}\${filename}"
		${Else}
			!insertmacro -Download '${name}' '${url}' '${RUNONCEDIR}\${filename}' ${verbose}
		${EndIf}
	${EndIf}
	StrCpy $0 "${RUNONCEDIR}\${filename}"

!if "${hash}" != ""
	${DetailPrint} "$(Verifying)${name}..."
	LegacyUpdateNSIS::VerifyFileHash "$0" "${hash}"
	Pop $R0
	Pop $R1
	${If} ${IsVerbose}
		${VerbosePrint} "Expected: ${hash}"
		${VerbosePrint} "Actual: $R1"
	${EndIf}
	${If} $R0 != 1
		StrCpy $2 "${name}"
		MessageBox MB_USERICON "$(MsgBoxHashFailed)" /SD IDOK
		Delete /REBOOTOK "$0"
		SetErrorLevel 1
		Abort
	${EndIf}
!endif
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

Function GetUpdateLanguage
	Var /GLOBAL UpdateLanguage
	${If} $UpdateLanguage == ""
		ReadRegStr $UpdateLanguage HKLM "${REGPATH_HARDWARE_SYSTEM}" "Identifier"
		${If} $UpdateLanguage == "NEC PC-98"
			StrCpy $UpdateLanguage "NEC98"
		${Else}
			ReadRegStr $UpdateLanguage HKLM "${REGPATH_CONTROL_LANGUAGE}" "InstallLanguage"
			ReadINIStr $UpdateLanguage $PLUGINSDIR\Patches.ini Language $UpdateLanguage
		${EndIf}
	${EndIf}
	Push $UpdateLanguage
FunctionEnd

!macro NeedsSPHandler name os sp
	Function Needs${name}
		${If} ${Is${os}}
		${AndIf} ${AtMostServicePack} ${sp}
			Push 1
		${Else}
			Push 0
		${EndIf}
	FunctionEnd
!macroend

!macro NeedsFileVersionHandler name file version
	Function Needs${name}
		${GetFileVersion} "$SYSDIR\${file}" $0
		${VersionCompare} $0 ${version} $1
		${If} $1 == 2 ; Less than
			Push 1
		${Else}
			Push 0
		${EndIf}
	FunctionEnd
!macroend

!if ${NT4} == 1
Var /GLOBAL SPCleanup
!endif

Function SkipSPUninstall
!if ${NT4} == 1
	Push $SPCleanup
!else
	Push 0
!endif
FunctionEnd

Var /GLOBAL Patch.Key
Var /GLOBAL Patch.File
Var /GLOBAL Patch.Title

Function -PatchHandler
	Call GetUpdateLanguage
	Call GetArch
	Pop $1
	Pop $0
	StrCpy $2 "$0-$1"
	ClearErrors
	ReadINIStr $0 $PLUGINSDIR\Patches.ini "$Patch.Key" "$2"
	${If} ${Errors}
		; Language neutral
		StrCpy $2 "$1"
		ClearErrors
		ReadINIStr $0 $PLUGINSDIR\Patches.ini "$Patch.Key" "$2"
		${If} ${Errors}
			StrCpy $0 "$Patch.Title"
			MessageBox MB_USERICON "$(MsgBoxPatchNotFound)" /SD IDOK
			SetErrorLevel 1
			Abort
		${EndIf}
	${EndIf}
	ReadINIStr $1 $PLUGINSDIR\Patches.ini "$Patch.Key" Prefix
	ReadINIStr $3 $PLUGINSDIR\Patches.ini "$Patch.Key" "$2-sha256"
	!insertmacro Download "$Patch.Title" "$1$0" "$Patch.File" "$3" 1
FunctionEnd

!define PATCH_FLAGS_OTHER 0
!define PATCH_FLAGS_NT4   1
!define PATCH_FLAGS_SHORT 2
!define PATCH_FLAGS_LONG  3

!macro -PatchHandlerFlags params cleanup
!if ${DEBUG} == 1
	; To make testing go faster
	StrCpy $R0 "${params} ${cleanup}"
!else
	; NT4 branch will add a SkipSPUninstall setting. For now, we ignore the cleanup param.
	StrCpy $R0 "${params}"
!endif
!macroend

!macro PatchHandler kbid title type params
	Function Download${kbid}
		${If} ${NeedsPatch} ${kbid}
			StrCpy $Patch.Key   "${kbid}"
			StrCpy $Patch.File  "${kbid}.exe"
			StrCpy $Patch.Title "${title}"
			Call -PatchHandler
		${EndIf}
	FunctionEnd

	Function Install${kbid}
		${IfNot} ${NeedsPatch} ${kbid}
			Return
		${EndIf}

		Call Download${kbid}
		Call SkipSPUninstall
!if ${type} == ${PATCH_FLAGS_OTHER}
		StrCpy $R0 ""
!endif
!if ${type} == ${PATCH_FLAGS_NT4}
		!insertmacro -PatchHandlerFlags "-z"    "-n -o"
!endif
!if ${type} == ${PATCH_FLAGS_SHORT}
		!insertmacro -PatchHandlerFlags "-u -z" "-n -o"
!endif
!if ${type} == ${PATCH_FLAGS_LONG}
		!insertmacro -PatchHandlerFlags "/passive /norestart" "/n /o"
!endif
		!insertmacro Install "${title}" "${kbid}.exe" "$R0 ${params}"
	FunctionEnd
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

!macro DownloadMSU kbid name url sha256
	!insertmacro Download '${name} (${kbid})' '${url}' '${kbid}.msu' '${sha256}' 1
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
				/n:"$PLUGINSDIR\$Exec.Patch\$1" \
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
