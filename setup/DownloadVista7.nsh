Function GetComponentArch
	Call GetArch
	Pop $0
	${If} $0 == "x64"
		StrCpy $0 "amd64"
	${EndIf}
FunctionEnd

!macro SPHandler kbid title os sp
	!insertmacro NeedsSPHandler "${kbid}" "${os}" "${sp}"

	Function Download${kbid}
		Call Needs${kbid}
		Pop $0
		${If} $0 == 1
			Call GetArch
			Pop $0
			ReadINIStr $0 $PLUGINSDIR\Patches.ini "${kbid}" $0
			ReadINIStr $1 $PLUGINSDIR\Patches.ini "${kbid}" Prefix
			!insertmacro Download "${title}" "$1$0" "${kbid}.exe" 1
		${EndIf}
	FunctionEnd

	Function Install${kbid}
		Call Needs${kbid}
		Pop $0
		${If} $0 == 1
			Call Download${kbid}
			!insertmacro InstallSP "${title}" "${kbid}.exe"
		${EndIf}
	FunctionEnd
!macroend

!macro MSUHandler kbid title
	Function Needs${kbid}
		Call GetComponentArch
		Pop $0
		ClearErrors
		EnumRegKey $1 HKLM "${REGPATH_CBS_PACKAGEINDEX}\Package_1_for_${kbid}~31bf3856ad364e35~$0~~0.0.0.0" 0
		${If} ${Errors}
			Push 1
		${Else}
			Push 0
		${EndIf}
	FunctionEnd

	Function Download${kbid}
		Call Needs${kbid}
		Pop $0
		${If} $0 == 1
			Call GetArch
			Pop $0
			ReadINIStr $1 $PLUGINSDIR\Patches.ini "${kbid}" $0
			ReadINIStr $2 $PLUGINSDIR\Patches.ini "${kbid}" Prefix
			!insertmacro DownloadMSU "${kbid}" "${title}" "$2$1"
		${EndIf}
	FunctionEnd

	Function Install${kbid}
		Call Needs${kbid}
		Pop $0
		${If} $0 == 1
			Call Download${kbid}
			!insertmacro InstallMSU "${kbid}" "${title}"
		${EndIf}
	FunctionEnd
!macroend

; Service Packs
!insertmacro SPHandler  "VistaSP1"  "Windows Vista Service Pack 1" "WinVista" 0
!insertmacro SPHandler  "VistaSP2"  "Windows Vista Service Pack 2" "WinVista" 1
!insertmacro SPHandler  "Win7SP1"   "Windows 7 Service Pack 1"     "Win7"     0

; Windows Vista post-SP2 update combination that fixes WU indefinitely checking for updates
!insertmacro MSUHandler "KB3205638" "Security Update for Windows Vista"
!insertmacro MSUHandler "KB4012583" "Security Update for Windows Vista"
!insertmacro MSUHandler "KB4015195" "Security Update for Windows Vista"
!insertmacro MSUHandler "KB4015380" "Security Update for Windows Vista"

; Internet Explorer 9 for Windows Vista
!insertmacro MSUHandler "KB971512"  "Update for Windows Vista"
!insertmacro MSUHandler "KB2117917" "Platform Update Supplement for Windows Vista"

!insertmacro NeedsFileVersionHandler "IE9" "mshtml.dll" "9.0.8112.16421"
!insertmacro PatchHandler "IE9" "Internet Explorer 9 for Windows Vista" "/passive /norestart /update-no /closeprograms"

; Windows Vista Servicing Stack Update
!insertmacro MSUHandler "KB4493730" "2019-04 Servicing Stack Update for Windows Server 2008"

; Windows 7 Servicing Stack Update
!insertmacro MSUHandler "KB3138612" "2016-03 Servicing Stack Update for Windows 7"
!insertmacro MSUHandler "KB4474419" "SHA-2 Code Signing Support Update for Windows 7"
!insertmacro MSUHandler "KB4490628" "2019-03 Servicing Stack Update for Windows 7"

; Windows Home Server 2011 Update Rollup 4
!insertmacro MSUHandler "KB2757011" "Windows Home Server 2011 Update Rollup 4"

Function NeedsVistaPostSP2
	Call NeedsKB3205638
	Call NeedsKB4012583
	Call NeedsKB4015195
	Call NeedsKB4015380
	Call NeedsKB4493730
	Pop $0
	Pop $1
	Pop $2
	Pop $3
	Pop $4
	${If} $0 == 1
	${OrIf} $1 == 1
	${OrIf} $2 == 1
	${OrIf} $3 == 1
	${OrIf} $4 == 1
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsWin7SHA2
	Call NeedsKB3138612
	Call NeedsKB4474419
	Call NeedsKB4490628
	Pop $0
	Pop $1
	Pop $2
	${If} $0 == 1
	${OrIf} $1 == 1
	${OrIf} $2 == 1
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd
