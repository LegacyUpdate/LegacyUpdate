!macro CABHandler kbid title packagename
	Function Needs${kbid}
		Call GetComponentArch
		Pop $0
		ClearErrors
		ReadRegStr $1 HKLM "${REGPATH_PACKAGEINDEX}\${packagename}~31bf3856ad364e35~$0~~0.0.0.0" ""
		${If} ${Errors}
			Push 1
		${Else}
			Push 0
		${EndIf}
	FunctionEnd

	Function Download${kbid}
		Call Needs${kbid}
		Pop $0
		${If} $0 == 1
			Call GetArch
			Pop $0
			ReadINIStr $1 $PLUGINSDIR\Patches.ini "${kbid}" $0
			!insertmacro DownloadAndInstallCAB "${kbid}" "${title}" "$1"
		${EndIf}
	FunctionEnd
!macroend

; Windows Home Server 2011 Update Rollup 4
!insertmacro CABHandler "KB2757011" "Windows Home Server 2011 Update Rollup 4" "Package_for_KB2757011"