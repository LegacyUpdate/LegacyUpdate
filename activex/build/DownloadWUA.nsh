!define VER_SUITE_PERSONAL 0x00000200

Function IsWinXPHomeSP3
	${If} ${IsWinXP2002}
	${AndIf} ${AtLeastServicePack} 3
		; Call GetVersionEx to get the suite mask
		System::Alloc $0
		System::Call '*(&i ${OSVERSIONINFOEXW_SIZE}) p .r0'
		System::Call '*$0(i ${OSVERSIONINFOEXW_SIZE})'
		System::Call 'kernel32::GetVersionExW(pr0) i .r3'
		System::Call '*$0(i .s, i .s, i .s, i .s, i .s, &t128 .s, &i2 .s, &i2 .s, &i2 .r1, &i1 .s, &i1)'
		System::Free $0

		; Are we on a Personal (Home Edition) suite?
		IntOp $0 $1 & ${VER_SUITE_PERSONAL}
		${If} $0 = ${VER_SUITE_PERSONAL}
			Push 1
		${Else}
			Push 0
		${EndIf}
	${Else}
		Push 0
	${EndIf}
FunctionEnd

Function DetermineWUAVersion
	; Hardcoded special case for XP Home SP3, because the WUA 7.6.7600.256 setup SFX is seriously
	; broken on it, potentially causing an unbootable Windows install due to it entering an infinite
	; loop of creating folders in the root of C:.
	Call IsWinXPHomeSP3
	Pop $0
	${If} $0 == 1
		StrCpy $1 "5.1.3-home"
	${Else}
		GetWinVer $1 Major
		GetWinVer $2 Minor
		GetWinVer $3 ServicePack
		StrCpy $1 "$1.$2.$3"
	${EndIf}

	StrCpy $0 ""

	ClearErrors
	ReadINIStr $2 $PLUGINSDIR\Patches.ini WUA $1
	${If} ${Errors}
		Return
	${EndIf}

	${GetFileVersion} "$SYSDIR\wuapi.dll" $1
	${VersionCompare} $1 $2 $3

	${If} $3 == 2
		Call GetArch
		Pop $0
		ReadINIStr $0 $PLUGINSDIR\Patches.ini WUA $2-$0
	${EndIf}
FunctionEnd

Function DownloadWUA
	Call DetermineWUAVersion
	${If} $0 != ""
		SetOutPath $PLUGINSDIR
		!insertmacro DownloadAndInstall "Windows Update Agent" "$0" "WindowsUpdateAgent.exe" "/quiet /norestart"
	${EndIf}
FunctionEnd
