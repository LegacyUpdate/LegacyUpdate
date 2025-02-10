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
			StrCpy $0 "$Patch.Title"
			MessageBox MB_USERICON "$(MsgBoxPatchNotFound)" /SD IDOK
			SetErrorLevel 1
			Abort
		${EndIf}
	${EndIf}
	ReadINIStr $1 $PLUGINSDIR\Patches.ini "$Patch.Key" Prefix
	!insertmacro Download "$Patch.Title" "$1$0" "$Patch.File" 1
FunctionEnd

!macro PatchHandler kbid title params
	Function Download${kbid}
		${If} ${NeedsPatch} ${kbid}
			StrCpy $Patch.Key   "${kbid}"
			StrCpy $Patch.File  "${kbid}.exe"
			StrCpy $Patch.Title "${title}"
			Call -PatchHandler
		${EndIf}
	FunctionEnd

	Function Install${kbid}
		${If} ${NeedsPatch} ${kbid}
			Call Download${kbid}
			!insertmacro Install "${title}" "${kbid}.exe" "${params}"
		${EndIf}
	FunctionEnd
!macroend

!if ${NT4} == 0
!insertmacro NeedsSPHandler "W2KSP4"  "Win2000"   2
!insertmacro NeedsSPHandler "XPSP1a"  "WinXP2002" 0
!insertmacro NeedsSPHandler "XPSP3"   "WinXP2002" 2
!insertmacro NeedsSPHandler "XPESP3"  "WinXP2002" 2
!insertmacro NeedsSPHandler "2003SP2" "WinXP2003" 1

!insertmacro NeedsFileVersionHandler "KB835732" "kernel32.dll" "5.00.2195.6897"

!insertmacro PatchHandler "W2KSP4"   "Windows 2000 $(SP) 4"            "-u -z"
!insertmacro PatchHandler "KB835732" "Windows 2000 KB835732 $(Update)" "/passive /norestart"
!insertmacro PatchHandler "XPSP1a"   "Windows XP $(SP) 1a"             "-u -z"
!insertmacro PatchHandler "XPSP3"    "Windows XP $(SP) 3"              "/passive /norestart"
!insertmacro PatchHandler "2003SP2"  "Windows XP $(P64)/$(SRV) 2003 $(SP) 2" "/passive /norestart"
!insertmacro PatchHandler "XPESP3"   "Windows XP $(EMB) $(SP) 3"       "/passive /norestart"
!endif

!insertmacro NeedsFileVersionHandler "IE6"      "mshtml.dll"   "6.0.2600.0"

!macro DownloadIE ver title
	${If} ${NeedsPatch} IE${ver}
		StrCpy $Patch.Key   "IE${ver}"
		StrCpy $Patch.File  "ie${ver}setup.exe"
		StrCpy $Patch.Title "${title} $(Setup)"
		Call -PatchHandler

		${IfNot} ${FileExists} "$PLUGINSDIR\IE${ver}\ie${ver}setup.exe"
			${DetailPrint} "$(Downloading)${title}..."
			!insertmacro ExecWithErrorHandling '${title}' '"$PLUGINSDIR\IE${ver}\ie${ver}setup.exe" /c:"ie${ver}wzd.exe /q /d /s:""#e"""'
		${EndIf}
	${EndIf}
!macroend

!macro InstallIE ver title
	${If} ${NeedsPatch} IE${ver}
		Call DownloadIE${ver}
		${DetailPrint} "$(Installing)${title}..."
		!insertmacro ExecWithErrorHandling '${title}' '"$PLUGINSDIR\IE${ver}\ie${ver}setup.exe" /c:"ie${ver}wzd.exe /q /r:n /s:""#e"""'
		RMDir /r /REBOOTOK "$WINDIR\Windows Update Setup Files"
	${EndIf}
!macroend

Function DownloadIE6
	!insertmacro DownloadIE 6 "$(IE) 6 $(SP) 1"
FunctionEnd

Function InstallIE6
	!insertmacro InstallIE 6 "$(IE) 6 $(SP) 1"
FunctionEnd
