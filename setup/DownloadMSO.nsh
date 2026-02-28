Var /GLOBAL MSO.PathsLoaded

Var /GLOBAL MSO12.DllPath
Var /GLOBAL MSO12.ExcelPath
Var /GLOBAL MSO12.WordPath
Var /GLOBAL MSO12.ExcelCnvPath
Var /GLOBAL MSO12.WordCnvPath

Var /GLOBAL MSO14.DllPath32
Var /GLOBAL MSO14.ExcelPath32
Var /GLOBAL MSO14.WordPath32
Var /GLOBAL MSO14.DllPath64
Var /GLOBAL MSO14.ExcelPath64
Var /GLOBAL MSO14.WordPath64

!macro -GetMSOPath component key
	ReadRegStr $R0 HKLM "${REGPATH_MSI_COMPONENTS}\${component}" "${key}"

	; Work around quirk with MSI component paths in the registry, where the second or third character may be garbage
	StrCpy $R1 "$R0" 1 0
	StrCpy $R0 "$R0" "" 3
	Push "$R1:\$R0"
!macroend

Function GetMSOPaths
	${If} $MSO.PathsLoaded == 1
		Return
	${EndIf}

	StrCpy $MSO.PathsLoaded 1

	; Office 2007 (x86 only)
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO12}     D94C8360B8BB1DC41B1950E0F8237563
	Pop $MSO12.DllPath
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO12}     D94C8360B8BB1DC41B1950E2F8237563
	Pop $MSO12.ExcelPath
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO12}     D94C8360B8BB1DC41B1950E1F8237563
	Pop $MSO12.WordPath

	; Office Converter Pack (x86 only)
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO12CONV} F17DAFC40CE3BDA4DA5BD0E651973510
	Pop $MSO12.ExcelCnvPath
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO12CONV} 602293A26C2A2CE4BBBA9A4870DE6B25
	Pop $MSO12.WordCnvPath

	; Office 2010 (x86)
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO14_32}  A609E893B628DD84791945C946C9CA5E
	Pop $MSO14.DllPath32
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO14_32}  98C6F8355DA2600418456C7670479E08
	Pop $MSO14.ExcelPath32
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO14_32}  E628C910A54494645A0BB00FF8CCE4EE
	Pop $MSO14.WordPath32

	; Office 2010 (x64)
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO14_64}  A609E893B628DD84791945C946C9CA5E
	Pop $MSO14.DllPath64
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO14_64}  98C6F8355DA2600418456C7670479E08
	Pop $MSO14.ExcelPath64
	!insertmacro -GetMSOPath ${MSI_COMPONENT_MSO14_64}  E628C910A54494645A0BB00FF8CCE4EE
	Pop $MSO14.WordPath64
FunctionEnd

!macro MSOHandler kbid title arch
	Function Download${kbid}
		StrCpy $Patch.Key   "${kbid}"
		StrCpy $Patch.File  "${kbid}.cab"
		StrCpy $Patch.Title "${title}"

		${If} ${NeedsPatch} ${kbid}
			Call -PatchHandler
		${EndIf}
	FunctionEnd

	Function Install${kbid}
		${IfNot} ${NeedsPatch} ${kbid}
			Return
		${EndIf}

		Call Download${kbid}

		StrCpy $Exec.Patch "${kbid}"
		!insertmacro ExtractCab "${title}" "${RUNONCEDIR}\${kbid}.cab" "$PLUGINSDIR\${kbid}"

		${DetailPrint} "$(Installing)${title}..."
		FindFirst $0 $1 "$PLUGINSDIR\${kbid}\*.msp"
		${If} $1 != ""
			FindClose $0
			StrCpy $0 "$PLUGINSDIR\${kbid}\$1"
			!insertmacro InstallMSI "${title}" "$0" 1
		${EndIf}
	FunctionEnd
!macroend

; Office Compatibility Pack (Compatibility Pack for the 2007 Office system)
!insertmacro NeedsFileVersionHandler "MSO12ConvSP3"  "$MSO12.ExcelCnvPath" "12.0.6611.1000"
!insertmacro NeedsFileVersionHandler "KB4018354"     "$MSO12.WordCnvPath"  "12.0.6611.1000"
!insertmacro NeedsFileVersionHandler "KB4461607"     "$MSO12.ExcelCnvPath" "12.0.6807.5000"

!insertmacro MSOHandler "MSO12ConvSP3" "$(MSO) $(CompatPack) $(SP) 3" "x86"
!insertmacro MSOHandler "KB4018354" "2018-04 $(SecUpd) for $(MSO) $(CompatPack) $(SP) 3 (KB4018354)" "x86"
!insertmacro MSOHandler "KB4461607" "2019-02 $(SecUpd) for $(MSO) $(CompatPack) $(SP) 3 (KB4461607)" "x86"

; Office 2007
!insertmacro NeedsFileVersionHandler "MSO12SP3"  "$MSO12.DllPath"              "12.0.6607.1000"
!insertmacro NeedsFileVersionHandler "KB4018353" "$MSO12.ExcelPath\excel.exe"  "12.0.6787.5000"
!insertmacro NeedsFileVersionHandler "KB4018355" "$MSO12.WordPath\winword.exe" "12.0.6787.5000"

!insertmacro MSOHandler "MSO12SP3"  "$(MSO) 2007 $(SP) 3" "x86"
!insertmacro MSOHandler "KB4018353" "2018-04 $(SecUpd) for $(MSO) Excel 2007 $(SP) 3 (KB4018353)" "x86"
!insertmacro MSOHandler "KB4018355" "2018-02 $(SecUpd) for $(MSO) Word 2007 $(SP) 3 (KB4018355)"  "x86"

; Office 2010
!insertmacro NeedsFileVersionHandler "MSO14SP2-x86"  "$MSO14.DllPath32"              "14.0.7015.1000"
!insertmacro NeedsFileVersionHandler "KB3017810-x86" "$MSO14.ExcelPath32\excel.exe"  "14.0.7268.5000"
!insertmacro NeedsFileVersionHandler "KB4493218-x86" "$MSO14.WordPath32\winword.exe" "14.0.7268.5000"

!insertmacro NeedsFileVersionHandler "MSO14SP2-x64"  "$MSO14.DllPath64"              "14.0.7015.1000"
!insertmacro NeedsFileVersionHandler "KB3017810-x64" "$MSO14.ExcelPath64\excel.exe"  "14.0.7268.5000"
!insertmacro NeedsFileVersionHandler "KB4493218-x64" "$MSO14.WordPath64\winword.exe" "14.0.7268.5000"

!insertmacro MSOHandler "MSO14SP2-x86"  "$(MSO) 2010 $(SP) 2" ""
!insertmacro MSOHandler "KB3017810-x86" "2021-04 $(SecUpd) for Microsoft Excel 2010 (KB3017810)" ""
!insertmacro MSOHandler "KB4493218-x86" "2021-04 $(SecUpd) for Microsoft Word 2010 (KB4493218)"  ""

!insertmacro MSOHandler "MSO14SP2-x64"  "$(MSO) 2010 $(SP) 2" ""
!insertmacro MSOHandler "KB3017810-x64" "2021-04 $(SecUpd) for Microsoft Excel 2010 (KB3017810)" ""
!insertmacro MSOHandler "KB4493218-x64" "2021-04 $(SecUpd) for Microsoft Word 2010 (KB4493218)"  ""

Function NeedsMSO14SP2
	${If} ${NeedsPatch} MSO14SP2-x86
	${OrIf} ${NeedsPatch} MSO14SP2-x64
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsKB3017810
	${If} ${NeedsPatch} KB3017810-x86
	${OrIf} ${NeedsPatch} KB3017810-x64
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function NeedsKB4493218
	${If} ${NeedsPatch} KB4493218-x86
	${OrIf} ${NeedsPatch} KB4493218-x64
		Push 1
	${Else}
		Push 0
	${EndIf}
FunctionEnd
