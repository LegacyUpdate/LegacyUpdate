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
		${EndIf}
	${EndIf}
FunctionEnd

Function DownloadWUA
	Call DetermineWUAVersion
	${If} $0 != ""
		SetOutPath $PLUGINSDIR
		DetailPrint "Downloading Windows Update Agent..."
		NSISdl::download "$0" "WindowsUpdateAgent.exe"
		DetailPrint "Installing Windows Update Agent..."
		ExecWait "WindowsUpdateAgent.exe /quiet /norestart" $0
		${If} $0 == ${ERROR_SUCCESS_REBOOT_REQUIRED}
			SetRebootFlag true
		${ElseIf} $0 != 0
			MessageBox MB_OK|MB_USERICON "Internet Explorer 6 SP1 failed to install. Error code: $0"
			Abort
		${EndIf}
		Delete "WindowsUpdateAgent.exe"
	${EndIf}
FunctionEnd
