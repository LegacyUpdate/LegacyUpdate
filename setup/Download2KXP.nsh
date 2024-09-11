Function GetUpdateLanguage
	Var /GLOBAL UpdateLanguage
	${If} $UpdateLanguage == ""
		ReadRegStr $UpdateLanguage HKLM "Hardware\Description\System" "Identifier"
		${If} $UpdateLanguage == "NEC PC-98"
			StrCpy $UpdateLanguage "NEC98"
		${Else}
			ReadRegStr $UpdateLanguage HKLM "System\CurrentControlSet\Control\Nls\Language" "InstallLanguage"
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

Var /GLOBAL Patch.Key
Var /GLOBAL Patch.File
Var /GLOBAL Patch.Title

Function -PatchHandler
	Call GetUpdateLanguage
	Call GetArch
	Pop $1
	Pop $0
	ClearErrors
	ReadINIStr $0 $PLUGINSDIR\Patches.ini "$Patch.Key" $0-$1
	${If} ${Errors}
		; Language neutral
		ClearErrors
		ReadINIStr $0 $PLUGINSDIR\Patches.ini "$Patch.Key" $1
		${If} ${Errors}
			MessageBox MB_USERICON "$Patch.Title could not be installed.$\r$\n$\r$\nThe installed Windows language and/or architecture is not supported." /SD IDOK
			SetErrorLevel 1
			Abort
		${EndIf}
	${EndIf}
	ReadINIStr $1 $PLUGINSDIR\Patches.ini "$Patch.Key" Prefix
	ReadINIStr $1 $PLUGINSDIR\Patches.ini Prefix $1
	!insertmacro Download "$Patch.Title" "$1$0" "$Patch.File" 1
FunctionEnd

!macro PatchHandler kbid title params
	Function Download${kbid}
		Call Needs${kbid}
		Pop $0
		${If} $0 == 1
			StrCpy $Patch.Key   "${kbid}"
			StrCpy $Patch.File  "${kbid}.exe"
			StrCpy $Patch.Title "${title}"
			Call -PatchHandler
		${EndIf}
	FunctionEnd

	Function Install${kbid}
		Call Needs${kbid}
		Pop $0
		${If} $0 == 1
			Call Download${kbid}
			!insertmacro Install "${title}" "${kbid}.exe" "${params}"
		${EndIf}
	FunctionEnd
!macroend

!insertmacro NeedsSPHandler "W2KSP4"  "Win2000"   2
!insertmacro NeedsSPHandler "XPSP2"   "WinXP2002" 0
!insertmacro NeedsSPHandler "XPSP3"   "WinXP2002" 2
!insertmacro NeedsSPHandler "XPESP3"  "WinXP2002" 2
!insertmacro NeedsSPHandler "2003SP2" "WinXP2003" 1

!insertmacro NeedsFileVersionHandler "KB835732" "kernel32.dll" "5.00.2195.6897"
!insertmacro NeedsFileVersionHandler "IE6"      "mshtml.dll"   "6.0.2600.0"

!insertmacro PatchHandler "W2KSP4"   "Windows 2000 Service Pack 4"                       "-u -z"
!insertmacro PatchHandler "KB835732" "Windows 2000 KB835732 Update"                      "/passive /norestart"
!insertmacro PatchHandler "XPSP2"    "Windows XP Service Pack 2"                         "/passive /norestart"
!insertmacro PatchHandler "XPSP3"    "Windows XP Service Pack 3"                         "/passive /norestart"
!insertmacro PatchHandler "2003SP2"  "Windows XP x64 Edition/Server 2003 Service Pack 2" "/passive /norestart"
!insertmacro PatchHandler "XPESP3"   "Windows XP Embedded Service Pack 3"                "/passive /norestart"

Function DownloadIE6
	Call NeedsIE6
	Pop $0
	${If} $0 == 1
		StrCpy $Patch.Key   "W2KIE6"
		StrCpy $Patch.File  "ie6sp1.cab"
		StrCpy $Patch.Title "Internet Explorer 6 SP1"
		Call -PatchHandler
	${EndIf}
FunctionEnd

Function InstallIE6
	Call NeedsIE6
	Pop $0
	${If} $0 == 1
		Call DownloadIE6
		!insertmacro DetailPrint "Extracting Internet Explorer 6 SP1..."
		ExecShellWait "" "$WINDIR\system32\expand.exe" '"$OUTDIR\ie6sp1.cab" -F:ie6setup.exe "$PLUGINSDIR"' SW_HIDE
		ExecShellWait "" "$WINDIR\system32\expand.exe" '"$OUTDIR\ie6sp1.cab" -F:iebatch.txt "$PLUGINSDIR"' SW_HIDE
		!insertmacro DetailPrint "Installing Internet Explorer 6 SP1..."
		!insertmacro ExecWithErrorHandling 'Internet Explorer 6 SP1' '"$PLUGINSDIR\ie6setup.exe" /q'
	${EndIf}
FunctionEnd
