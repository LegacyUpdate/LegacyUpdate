Function GetComponentArch
	${If} ${IsNativeIA32}
		Push "x86"
	${ElseIf} ${IsNativeAMD64}
		Push "amd64"
	${ElseIf} ${IsNativeIA64}
		Push "ia64"
	${Else}
		Push ""
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
			!insertmacro DownloadAndInstallSP "${title}" "$0" "${kbid}"
		${EndIf}
	FunctionEnd
!macroend

!macro MSUHandler kbid title packagename packageversion
	Function Needs${kbid}
		Call GetComponentArch
		Pop $0
		${If} ${FileExists} "$WINDIR\servicing\Packages\${packagename}~31bf3856ad364e35~$0~~${packageversion}.mum"
			Push 0
		${Else}
			Push 1
		${EndIf}
	FunctionEnd

	Function Download${kbid}
		Call Needs${kbid}
		Pop $0
		${If} $0 == 1
			Call GetArch
			Pop $0
			ReadINIStr $1 $PLUGINSDIR\Patches.ini "${kbid}" $0
			!insertmacro DownloadAndInstallMSU "${kbid}" "${title}" "$1"
		${EndIf}
	FunctionEnd
!macroend

; Service Packs
!insertmacro SPHandler  "VistaSP1"  "Windows Vista Service Pack 1" "WinVista" 0
!insertmacro SPHandler  "VistaSP2"  "Windows Vista Service Pack 2" "WinVista" 1
!insertmacro SPHandler  "Win7SP1"   "Windows 7 Service Pack 1"     "Win7"     0

; Windows Vista post-SP2 update combination that fixes WU indefinitely checking for updates
!insertmacro MSUHandler "KB3205638" "Security Update for Windows Vista"                         "Package_for_KB3205638"   "6.0.1.0"
!insertmacro MSUHandler "KB4012583" "Security Update for Windows Vista"                         "Package_for_KB4012583"   "6.0.1.2"
!insertmacro MSUHandler "KB4015195" "Security Update for Windows Vista"                         "Package_for_KB4015195"   "6.0.1.0"
!insertmacro MSUHandler "KB4015380" "Security Update for Windows Vista"                         "Package_for_KB4015380"   "6.0.1.0"

; Windows Vista Servicing Stack Update
!insertmacro MSUHandler "KB4493730" "2019-04 Servicing Stack Update for Windows Server 2008"    "Package_1_for_KB4493730" "6.0.1.0"

; Windows 7 Servicing Stack Update
!insertmacro MSUHandler "KB3138612" "2016-03 Servicing Stack Update for Windows 7"              "Package_for_KB3138612"   "6.1.1.1"

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
