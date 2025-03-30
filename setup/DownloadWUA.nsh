Function DetermineWUAVersion
	; WUA refuses to install on 2000 Datacenter Server. Maybe we can hack around this in future.
	${If} ${IsWin2000}
	${AndIf} ${IsDatacenter}
		StrCpy $0 ""
		Return
	${EndIf}

	GetWinVer $1 Major
	GetWinVer $2 Minor
	GetWinVer $3 ServicePack

	; Handle scenarios where the SP will be updated by the time we install WUA.
	${If} "$1.$2" == "5.1"
		${If} ${SectionIsSelected} ${XPSP3}
		${OrIf} ${SectionIsSelected} ${XPESP3}
			StrCpy $3 "3"
		${EndIf}
	${ElseIf} "$1.$2" == "5.2"
		${If} ${SectionIsSelected} ${2003SP2}
			StrCpy $3 "2"
		${EndIf}
	${EndIf}

	StrCpy $1 "$1.$2.$3"

	; Hardcoded special case for XP Home/Embedded SP3, because the WUA 7.6.7600.256 setup SFX is seriously broken on it,
	; potentially causing an unbootable Windows install due to it entering an infinite loop of creating folders in the
	; root of C:.
	${If} $1 == "5.1.3"
		${If} ${IsHomeEdition}
		${OrIf} ${IsEmbedded}
			StrCpy $1 "$1-home"
		${EndIf}
	${EndIf}

	StrCpy $0 ""
	ReadINIStr $2 $PLUGINSDIR\Patches.ini WUA $1
	${If} $2 == ""
		Return
	${EndIf}

	${GetFileVersion} "$SYSDIR\wuaueng.dll" $1
	${VersionCompare} $1 $2 $3
	${If} $3 == 2
		Call GetArch
		Pop $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini WUA $2-$0
		${If} $0 == ""
			Return
		${EndIf}

		ReadINIStr $1 $PLUGINSDIR\Patches.ini WUA Prefix
		StrCpy $0 "$1$0"
	${EndIf}
FunctionEnd

Function NeedsWUA
	Call DetermineWUAVersion
	${If} $0 == ""
		Push 0
	${Else}
		Push 1
	${EndIf}
FunctionEnd

Function DownloadWUA
	Call DetermineWUAVersion
	${If} $0 != ""
		!insertmacro Download "$(WUA)" "$0" "WindowsUpdateAgent.exe" 1
	${EndIf}
FunctionEnd

Function InstallWUA
	Call DetermineWUAVersion
	${If} $0 != ""
		Call DownloadWUA
		!insertmacro Install "$(WUA)" "WindowsUpdateAgent.exe" "/quiet /norestart"
	${EndIf}
FunctionEnd
