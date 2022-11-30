!define W2K_IE6 "http://content.legacyupdate.net/legacyupdate/ie/ie6/ie6sp1_w2k.exe"

Function GetUpdateLanguage
	ReadRegStr $0 HKLM "Hardware\Description\System" "Identifier"
	${If} $0 == "NEC PC-98"
		Push "NEC98"
	${Else}
		ReadRegStr $0 HKLM "System\CurrentControlSet\Control\Nls\Language" "InstallLanguage"
		ReadINIStr $0 $PLUGINSDIR\Patches.ini Language $0
		Push $0
	${EndIf}
FunctionEnd

Function NeedsW2KSP4
	${If} ${IsWin2000}
	${AndIf} ${AtMostServicePack} 2
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsKB835732
	${GetFileVersion} "$SYSDIR\kernel32.dll" $0
	${VersionCompare} $0 "5.00.2195.6897" $1
	${If} $1 == 2
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsIE6
	${GetFileVersion} "$SYSDIR\mshtml.dll" $0
	${VersionCompare} $0 "6.0.2600.0" $1
	${If} $1 == 2
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsXPSP2
	${If} ${IsWinXP2002}
	${AndIf} ${AtMostServicePack} 0
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsXPSP3
	${If} ${IsWinXP2002}
	${AndIf} ${AtMostServicePack} 2
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function Needs2003SP2
	${If} ${IsWinXP2003}
	${AndIf} ${AtMostServicePack} 1
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function DownloadW2KSP4
	Call NeedsW2KSP4
	Pop $0
	${If} $0 == 1
		Call GetUpdateLanguage
		Pop $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini W2KSP4 $0
		!insertmacro DownloadAndInstall "Windows 2000 Service Pack 4" "$0" "w2ksp4.exe" "-u -z"
	${EndIf}
FunctionEnd

Function DownloadKB835732
	Call NeedsKB835732
	Pop $0
	${If} $0 == 1
		Call GetUpdateLanguage
		Pop $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini KB835732 $0
		!insertmacro DownloadAndInstall "Windows 2000 KB835732 Update" "$0" "kb835732.exe" "/passive /norestart"
	${EndIf}
FunctionEnd

Function DownloadIE6
	Call NeedsIE6
	Pop $0
	${If} $0 == 1
		!insertmacro DownloadAndInstall "Internet Explorer 6 SP1" "${W2K_IE6}" "ie6setup.exe" '/q /c:"ie6setup.exe /q"'
	${EndIf}
FunctionEnd

Function DownloadXPSP2
	Call NeedsXPSP2
	Pop $0
	${If} $0 == 1
		Call GetUpdateLanguage
		Pop $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini XPSP2 $0
		!insertmacro DownloadAndInstall "Windows XP Service Pack 2" "$0" "xpsp2.exe" "/passive /norestart"
	${EndIf}
FunctionEnd

Function DownloadXPSP3
	Call NeedsXPSP3
	Pop $0
	${If} $0 == 1
		Call GetUpdateLanguage
		Pop $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini XPSP3 $0
		!insertmacro DownloadAndInstall "Windows XP Service Pack 3" "$0" "xpsp3.exe" "/passive /norestart"
	${EndIf}
FunctionEnd

Function Download2003SP2
	Call Needs2003SP2
	Pop $0
	${If} $0 == 1
		Call GetUpdateLanguage
		Call GetArch
		Pop $1
		Pop $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini 2003SP2 $0-$1
		!insertmacro DownloadAndInstall "Windows XP x64 Edition/Server 2003 Service Pack 2" "$0" "w2k3sp2.exe" "/passive /norestart"
	${EndIf}
FunctionEnd
