!define WUA_7_2_6001_7888_x86 "http://download.windowsupdate.com/windowsupdate/redist/standalone/7.2.6001.788/windowsupdateagent30-x86.exe"
!define WUA_7_2_6001_7888_x64 "http://download.windowsupdate.com/windowsupdate/redist/standalone/7.2.6001.788/windowsupdateagent30-x64.exe"
!define WUA_7_6_7600_320_x86  "http://download.windowsupdate.com/windowsupdate/redist/standalone/7.6.7600.320/WindowsUpdateAgent-7.6-x86.exe"
!define WUA_7_6_7600_320_x64  "http://download.windowsupdate.com/windowsupdate/redist/standalone/7.6.7600.320/WindowsUpdateAgent-7.6-x64.exe"

Function DetermineWUAVersion
	${WinVerGetServicePackLevel} $R3

	${If} ${IsWin2000}
		${If} ${IsNativeIA32}
			StrCpy $0 "${WUA_7_2_6001_7888_x86}"
			Return
		${EndIf}
	${ElseIf} ${IsWinXP}
		${If} $R3 >= 3
			${If} ${IsNativeIA32}
				StrCpy $0 "${WUA_7_6_7600_320_x86}"
				Return
			${ElseIf} ${IsNativeAMD64}
				StrCpy $0 "${WUA_7_6_7600_320_x64}"
				Return
			${EndIf}
		${Else}
			${If} ${IsNativeIA32}
				StrCpy $0 "${WUA_7_2_6001_7888_x86}"
				Return
			${ElseIf} ${IsNativeAMD64}
				StrCpy $0 "${WUA_7_2_6001_7888_x64}"
				Return
			${EndIf}
		${EndIf}
	${ElseIf} ${IsWin2003}
		${If} $R3 >= 2
			${If} ${IsNativeIA32}
				StrCpy $0 "${WUA_7_6_7600_320_x86}"
				Return
			${ElseIf} ${IsNativeAMD64}
				StrCpy $0 "${WUA_7_6_7600_320_x64}"
				Return
			${EndIf}
		${Else}
			${If} ${IsNativeIA32}
				StrCpy $0 "${WUA_7_2_6001_7888_x86}"
				Return
			${ElseIf} ${IsNativeAMD64}
				StrCpy $0 "${WUA_7_2_6001_7888_x64}"
				Return
			${EndIf}
		${EndIf}
	${EndIf}
	StrCpy $0 ""
FunctionEnd

Function DownloadWUA
	Call DetermineWUAVersion
	${If} $0 != ""
		SetOutPath "$TEMP"
		DetailPrint "Downloading Windows Update Agent..."
		NSISdl::download "$0" "WindowsUpdateAgent.exe"
		DetailPrint "Installing Windows Update Agent..."
		ExecWait "WindowsUpdateAgent.exe /quiet /norestart"
		Delete "WindowsUpdateAgent.exe"
	${EndIf}
FunctionEnd
