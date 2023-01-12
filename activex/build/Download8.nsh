; Windows 8 Servicing Stack
!insertmacro MSUHandler "KB2937636" "Servicing Stack Update for Windows 8"   "Package_for_KB2937636_RTM" "6.2.1.4"

; Windows 8.1 Servicing Stack
!insertmacro MSUHandler "KB3021910" "Servicing Stack Update for Windows 8.1" "Package_for_KB3021910"     "6.3.1.2"

; Windows 8.1 Update 1
!insertmacro MSUHandler "KB2919355" "Windows 8.1 Update 1"                   "Package_for_KB2919355"     "6.3.1.14"
!insertmacro MSUHandler "KB2932046" "Windows 8.1 Update 1"                   "Package_for_KB2932046"     "6.3.1.5"
!insertmacro MSUHandler "KB2959977" "Windows 8.1 Update 1"                   "Package_for_KB2959977"     "6.3.1.1"
!insertmacro MSUHandler "KB2937592" "Windows 8.1 Update 1"                   "Package_for_KB2937592"     "6.3.1.0"
!insertmacro MSUHandler "KB2934018" "Windows 8.1 Update 1"                   "Package_for_KB2934018"     "6.3.1.5"

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

; Weird prerequisite to Update 1 that prevents the main KB2919355 update from failing to install
Function DownloadClearCompressionFlag
	Call GetArch
	Pop $0
	ReadINIStr $0 $PLUGINSDIR\Patches.ini ClearCompressionFlag $0
	!insertmacro DownloadAndInstall "Windows 8.1 Update 1 Preparation Tool" "$0" "ClearCompressionFlag.exe" ""
FunctionEnd
