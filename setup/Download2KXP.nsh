!insertmacro NeedsSPHandler "W2KSP4"  "Win2000"   2
!insertmacro NeedsSPHandler "XPSP1a"  "WinXP2002" 0
!insertmacro NeedsSPHandler "XPSP3"   "WinXP2002" 2
!insertmacro NeedsSPHandler "XPESP3"  "WinXP2002" 2
!insertmacro NeedsSPHandler "2003SP2" "WinXP2003" 1

!insertmacro NeedsFileVersionHandler "W2KUR1" "kernel32.dll" "5.00.2195.7006"
!insertmacro NeedsFileVersionHandler "IE6"    "mshtml.dll"   "6.0.2600.0"

!insertmacro PatchHandler "W2KSP4"  "Windows 2000 $(SP) 4"      ${PATCH_FLAGS_SHORT} ""
!insertmacro PatchHandler "W2KUR1"  "$(SectionW2KUR1)"          ${PATCH_FLAGS_LONG}  ""
!insertmacro PatchHandler "XPSP1a"  "Windows XP $(SP) 1a"       ${PATCH_FLAGS_SHORT} ""
!insertmacro PatchHandler "XPSP3"   "Windows XP $(SP) 3"        ${PATCH_FLAGS_LONG}  ""
!insertmacro PatchHandler "2003SP2" "Windows XP $(P64)/$(SRV) 2003 $(SP) 2" ${PATCH_FLAGS_LONG} ""
!insertmacro PatchHandler "XPESP3"  "Windows XP $(EMB) $(SP) 3" ${PATCH_FLAGS_LONG}  ""

!insertmacro NeedsFileVersionHandler "IE6"      "mshtml.dll"   "6.0.2600.0"

!macro DownloadIE ver title
	${If} ${NeedsPatch} IE${ver}
		StrCpy $Patch.Key   "IE${ver}"
		StrCpy $Patch.File  "ie${ver}setup.exe"
		StrCpy $Patch.Title "${title} $(Setup)"
		Call -PatchHandler

		${IfNot} ${FileExists} "$PLUGINSDIR\IE${ver}\ie${ver}setup.exe"
			${DetailPrint} "$(Downloading)${title}..."
			!insertmacro ExecWithErrorHandling '${title}' '"$PLUGINSDIR\IE${ver}\ie${ver}setup.exe" /c:"ie${ver}wzd.exe /q /d /s:""#e"""'
		${EndIf}
	${EndIf}
!macroend

!macro InstallIE ver title
	${If} ${NeedsPatch} IE${ver}
		Call DownloadIE${ver}
		${DetailPrint} "$(Installing)${title}..."
		StrCpy $RunOnce.UseFallback 1
		!insertmacro ExecWithErrorHandling '${title}' '"$PLUGINSDIR\IE${ver}\ie${ver}setup.exe" /c:"ie${ver}wzd.exe /q /r:n /s:""#e"""'
		RMDir /r /REBOOTOK "$WINDIR\Windows Update Setup Files"
	${EndIf}
!macroend

Function DownloadIE6
	!insertmacro DownloadIE 6 "$(IE) 6 $(SP) 1"
FunctionEnd

Function InstallIE6
	!insertmacro InstallIE 6 "$(IE) 6 $(SP) 1"
FunctionEnd

Function FixW2KUR1
	; Fix idling on multi-CPU systems when Update Rollup 1 is installed
	WriteRegDWORD HKLM "SYSTEM\CurrentControlSet\Control\HAL" "14140000FFFFFFFF" 0x10
FunctionEnd
