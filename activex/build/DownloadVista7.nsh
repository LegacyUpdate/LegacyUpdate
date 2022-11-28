Function DetermineComponentArch
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

!macro NeedsServicingPackage name version
	Call DetermineComponentArch
	Pop $0
	${If} ${FileExists} "$WINDIR\servicing\Packages\${name}~31bf3856ad364e35~$0~~${version}.mum"
		Push 0
	${Else}
		Push 1
	${EndIf}
!macroend

Function NeedsVistaSP1
	; Server 2008 RTM is equivalent to Vista SP1.
	${If} ${IsWinVista}
	${AndIf} ${AtMostServicePack} 0
	${AndIfNot} ${IsServerOS}
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsVistaSP2
	${If} ${IsWinVista}
	${AndIf} ${AtMostServicePack} 1
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsWin7SP1
	${If} ${IsWin7}
	${AndIf} ${AtMostServicePack} 0
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsKB3205638
	!insertmacro NeedsServicingPackage "Package_for_KB3205638" "6.0.1.0"
FunctionEnd

Function NeedsKB4012583
	!insertmacro NeedsServicingPackage "Package_for_KB4012583" "6.0.1.2"
FunctionEnd

Function NeedsKB4015195
	!insertmacro NeedsServicingPackage "Package_for_KB4015195" "6.0.1.0"
FunctionEnd

Function NeedsKB4015380
	!insertmacro NeedsServicingPackage "Package_for_KB4015380" "6.0.1.0"
FunctionEnd

Function NeedsKB3138612
	!insertmacro NeedsServicingPackage "Package_for_KB3138612" "6.1.1.1"
FunctionEnd

Function NeedsVistaPostSP2
	Call NeedsKB3205638
	Call NeedsKB4012583
	Call NeedsKB4015195
	Call NeedsKB4015380
	Pop $0
	Pop $1
	Pop $2
	Pop $3
	${If} $0 == 1
	${OrIf} $1 == 1
	${OrIf} $2 == 1
	${OrIf} $3 == 1
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function DownloadVistaSP1
	Call NeedsVistaSP1
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini VistaSP1 $0
		!insertmacro DownloadAndInstallSP "Windows Vista Service Pack 1" "$1" "vistasp1"
	${EndIf}
FunctionEnd

Function DownloadVistaSP2
	Call NeedsVistaSP2
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini VistaSP2 $0
		!insertmacro DownloadAndInstallSP "Windows Vista Service Pack 2" "$1" "vistasp2"
	${EndIf}
FunctionEnd

Function DownloadWin7SP1
	Call NeedsWin7SP1
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini Win7SP1 $0
		!insertmacro DownloadAndInstallSP "Windows 7 Service Pack 1" "$1" "win7sp1"
	${EndIf}
FunctionEnd

Function DownloadKB3205638
	Call NeedsKB3205638
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB3205638 $0
		!insertmacro DownloadAndInstallMSU "KB3205638" "$1"
	${EndIf}
FunctionEnd

Function DownloadKB4012583
	Call NeedsKB4012583
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB4012583 $0
		!insertmacro DownloadAndInstallMSU "KB4012583" "$1"
	${EndIf}
FunctionEnd

Function DownloadKB4015195
	Call NeedsKB4015195
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB4015195 $0
		!insertmacro DownloadAndInstallMSU "KB4015195" "$1"
	${EndIf}
FunctionEnd

Function DownloadKB4015380
	Call NeedsKB4015380
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB4015380 $0
		!insertmacro DownloadAndInstallMSU "KB4015380" "$1"
	${EndIf}
FunctionEnd

Function DownloadKB3138612
	Call NeedsKB3138612
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB3138612 $0
		!insertmacro DownloadAndInstallMSU "KB3138612" "$1"
	${EndIf}
FunctionEnd
