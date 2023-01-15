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

!macro PatchHandler kbid title params
	Function Download${kbid}
		Call Needs${kbid}
		Pop $0
		${If} $0 == 1
			Call GetUpdateLanguage
			Call GetArch
			Pop $1
			Pop $0
			ReadINIStr $0 $PLUGINSDIR\Patches.ini "${kbid}" $0-$1
			!insertmacro DownloadAndInstall "${title}" "$0" "${kbid}.exe" "${params}"
		${EndIf}
	FunctionEnd
!macroend

!insertmacro NeedsSPHandler "W2KSP4"  "Win2000"   2
!insertmacro NeedsSPHandler "XPSP2"   "WinXP2002" 0
!insertmacro NeedsSPHandler "XPSP3"   "WinXP2002" 2
!insertmacro NeedsSPHandler "2003SP2" "WinXP2003" 1

!insertmacro NeedsFileVersionHandler "KB835732" "kernel32.dll" "5.00.2195.6897"
!insertmacro NeedsFileVersionHandler "IE6"      "mshtml.dll"   "6.0.2600.0"

!insertmacro PatchHandler "W2KSP4"   "Windows 2000 Service Pack 4"                       "-u -z"
!insertmacro PatchHandler "KB835732" "Windows 2000 KB835732 Update"                      "/passive /norestart"
!insertmacro PatchHandler "XPSP2"    "Windows XP Service Pack 2"                         "/passive /norestart"
!insertmacro PatchHandler "XPSP3"    "Windows XP Service Pack 3"                         "/passive /norestart"
!insertmacro PatchHandler "2003SP2"  "Windows XP x64 Edition/Server 2003 Service Pack 2" "/passive /norestart"

Function DownloadIE6
	Call NeedsIE6
	Pop $0
	${If} $0 == 1
		Call GetUpdateLanguage
		Call GetArch
		Pop $1
		Pop $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini "W2KIE6" $0-$1
		!insertmacro DownloadIfNeeded "Internet Explorer 6 SP1" "$0" "ie6sp1.cab"
		!insertmacro DetailPrint "Extracting Internet Explorer 6 SP1..."
		ExecShellWait "" "expand.exe" '"$0" -F:ie6setup.exe .' SW_HIDE
		ExecShellWait "" "expand.exe" '"$0" -F:iebatch.txt .' SW_HIDE
		!insertmacro DetailPrint "Installing Internet Explorer 6 SP1..."
		!insertmacro ExecWithErrorHandling 'Internet Explorer 6 SP1' 'ie6setup.exe /q' 0
	${EndIf}
FunctionEnd
