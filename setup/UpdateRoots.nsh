!macro -SetSecureProtocolsBitmask root path key
	ReadRegDword $0 ${root} "${path}" "${key}"

	; If the value isn't yet set, ReadRegDword will return 0. This means TLSv1.1 and v1.2 will be the
	; only enabled protocols. This is intentional behavior, because SSLv2 and SSLv3 are not secure,
	; and TLSv1.0 is deprecated. The user can manually enable them in Internet Settings if needed.
	; On XP, we'll also enable TLSv1.0, given TLSv1.1 and v1.2 are only offered through an optional
	; POSReady 2009 update.
	${If} $0 == 0
	${AndIf} ${AtMostWinXP2003}
		IntOp $0 $0 | ${WINHTTP_FLAG_SECURE_PROTOCOL_TLS1}
	${EndIf}
	IntOp $0 $0 | ${WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1}
	IntOp $0 $0 | ${WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2}

	WriteRegDword ${root} "${path}" "${key}" $0
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
	!insertmacro -SetSecureProtocolsBitmask HKLM "${REGPATH_INETSETTINGS}" "SecureProtocols"
	!insertmacro -SetSecureProtocolsBitmask HKCU "${REGPATH_INETSETTINGS}" "SecureProtocols"

	; Enable WinHTTP TLSv1.1 and v1.2
	!insertmacro -SetSecureProtocolsBitmask HKLM "${REGPATH_INETSETTINGS}\WinHttp" "DefaultSecureProtocols"
	!insertmacro -SetSecureProtocolsBitmask HKCU "${REGPATH_INETSETTINGS}\WinHttp" "DefaultSecureProtocols"

	; Enable .NET inheriting SChannel protocol config
	; .NET 3 uses the same registry keys as .NET 2.
	WriteRegDword HKLM "${REGPATH_DOTNET_V2}" "SystemDefaultTlsVersions" 1
	WriteRegDword HKLM "${REGPATH_DOTNET_V4}" "SystemDefaultTlsVersions" 1
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

!macro _DownloadSST name
	!insertmacro Download "Certificate Trust List (${name})" "${TRUSTEDR}/${name}.sst" "${name}.sst" 0
!macroend

Function DownloadRoots
	!insertmacro DetailPrint "$(Downloading)Certificate Trust List..."
	!insertmacro _DownloadSST "authroots"
	!insertmacro _DownloadSST "delroots"
	!insertmacro _DownloadSST "roots"
	!insertmacro _DownloadSST "updroots"
	!insertmacro _DownloadSST "disallowedcert"
FunctionEnd

Function UpdateRoots
	File "updroots.exe"
	!insertmacro DetailPrint "$(Installing)Certificate Trust List..."
	!insertmacro ExecWithErrorHandling "Certificate Trust List" '"$OUTDIR\updroots.exe" authroots.sst'
	!insertmacro ExecWithErrorHandling "Certificate Trust List" '"$OUTDIR\updroots.exe" updroots.sst'
	!insertmacro ExecWithErrorHandling "Certificate Trust List" '"$OUTDIR\updroots.exe" -l roots.sst'
	!insertmacro ExecWithErrorHandling "Certificate Trust List" '"$OUTDIR\updroots.exe" -d delroots.sst'
	!insertmacro ExecWithErrorHandling "Certificate Trust List" '"$OUTDIR\updroots.exe" -l -u disallowedcert.sst'
FunctionEnd
