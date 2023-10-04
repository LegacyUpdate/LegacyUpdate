Function EnableMicrosoftUpdate
	!insertmacro DetailPrint "Enabling Microsoft Update..."
	System::Call '${CoCreateInstance}("${CLSID_UpdateServiceManager}", 0, ${CLSCTX_INPROC_SERVER}, "${IID_IUpdateServiceManager2}", .rr4) .r1'
	${If} $0 == 0
	${OrIf} $1 != 0
		MessageBox MB_USERICON "Failed to enable Microsoft Update.$\r$\n$\r$\nError code: $1" /SD IDOK
	${Else}
		System::Call '$0->${IUpdateServiceManager2::AddService2}("${WU_MU_SERVICE_ID}", 7, "", r4) .r1'
		${If} $1 != 0
			MessageBox MB_USERICON "Failed to enable Microsoft Update.$\r$\n$\r$\nError code: $1" /SD IDOK
		${EndIf}
		${IUnknown::Release} $0 ''
	${EndIf}
FunctionEnd
