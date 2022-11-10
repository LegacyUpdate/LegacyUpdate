!define TRUSTEDR "http://download.windowsupdate.com/msdownload/update/v3/static/trustedr/en"

Function UpdateRoots
	SetOutPath $PLUGINSDIR
	File "..\build\updroots.exe"
	!insertmacro DetailPrint "Downloading Certificate Trust List..."
	!insertmacro Download "Certificate Trust List" "${TRUSTEDR}/authroots.sst"      "authroots.sst"
	!insertmacro Download "Certificate Trust List" "${TRUSTEDR}/delroots.sst"       "delroots.sst"
	!insertmacro Download "Certificate Trust List" "${TRUSTEDR}/roots.sst"          "roots.sst"
	!insertmacro Download "Certificate Trust List" "${TRUSTEDR}/updroots.sst"       "updroots.sst"
	!insertmacro Download "Certificate Trust List" "${TRUSTEDR}/disallowedcert.sst" "disallowedcert.sst"
	!insertmacro DetailPrint "Installing Certificate Trust List..."
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe authroots.sst"
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe updroots.sst"
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe -l roots.sst"
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe -d delroots.sst"
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe -l -u disallowedcert.sst"
	Delete "updroots.exe"
	Delete "authroots.sst"
	Delete "updroots.sst"
	Delete "roots.sst"
	Delete "delroots.sst"
	Delete "disallowedcert.sst"
FunctionEnd
