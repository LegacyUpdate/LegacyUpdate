Function InstallVCRedist
	IfFileExists "$SYSDIR\mfc90u.dll" 0 done
	IfFileExists "$SYSDIR\msvcr90.dll" 0 done
		SetOutPath "$TEMP"
		DetailPrint "Installing Visual C++ 2008 Redistributable..."
		File "C:\Program Files (x86)\Microsoft SDKs\Windows\v6.0A\Bootstrapper\Packages\vcredist_x86\vcredist_x86.exe"
		ExecWait "vcredist_x86.exe /q"
		Delete "vcredist_x86.exe"
	done:
FunctionEnd
