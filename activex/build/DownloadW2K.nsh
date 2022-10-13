; TODO: Handle all of the service pack languages
!define W2K_SP4_EN "http://download.windowsupdate.com/msdownload/update/v3-19990518/cabpool/w2ksp4_en_7f12d2da3d7c5b6a62ec4fde9a4b1e6.exe"

!define W2K_KB835732_EN "http://download.windowsupdate.com/msdownload/update/v3-19990518/cabpool/windows2000-kb835732-x86-enu_7b7d1dffd9347af2c166bfcecc4782c.exe"

!define W2K_IE6 "http://legacyupdate.net/patches/ie6sp1_w2k.exe"

Function DownloadW2KSP4
	${If} ${IsWin2000}
		${WinVerGetServicePackLevel} $0
		${If} $0 < 3
			SetOutPath "$TEMP"
			DetailPrint "Downloading Windows 2000 SP4..."
			NSISdl::download "${W2K_SP4_EN}" "w2ksp4.exe"
			DetailPrint "Installing Windows 2000 SP4..."
			ExecWait "w2ksp4.exe -u -z"
			Delete "w2ksp4.exe"
			SetRebootFlag true
		${EndIf}
	${EndIf}
FunctionEnd

Function DownloadKB835732
	${If} ${IsWin2000}
		${GetFileVersion} "$SYSDIR\kernel32.dll" $0
		${VersionCompare} $0 "5.00.2195.7000" $1
		${If} $1 == 2
			SetOutPath "$TEMP"
			DetailPrint "Downloading Windows 2000 KB835732 Update..."
			NSISdl::download "${W2K_KB835732_EN}" "kb835732.exe"
			DetailPrint "Installing Windows 2000 KB835732 Update..."
			ExecWait "kb835732.exe /passive /norestart"
			Delete "kb835732.exe"
			SetRebootFlag true
		${EndIf}
	${EndIf}
FunctionEnd

Function DownloadIE6
	${GetFileVersion} "$SYSDIR\mshtml.dll" $0
	${VersionCompare} $0 "6.0.2600.0" $1
	${If} $1 == 2
		SetOutPath "$TEMP"
		DetailPrint "Downloading Internet Explorer 6..."
		NSISdl::download "${W2K_IE6}" "ie6setup.exe"
		DetailPrint "Installing Internet Explorer 6..."
		ExecWait 'ie6setup.exe /q /c:"ie6setup.exe /q"'
		Delete "ie6setup.exe"
		SetRebootFlag true
	${EndIf}
FunctionEnd
