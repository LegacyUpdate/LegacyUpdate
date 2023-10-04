!define IsRunOnce     `"" HasFlag "/runonce"`
!define IsPostInstall `"" HasFlag "/postinstall"`
!define NoRestart     `"" HasFlag "/norestart"`

!macro -PromptReboot
	SetErrorLevel ${ERROR_SUCCESS_REBOOT_REQUIRED}

	${If} ${NoRestart}
		; Prompt for reboot
		${IfNot} ${Silent}
			System::Call '${RestartDialog}($HWNDPARENT, \
				"Windows will be restarted to complete installation of prerequisite components. Setup will resume after the restart.", \
				${EWX_REBOOT})'
		${EndIf}
	${Else}
		; Reboot immediately
		Reboot
	${EndIf}
!macroend

!macro -RegisterRunOnce flags
	WriteRegStr HKLM "${REGPATH_RUNONCE}" "Legacy Update" '"$RunOnceDir\LegacyUpdateSetup.exe" ${flags}'
!macroend

Function RegisterRunOnce
	!insertmacro -RegisterRunOnce "/runonce"
FunctionEnd

Function un.RegisterRunOnce
	; Unused, just needs to exist to make the compiler happy
FunctionEnd

Function RegisterRunOncePostInstall
	!insertmacro -RegisterRunOnce "/postinstall"
FunctionEnd

!macro -WriteRegStrWithBackup root key name value
	; Backup the key if it exists
	ClearErrors
	ReadRegStr $0 ${root} "${key}" "${name}"
	${IfNot} ${Errors}
		WriteRegStr ${root} "${key}" "LegacyUpdate_${name}" $0
	${EndIf}

	WriteRegStr ${root} "${key}" "${name}" "${value}"
!macroend

!macro -RestoreRegStr root key name
	; Restore the key if it exists
	ClearErrors
	ReadRegStr $0 ${root} "${key}" "LegacyUpdate_${name}"
	${If} ${Errors}
		DeleteRegValue ${root} "${key}" "${name}"
	${Else}
		WriteRegStr ${root} "${key}" "${name}" $0
		DeleteRegValue ${root} "${key}" "LegacyUpdate_${name}"
	${EndIf}
!macroend

!macro -RebootIfRequired un
	${If} ${RebootFlag}
		${IfNot} ${IsRunOnce}
		${AndIfNot} ${NoRestart}
			!insertmacro DetailPrint "Preparing to restart..."

			; Get the localised name of the Administrators group from its SID
			System::Call '*(&i1 0, &i4 0, &i1 5) i .r0'
			; S-1-5-32-544
			System::Call '${AllocateAndInitializeSid}(r0, 2, ${SECURITY_BUILTIN_DOMAIN_RID}, ${DOMAIN_ALIAS_RID_ADMINS}, 0, 0, 0, 0, 0, 0, .r1)'
			System::Free $0
			System::Call '${LookupAccountSid}(0, r1, .r0, ${NSIS_MAX_STRLEN}, .r2, ${NSIS_MAX_STRLEN}, 0)'
			System::Call '${FreeSid}(r1)'

			; Create the admin user
			ExecShellWait "" "$WINDIR\system32\net.exe" "user /add ${RUNONCE_USERNAME} ${RUNONCE_PASSWORD}" SW_HIDE
			ExecShellWait "" "$WINDIR\system32\net.exe" 'localgroup /add "$0" ${RUNONCE_USERNAME}' SW_HIDE

			!insertmacro -WriteRegStrWithBackup HKLM "${REGPATH_WINLOGON}" "AutoAdminLogon" "1"
			!insertmacro -WriteRegStrWithBackup HKLM "${REGPATH_WINLOGON}" "DefaultDomainName" "."
			!insertmacro -WriteRegStrWithBackup HKLM "${REGPATH_WINLOGON}" "DefaultUserName" "${RUNONCE_USERNAME}"
			!insertmacro -WriteRegStrWithBackup HKLM "${REGPATH_WINLOGON}" "DefaultPassword" "${RUNONCE_PASSWORD}"

			; Copy to runonce path to ensure installer is accessible by the temp user
			CreateDirectory "$RunOnceDir"
			CopyFiles /SILENT "$EXEPATH" "$RunOnceDir\LegacyUpdateSetup.exe"

			; Remove mark of the web to prevent "Open File - Security Warning" dialog
			System::Call '${DeleteFile}("$RunOnceDir\LegacyUpdateSetup.exe:Zone.Identifier")'
		${EndIf}

		Call ${un}RegisterRunOnce
		!insertmacro -PromptReboot
		Quit
	${EndIf}
!macroend

Function RebootIfRequired
	${MementoSectionSave}
	!insertmacro -RebootIfRequired ""
FunctionEnd

Function un.RebootIfRequired
	!insertmacro -RebootIfRequired "un."
FunctionEnd

Function OnRunOnceLogon
	; Trick winlogon into thinking the shell has started, so it doesn't appear to be stuck at
	; "Welcome" (XP) or "Preparing your desktop..." (Vista+)
	; https://social.msdn.microsoft.com/Forums/WINDOWS/en-US/ca253e22-1ef8-4582-8710-9cd9c89b15c3
	${If} ${AtLeastWinVista}
		StrCpy $0 "ShellDesktopSwitchEvent"
	${Else}
		StrCpy $0 "msgina: ShellReadyEvent"
	${EndIf}

	System::Call '${OpenEvent}(${EVENT_MODIFY_STATE}, 0, "$0") .r0'
	${If} $0 != 0
		System::Call '${SetEvent}(r0)'
		System::Call '${CloseHandle}(r0)'
	${EndIf}

	; Handle Safe Mode case. RunOnce can still be processed in Safe Mode in some edge cases. If that
	; happens, just silently register runonce again and quit.
	${If} ${IsSafeMode}
		Call RegisterRunOnce
		Quit
	${EndIf}

	; Allow the themes component to be registered if necessary. This sets the theme to Aero Basic
	; rather than Classic in Vista/7.
	ClearErrors
	ReadRegStr $0 HKLM "${REGPATH_COMPONENT_THEMES}" "StubPath"
	${IfNot} ${Errors}
		ExecShellWait "" "$WINDIR\system32\cmd.exe" "/c $0" SW_HIDE
	${EndIf}
FunctionEnd

Function CleanUpRunOnce
	; Restore autologon keys
	!insertmacro -RestoreRegStr HKLM "${REGPATH_WINLOGON}" "AutoAdminLogon"
	!insertmacro -RestoreRegStr HKLM "${REGPATH_WINLOGON}" "DefaultDomainName"
	!insertmacro -RestoreRegStr HKLM "${REGPATH_WINLOGON}" "DefaultUserName"
	!insertmacro -RestoreRegStr HKLM "${REGPATH_WINLOGON}" "DefaultPassword"

	; Delete the temp user
	ExecShellWait "" "$WINDIR\system32\net.exe" "user /delete ${RUNONCE_USERNAME}" SW_HIDE

	${If} ${IsRunOnce}
		; Clean up temporary setup exe if we created it (likely on next reboot)
		${If} ${FileExists} "$RunOnceDir"
			RMDir /r /REBOOTOK "$RunOnceDir"
		${EndIf}

		; Be really really sure this is the right user before we nuke their profile and log out
		System::Call '${GetUserName}(.r0, ${NSIS_MAX_STRLEN}) .r1'
		${If} $0 == "${RUNONCE_USERNAME}"
			; Register postinstall runonce for the next admin user logon, and log out of the temporary user
			${IfNot} ${Abort}
				Call RegisterRunOncePostInstall
			${EndIf}

			RMDir /r /REBOOTOK "$PROFILE"
			System::Call "${ExitWindowsEx}(${EWX_FORCE}, 0) .r0"
		${EndIf}
	${EndIf}
FunctionEnd
