Function EnableMicrosoftUpdate
	!insertmacro DetailPrint "Enabling Microsoft Update..."
	System::Call '${CoCreateInstance}("${CLSID_UpdateServiceManager}", 0, ${CLSCTX_INPROC_SERVER}, "${IID_IUpdateServiceManager2}", .r0) .r1'
	${If} $0 != 0
	${OrIf} $1 == 0
		MessageBox MB_USERICON "Failed to enable Microsoft Update.$\r$\n$\r$\nError code: $0" /SD IDOK
	${EndIf}
	System::Call '$0->${IUpdateServiceManager2_AddService2}("${WU_MU_SERVICE_ID}", 7, "")'
	System::Call '${CoTaskMemFree}($0)'
FunctionEnd
