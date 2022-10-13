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
		DetailPrint "Downloading Windows 2000 SP4..."
		Call GetUpdateLanguage
		ReadINIStr $1 $PLUGINSDIR\Patches.ini W2KSP4 $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini W2KSP4 Prefix
		NSISdl::download "$0$1" "w2ksp4.exe"
		DetailPrint "Installing Windows 2000 SP4..."
		ExecWait "w2ksp4.exe -u -z" $0
		${If} $0 == ${ERROR_SUCCESS_REBOOT_REQUIRED}
			SetRebootFlag true
		${ElseIf} $0 != 0
			MessageBox MB_OK|MB_USERICON "Windows 2000 SP4 failed to install. Error code: $0"
			Abort
		${EndIf}
		Delete "w2ksp4.exe"
	${EndIf}
FunctionEnd

Function DownloadKB835732
	${GetFileVersion} "$SYSDIR\kernel32.dll" $0
	${VersionCompare} $0 "5.00.2195.7000" $1
	${If} $1 == 2
		DetailPrint "Downloading Windows 2000 KB835732 Update..."
		Call GetUpdateLanguage
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB835732 $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini KB835732 Prefix
		NSISdl::download "$0$1" "kb835732.exe"
		DetailPrint "Installing Windows 2000 KB835732 Update..."
		ExecWait "kb835732.exe /passive /norestart" $0
		${If} $0 == ${ERROR_SUCCESS_REBOOT_REQUIRED}
			SetRebootFlag true
		${ElseIf} $0 != 0
			MessageBox MB_OK|MB_USERICON "Windows 2000 KB835732 Update failed to install. Error code: $0"
			Abort
		${EndIf}
		Delete "kb835732.exe"
	${EndIf}
FunctionEnd

Function DownloadIE6
	${GetFileVersion} "$SYSDIR\mshtml.dll" $0
	${VersionCompare} $0 "6.0.2600.0" $1
	${If} $1 == 2
		DetailPrint "Downloading Internet Explorer 6 SP1..."
		NSISdl::download "${W2K_IE6}" "ie6setup.exe"
		DetailPrint "Installing Internet Explorer 6 SP1..."
		ExecWait 'ie6setup.exe /q /c:"ie6setup.exe /q"' $0
		${If} $0 == ${ERROR_SUCCESS_REBOOT_REQUIRED}
			SetRebootFlag true
		${ElseIf} $0 != 0
			MessageBox MB_OK|MB_USERICON "Internet Explorer 6 SP1 failed to install. Error code: $0"
			Abort
		${EndIf}
		Delete "ie6setup.exe"
	${EndIf}
FunctionEnd
