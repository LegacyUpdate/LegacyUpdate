Function DetermineWUAVersion
	StrCpy $0 ""

	GetWinVer $2 Major
	GetWinVer $3 Minor
	GetWinVer $4 ServicePack
	StrCpy $5 "$2.$3.$4"

	ClearErrors
	ReadINIStr $6 $PLUGINSDIR\Patches.ini WUA $5
	${If} ${Errors}
		Return
	${EndIf}

	${GetFileVersion} "$SYSDIR\wuapi.dll" $1
	${VersionCompare} $1 $6 $7
	${If} $7 == 2
		${If} ${IsNativeIA32}
			ReadINIStr $0 $PLUGINSDIR\Patches.ini WUA $6-x86
		${ElseIf} ${IsNativeAMD64}
			ReadINIStr $0 $PLUGINSDIR\Patches.ini WUA $6-x64
		${ElseIf} ${IsNativeIA64}
			ReadINIStr $0 $PLUGINSDIR\Patches.ini WUA $6-ia64
		${EndIf}
	${EndIf}
FunctionEnd

Function DownloadWUA
	Call DetermineWUAVersion
	${If} $0 != ""
		SetOutPath $PLUGINSDIR
		!insertmacro DownloadAndInstall "Windows Update Agent" "$0" "WindowsUpdateAgent.exe" "/quiet /norestart"
	${EndIf}
FunctionEnd
