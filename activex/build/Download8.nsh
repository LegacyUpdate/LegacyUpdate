Function NeedsKB2937636
	${If} ${IsWin8}
	${OrIf} ${IsWin2012}
		!insertmacro NeedsServicingPackage "Package_for_KB2937636_RTM" "6.2.1.4"
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsKB3021910
	${If} ${IsWin8.1}
	${OrIf} ${IsWin2012R2}
		!insertmacro NeedsServicingPackage "Package_for_KB3021910" "6.3.1.2"
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsKB2919355
	${If} ${IsWin8.1}
	${OrIf} ${IsWin2012R2}
		!insertmacro NeedsServicingPackage "Package_for_KB2919355" "6.3.1.14"
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsKB2932046
	${If} ${IsWin8.1}
	${OrIf} ${IsWin2012R2}
		!insertmacro NeedsServicingPackage "Package_for_KB2932046" "6.3.1.5"
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsKB2959977
	${If} ${IsWin8.1}
	${OrIf} ${IsWin2012R2}
		!insertmacro NeedsServicingPackage "Package_for_KB2959977" "6.3.1.1"
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsKB2937592
	${If} ${IsWin8.1}
	${OrIf} ${IsWin2012R2}
		!insertmacro NeedsServicingPackage "Package_for_KB2937592" "6.3.1.0"
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsKB2934018
	${If} ${IsWin8.1}
	${OrIf} ${IsWin2012R2}
		!insertmacro NeedsServicingPackage "Package_for_KB2934018" "6.3.1.5"
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsWin81Update1
	Call NeedsKB2919355
	Call NeedsKB2932046
	Call NeedsKB2959977
	Call NeedsKB2937592
	Call NeedsKB2934018
	Pop $0
	Pop $1
	Pop $2
	Pop $3
	Pop $4

	${If} $0 == 1
	${OrIf} $1 == 1
	${OrIf} $2 == 1
	${OrIf} $3 == 1
	${OrIf} $4 == 1
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function DownloadKB2937636
	Call NeedsKB2937636
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB2937636 $0
		!insertmacro DownloadAndInstallMSU "KB2937636" "Servicing Stack Update for Windows 8" "$1"
	${EndIf}
FunctionEnd

Function DownloadKB3021910
	Call NeedsKB3021910
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB3021910 $0
		!insertmacro DownloadAndInstallMSU "KB3021910" "Servicing Stack Update for Windows 8.1" "$1"
	${EndIf}
FunctionEnd

Function DownloadClearCompressionFlag
	Call GetArch
	Pop $0
	ReadINIStr $0 $PLUGINSDIR\Patches.ini ClearCompressionFlag $0
	!insertmacro DownloadAndInstall "Windows 8.1 Update 1 Preparation Tool" "$0" "ClearCompressionFlag.exe" ""
FunctionEnd

Function DownloadKB2919355
	Call NeedsKB2919355
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB2919355 $0
		!insertmacro DownloadAndInstallMSU "KB2919355" "Windows 8.1 Update 1" "$1"
	${EndIf}
FunctionEnd

Function DownloadKB2932046
	Call NeedsKB2932046
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB2932046 $0
		!insertmacro DownloadAndInstallMSU "KB2932046" "Windows 8.1 Update 1" "$1"
	${EndIf}
FunctionEnd

Function DownloadKB2959977
	Call NeedsKB2959977
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB2959977 $0
		!insertmacro DownloadAndInstallMSU "KB2959977" "Windows 8.1 Update 1" "$1"
	${EndIf}
FunctionEnd

Function DownloadKB2937592
	Call NeedsKB2937592
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB2937592 $0
		!insertmacro DownloadAndInstallMSU "KB2937592" "Windows 8.1 Update 1" "$1"
	${EndIf}
FunctionEnd

Function DownloadKB2934018
	Call NeedsKB2934018
	Pop $0
	${If} $0 == 1
		Call GetArch
		Pop $0
		ReadINIStr $1 $PLUGINSDIR\Patches.ini KB2934018 $0
		!insertmacro DownloadAndInstallMSU "KB2934018" "Windows 8.1 Update 1" "$1"
	${EndIf}
FunctionEnd
