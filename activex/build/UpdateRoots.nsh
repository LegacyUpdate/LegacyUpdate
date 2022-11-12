!define TRUSTEDR "http://download.windowsupdate.com/msdownload/update/v3/static/trustedr/en"

Function UpdateRoots
	SetOutPath $PLUGINSDIR
	File "..\build\updroots.exe"
	!insertmacro DetailPrint "Downloading Certificate Trust List..."
	inetc::get \
		"${TRUSTEDR}/authroots.sst"      "authroots.sst" \
		"${TRUSTEDR}/delroots.sst"       "delroots.sst" \
		"${TRUSTEDR}/roots.sst"          "roots.sst" \
		"${TRUSTEDR}/updroots.sst"       "updroots.sst" \
		"${TRUSTEDR}/disallowedcert.sst" "disallowedcert.sst" \
		/end
	Pop $0
	${If} $0 != "OK"
		${If} $0 != "Cancelled"
			MessageBox MB_OK|MB_USERICON "${name} failed to download.$\r$\n$\r$\n$0" /SD IDOK
		${EndIf}
		SetErrorLevel 1
		Abort
	${EndIf}
	!insertmacro DetailPrint "Installing Certificate Trust List..."
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe authroots.sst"
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe updroots.sst"
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe -l roots.sst"
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe -d delroots.sst"
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe -l -u disallowedcert.sst"
FunctionEnd
