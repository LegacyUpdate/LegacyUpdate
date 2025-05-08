!insertmacro NeedsFileVersionHandler "IE6" "mshtml.dll" "6.0.2600.0"

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
