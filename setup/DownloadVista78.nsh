Function NeedsPackage
	Pop $0
	ClearErrors
	FindFirst $R0 $R1 "$WINDIR\servicing\Packages\$0~31bf3856ad364e35~*"
	FindClose $R0
	${If} ${Errors}
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

!macro SPHandler kbid pkg title
	Function Needs${kbid}
!if "${kbid}" == "VistaSP1"
		; Special case: Server 2008 is already SP1, but doesn't have a VistaSP1 package we can detect.
		${If} ${IsServerOS}
			Push 0
			Return
		${EndIf}
!endif
		Push ${pkg}
		Call NeedsPackage
	FunctionEnd

	Function Download${kbid}
		Call Needs${kbid}
		Pop $0
		${If} $0 == 1
			Call GetArch
			Pop $0
			ReadINIStr $2 $PLUGINSDIR\Patches.ini "${kbid}" "$0-sha256"
			ReadINIStr $0 $PLUGINSDIR\Patches.ini "${kbid}" $0
			ReadINIStr $1 $PLUGINSDIR\Patches.ini "${kbid}" Prefix
			!insertmacro Download "${title}" "$1$0" "${kbid}.exe" "$2" 1
		${EndIf}
	FunctionEnd

	Function Install${kbid}
		${If} ${NeedsPatch} ${kbid}
			Call Download${kbid}
			!insertmacro InstallSP "${title}" "${kbid}.exe"
		${EndIf}
	FunctionEnd
!macroend

!macro MSUHandler kbid title
	Function Needs${kbid}
		Push Package_for_${kbid}
		Call NeedsPackage
	FunctionEnd

	Function Download${kbid}
		${If} ${NeedsPatch} ${kbid}
			Call GetArch
			Pop $0
			ReadINIStr $1 $PLUGINSDIR\Patches.ini "${kbid}" $0
			ReadINIStr $2 $PLUGINSDIR\Patches.ini "${kbid}" Prefix
			ReadINIStr $3 $PLUGINSDIR\Patches.ini "${kbid}" "$0-sha256"
			!insertmacro DownloadMSU "${kbid}" "${title}" "$2$1" "$3"
		${EndIf}
	FunctionEnd

	Function Install${kbid}
		${If} ${NeedsPatch} ${kbid}
			Call Download${kbid}
			!insertmacro InstallMSU "${kbid}" "${title}"
		${EndIf}
	FunctionEnd
!macroend

; Service Packs
!insertmacro SPHandler  "VistaSP1"  "VistaSP1-KB936330"    "Windows Vista $(SP) 1"
!insertmacro SPHandler  "VistaSP2"  "VistaSP2-KB948465"    "Windows Vista $(SP) 2"
!insertmacro SPHandler  "Win7SP1"   "Windows7SP1-KB976933" "Windows 7 $(SP) 1"

; Windows Vista post-SP2 update combination that fixes WU indefinitely checking for updates
!insertmacro MSUHandler "KB3205638" "$(SecUpd) for Windows Vista"
!insertmacro MSUHandler "KB4012583" "$(SecUpd) for Windows Vista"
!insertmacro MSUHandler "KB4015195" "$(SecUpd) for Windows Vista"
!insertmacro MSUHandler "KB4015380" "$(SecUpd) for Windows Vista"

; Internet Explorer 9 for Windows Vista
!insertmacro MSUHandler "KB971512"  "$(Update) for Windows Vista"
!insertmacro MSUHandler "KB2117917" "$(PUS) for Windows Vista"

!insertmacro NeedsFileVersionHandler "IE9" "mshtml.dll" "9.0.8112.16421"
!insertmacro PatchHandler "IE9" "$(IE) 9 for Windows Vista" ${PATCH_FLAGS_OTHER} "/passive /norestart /update-no /closeprograms"

; Windows Vista Servicing Stack Update
!insertmacro MSUHandler "KB4493730" "2019-04 $(SSU) for Windows $(SRV) 2008"

; Windows 7 Servicing Stack Update
!insertmacro MSUHandler "KB3138612" "2016-03 $(SSU) for Windows 7"
!insertmacro MSUHandler "KB4474419" "$(SHA2) for Windows 7"
!insertmacro MSUHandler "KB4490628" "2019-03 $(SSU) for Windows 7"

; Windows Home Server 2011 Update Rollup 4
!insertmacro MSUHandler "KB2757011" "$(SectionWHS2011U4)"

; Windows 8 Servicing Stack
!insertmacro MSUHandler "KB4598297" "2021-01 $(SSU) for Windows $(SRV) 2012"

; Windows 8.1 Servicing Stack
!insertmacro MSUHandler "KB3021910" "2015-04 $(SSU) for Windows 8.1"

; Weird prerequisite to Update 1 that fixes the main KB2919355 update failing to install
Function NeedsClearCompressionFlag
	Call NeedsKB2919355
FunctionEnd

!insertmacro PatchHandler "ClearCompressionFlag" "Windows 8.1 $(Update) 1 $(PrepTool)" ${PATCH_FLAGS_OTHER} ""

; Windows 8.1 Update 1
!insertmacro MSUHandler "KB2919355" "Windows 8.1 $(Update) 1"
!insertmacro MSUHandler "KB2932046" "Windows 8.1 $(Update) 1"
!insertmacro MSUHandler "KB2959977" "Windows 8.1 $(Update) 1"
!insertmacro MSUHandler "KB2937592" "Windows 8.1 $(Update) 1"
!insertmacro MSUHandler "KB2934018" "Windows 8.1 $(Update) 1"

; Windows 8.1 Update 3
; TODO
; !insertmacro MSUHandler "KB2934018" "Windows 8.1 $(Update) 3"

Function NeedsVistaPostSP2
	${If} ${NeedsPatch} KB3205638
	${OrIf} ${NeedsPatch} KB4012583
	${OrIf} ${NeedsPatch} KB4015195
	${OrIf} ${NeedsPatch} KB4015380
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsVistaESU
	Call NeedsKB4493730
FunctionEnd

Function NeedsWin7PostSP1
	${If} ${NeedsPatch} KB3138612
	${OrIf} ${NeedsPatch} KB4474419
	${OrIf} ${NeedsPatch} KB4490628
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsWin81Update1
	${If} ${NeedsPatch} KB2919355
	${OrIf} ${NeedsPatch} KB2932046
	${OrIf} ${NeedsPatch} KB2937592
	${OrIf} ${NeedsPatch} KB2934018
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

; TODO
; Function NeedsWin81Update3
; 	Call GetArch
; 	Pop $0
; 	${If} $0 == "arm"
; 	${AndIf} ${NeedsPatch} KB2934018
; 		Push 1
; 	${Else}
; 		Push 0
; 	${EndIf}
; FunctionEnd
