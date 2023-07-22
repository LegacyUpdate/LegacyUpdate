!define IsRunOnce     `"" HasFlag "/runonce"`
!define IsPostInstall `"" HasFlag "/postinstall"`
!define NoRestart     `"" HasFlag "/norestart"`

!macro -PromptReboot
	SetErrorLevel ${ERROR_SUCCESS_REBOOT_REQUIRED}

	${If} ${NoRestart}
		; Prompt for reboot
		${IfNot} ${Silent}
			System::Call 'shell32::RestartDialog(p 0, \
				t "Windows will be restarted to complete installation of prerequisite components. Setup will resume after the restart.", \
				i ${EWX_REBOOT})'
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
			System::Call 'advapi32::AllocateAndInitializeSid(i r0, i 2, i 32, i 544, i 0, i 0, i 0, i 0, i 0, i 0, *i .r1)'
			System::Free $0
			System::Call 'advapi32::LookupAccountSid(i 0, i r1, t .r0, *i ${NSIS_MAX_STRLEN}, t .r2, *i ${NSIS_MAX_STRLEN}, *i 0)'
			System::Call 'advapi32::FreeSid(i r1)'

			; Create the admin user
			ExecShellWait "" "$WINDIR\system32\net.exe" "user /add ${RUNONCE_USERNAME} ${RUNONCE_PASSWORD}" SW_HIDE
			ExecShellWait "" "$WINDIR\system32\net.exe" 'localgroup /add "$0" ${RUNONCE_USERNAME}' SW_HIDE

			!insertmacro -WriteRegStrWithBackup HKLM "${REGPATH_WINLOGON}" "AutoAdminLogon" "1"
			!insertmacro -WriteRegStrWithBackup HKLM "${REGPATH_WINLOGON}" "DefaultDomainName" "."
			!insertmacro -WriteRegStrWithBackup HKLM "${REGPATH_WINLOGON}" "DefaultUserName" "${RUNONCE_USERNAME}"
			!insertmacro -WriteRegStrWithBackup HKLM "${REGPATH_WINLOGON}" "DefaultPassword" "${RUNONCE_PASSWORD}"

			; Copy to a local path, just in case the installer is on a network share
			CreateDirectory "$RunOnceDir"
			CopyFiles /SILENT "$EXEPATH" "$RunOnceDir\LegacyUpdateSetup.exe"
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

	System::Call 'kernel32::OpenEvent(i ${EVENT_MODIFY_STATE}, i 0, t "$0") i .r0'
	${If} $0 != 0
		System::Call 'kernel32::SetEvent(i r0)'
		System::Call 'kernel32::CloseHandle(i r0)'
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
		System::Call 'advapi32::GetUserName(t .r0, *i ${NSIS_MAX_STRLEN}) i .r1'
		${If} $0 == "${RUNONCE_USERNAME}"
			; Register postinstall runonce for the next admin user logon, and log out of the temporary user
			${IfNot} ${Abort}
				Call RegisterRunOncePostInstall
			${EndIf}

			RMDir /r /REBOOTOK "$PROFILE"
			System::Call "user32::ExitWindowsEx(i ${EWX_FORCE} , i 0) i .r0"
		${EndIf}
	${EndIf}
FunctionEnd
