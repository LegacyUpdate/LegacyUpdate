!macro -SetSecureProtocolsBitmask root path key
	ReadRegDword $0 ${root} "${path}" "${key}"
	${VerbosePrint} "${root}\${path}"
	${VerbosePrint} "Before: $0"

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

	${VerbosePrint} "After: $0"
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
	!insertmacro -SetSecureProtocolsBitmask HKLM "${REGPATH_INETSETTINGS_WINHTTP}" "DefaultSecureProtocols"
	!insertmacro -SetSecureProtocolsBitmask HKCU "${REGPATH_INETSETTINGS_WINHTTP}" "DefaultSecureProtocols"

	; Enable .NET inheriting SChannel protocol config
	; .NET 3 uses the same registry keys as .NET 2.
	WriteRegDword HKLM "${REGPATH_DOTNET_V2}" "SystemDefaultTlsVersions" 1
	WriteRegDword HKLM "${REGPATH_DOTNET_V4}" "SystemDefaultTlsVersions" 1
FunctionEnd

Function ConfigureCrypto
	${VerbosePrint} "Configuring crypto (native)"
	Call _ConfigureCrypto

	; Repeat in the WOW64 registry if needed
	${If} ${RunningX64}
		${VerbosePrint} "Configuring crypto (WOW64)"
		SetRegView 32
		Call _ConfigureCrypto
		SetRegView 64
	${EndIf}
FunctionEnd

!macro _DownloadSST name
	!insertmacro Download "$(CTL) (${name})" "${TRUSTEDR}/${name}.sst" "${name}.sst" 0
!macroend

Function DownloadRoots
	${DetailPrint} "$(Downloading)$(CTL)..."
	!insertmacro _DownloadSST authroots
	!insertmacro _DownloadSST delroots
	!insertmacro _DownloadSST roots
	!insertmacro _DownloadSST updroots
	!insertmacro _DownloadSST disallowedcert
FunctionEnd

!macro _InstallRoots state store file
	LegacyUpdateNSIS::UpdateRoots ${state} ${store} "${file}"
	Pop $0
	${If} $0 != 0
		LegacyUpdateNSIS::MessageForHresult $0
		Pop $1
		StrCpy $2 "$(CTL) (${file})"
		MessageBox MB_USERICON "$(MsgBoxInstallFailed)" /SD IDOK
		SetErrorLevel $0
		Abort
	${EndIf}
!macroend

Function UpdateRoots
	${DetailPrint} "$(Installing)$(CTL)..."
	!insertmacro _InstallRoots /update AuthRoot authroots.sst
	!insertmacro _InstallRoots /update AuthRoot updroots.sst
	!insertmacro _InstallRoots /update Root roots.sst
	!insertmacro _InstallRoots /delete AuthRoot delroots.sst
	!insertmacro _InstallRoots /update Disallowed disallowedcert.sst

	WriteRegStr   HKLM "${REGPATH_COMPONENTS}\${ROOTSUPDATE_GUID}" ""            "RootsUpdate"
	WriteRegDword HKLM "${REGPATH_COMPONENTS}\${ROOTSUPDATE_GUID}" "IsInstalled" 1
	WriteRegStr   HKLM "${REGPATH_COMPONENTS}\${ROOTSUPDATE_GUID}" "Version"     "1337,0,2195,0"
	WriteRegStr   HKLM "${REGPATH_COMPONENTS}\${ROOTSUPDATE_GUID}" "Locale"      "*"
	WriteRegStr   HKLM "${REGPATH_COMPONENTS}\${ROOTSUPDATE_GUID}" "ComponentID" "Windows Roots Update"
FunctionEnd
