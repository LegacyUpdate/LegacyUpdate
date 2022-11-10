!define W2K_IE6 "http://content.legacyupdate.net/legacyupdate/ie/ie6/ie6sp1_w2k.exe"

Function GetUpdateLanguage
	ReadRegStr $1 HKLM "Hardware\Description\System" "Identifier"
	${If} $1 == "NEC PC-98"
		StrCpy $0 "NEC98"
		Return
	${EndIf}

	StrCpy $0 "ENU"
	ReadRegStr $1 HKLM "System\CurrentControlSet\Control\Nls\Language" "InstallLanguage"
	ReadINIStr $0 $PLUGINSDIR\Patches.ini Language $1
FunctionEnd

Function DownloadW2KSP4
	${If} ${IsWin2000}
	${AndIf} ${AtMostServicePack} 2
		Call GetUpdateLanguage
		ReadINIStr $1 $PLUGINSDIR\Patches.ini W2KSP4 $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini W2KSP4 Prefix
		!insertmacro DownloadAndInstall "Windows 2000 Service Pack 4" "$0$1" "w2ksp4.exe" "-u -z"
	${EndIf}
FunctionEnd

Function DownloadKB835732
	${GetFileVersion} "$SYSDIR\kernel32.dll" $0
	${VersionCompare} $0 "5.00.2195.7000" $1
	${If} $1 == 2
		Call GetUpdateLanguage
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB835732 $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini KB835732 Prefix
		!insertmacro DownloadAndInstall "Windows 2000 KB835732 Update" "$0$1" "kb835732.exe" "/passive /norestart"
	${EndIf}
FunctionEnd

Function DownloadIE6
	${GetFileVersion} "$SYSDIR\mshtml.dll" $0
	${VersionCompare} $0 "6.0.2600.0" $1
	${If} $1 == 2
		!insertmacro DownloadAndInstall "Internet Explorer 6 SP1" "${W2K_IE6}" "ie6setup.exe" '/q /c:"ie6setup.exe /q"'
	${EndIf}
FunctionEnd
