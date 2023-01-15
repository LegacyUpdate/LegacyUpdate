Function DetermineWUAVersion
	; Hardcoded special case for XP Home SP3, because the WUA 7.6.7600.256 setup SFX is seriously
	; broken on it, potentially causing an unbootable Windows install due to it entering an infinite
	; loop of creating folders in the root of C:.
	${If} ${IsWinXP2002}
	${AndIf} ${AtLeastServicePack} 3
	${AndIf} ${IsHomeEdition}
		StrCpy $1 "5.1.3-home"
	${Else}
		GetWinVer $1 Major
		GetWinVer $2 Minor
		GetWinVer $3 ServicePack
		StrCpy $1 "$1.$2.$3"
	${EndIf}

	StrCpy $0 ""

	ClearErrors
	ReadINIStr $2 $PLUGINSDIR\Patches.ini WUA $1
	${If} ${Errors}
		Return
	${EndIf}

	${GetFileVersion} "$SYSDIR\wuapi.dll" $1
	${VersionCompare} $1 $2 $3
	${If} $3 == 2
		Call GetArch
		Pop $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini WUA $2-$0
	${EndIf}
FunctionEnd

Function DownloadWUA
	Call DetermineWUAVersion
	${If} $0 != ""
		!insertmacro DownloadAndInstall "Windows Update Agent" "$0" "WindowsUpdateAgent.exe" "/quiet /norestart"
	${EndIf}
FunctionEnd
