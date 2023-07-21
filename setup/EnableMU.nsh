Function EnableMicrosoftUpdate
	DetailPrint "Enabling Microsoft Update..."
	System::Call 'ole32::CoCreateInstance(g "${CLSID_UpdateServiceManager}", p 0, i ${CLSCTX_INPROC_SERVER}, g "${IID_IUpdateServiceManager2}", *p .r0) i.r1'
	${If} $0 != 0
	${OrIf} $1 == 0
		MessageBox MB_USERICON "Failed to enable Microsoft Update.$\r$\n$\r$\nError code: $0" /SD IDOK
	${EndIf}
	System::Call '$0->${METHOD_AddService2}(t "${WU_MU_SERVICE_ID}", i 7, t"")'
	System::Call 'ole32::CoTaskMemFree(p $0)'
FunctionEnd
