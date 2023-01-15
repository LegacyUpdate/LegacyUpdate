!define TRUSTEDR "http://download.windowsupdate.com/msdownload/update/v3/static/trustedr/en"

!define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1   0x00000080
!define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1 0x00000200
!define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 0x00000800

!macro -SetSecureProtocolsBitmask
	; If the value isn't yet set, ReadRegDword will return 0. This means TLSv1.1 and v1.2 will be the
	; only enabled protocols. This is intentional behavior, because SSLv2 and SSLv3 are not secure,
	; and TLSv1.0 is deprecated. The user can manually enable them in Internet Settings if needed.
	; On XP, we'll also enable TLSv1.0, given TLSv1.1 and v1.2 are only offered through an optional
	; POSReady 2009 update.
	${If} $0 == 0
	${AndIf} ${AtMostWin2003}
		IntOp $0 $0 | ${WINHTTP_FLAG_SECURE_PROTOCOL_TLS1}
	${EndIf}
	IntOp $0 $0 | ${WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1}
	IntOp $0 $0 | ${WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2}
!macroend

Function _ConfigureCrypto
	; Enable SChannel TLSv1.1 and v1.2
	WriteRegDword HKLM "${REGPATH_SCHANNEL_PROTOCOLS}\TLS 1.1\Client" "Enabled" 1
	WriteRegDword HKLM "${REGPATH_SCHANNEL_PROTOCOLS}\TLS 1.1\Client" "DisabledByDefault" 0
	WriteRegDword HKLM "${REGPATH_SCHANNEL_PROTOCOLS}\TLS 1.1\Server" "Enabled" 1
	WriteRegDword HKLM "${REGPATH_SCHANNEL_PROTOCOLS}\TLS 1.1\Server" "DisabledByDefault" 0
	WriteRegDword HKLM "${REGPATH_SCHANNEL_PROTOCOLS}\TLS 1.2\Client" "Enabled" 1
	WriteRegDword HKLM "${REGPATH_SCHANNEL_PROTOCOLS}\TLS 1.2\Client" "DisabledByDefault" 0
	WriteRegDword HKLM "${REGPATH_SCHANNEL_PROTOCOLS}\TLS 1.2\Server" "Enabled" 1
	WriteRegDword HKLM "${REGPATH_SCHANNEL_PROTOCOLS}\TLS 1.2\Server" "DisabledByDefault" 0

	; Enable IE TLSv1.1 and v1.2
	ReadRegDword $0 HKLM "${REGPATH_INETSETTINGS}" "SecureProtocols"
	!insertmacro -SetSecureProtocolsBitmask
	WriteRegDword HKLM "${REGPATH_INETSETTINGS}" "SecureProtocols" $0

	ReadRegDword $0 HKCU "${REGPATH_INETSETTINGS}" "SecureProtocols"
	!insertmacro -SetSecureProtocolsBitmask
	WriteRegDword HKCU "${REGPATH_INETSETTINGS}" "SecureProtocols" $0

	; Enable WinHTTP TLSv1.1 and v1.2
	ReadRegDword $0 HKLM "${REGPATH_INETSETTINGS}\WinHttp" "DefaultSecureProtocols"
	!insertmacro -SetSecureProtocolsBitmask
	WriteRegDword HKLM "${REGPATH_INETSETTINGS}\WinHttp" "DefaultSecureProtocols" $0

	ReadRegDword $0 HKCU "${REGPATH_INETSETTINGS}\WinHttp" "DefaultSecureProtocols"
	!insertmacro -SetSecureProtocolsBitmask
	WriteRegDword HKCU "${REGPATH_INETSETTINGS}\WinHttp" "DefaultSecureProtocols" $0
FunctionEnd

Function ConfigureCrypto
	Call _ConfigureCrypto

	; Repeat in the WOW64 registry if needed
	${If} ${RunningX64}
		SetRegView 32
		Call _ConfigureCrypto
		SetRegView 64
	${EndIf}
FunctionEnd

Function UpdateRoots
	File "..\build\updroots.exe"
	!insertmacro DetailPrint "Downloading Certificate Trust List..."
	inetc::get \
		/bgcolor FFFFFF /textcolor 000000 \
		"${TRUSTEDR}/authroots.sst"      "authroots.sst" \
		"${TRUSTEDR}/delroots.sst"       "delroots.sst" \
		"${TRUSTEDR}/roots.sst"          "roots.sst" \
		"${TRUSTEDR}/updroots.sst"       "updroots.sst" \
		"${TRUSTEDR}/disallowedcert.sst" "disallowedcert.sst" \
		/end
	Pop $0
	${If} $0 != "OK"
		${If} $0 != "Cancelled"
			MessageBox MB_OK|MB_USERICON "Certificate Trust List failed to download.$\r$\n$\r$\n$0" /SD IDOK
		${EndIf}
		SetErrorLevel 1
		Abort
	${EndIf}
	!insertmacro DetailPrint "Installing Certificate Trust List..."
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe authroots.sst" 0
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe updroots.sst" 0
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe -l roots.sst" 0
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe -d delroots.sst" 0
	!insertmacro ExecWithErrorHandling "Certificate Trust List" "updroots.exe -l -u disallowedcert.sst" 0
FunctionEnd
