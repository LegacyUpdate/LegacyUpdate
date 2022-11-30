!macro IsRunOnce
	!insertmacro HasFlag /runonce
!macroend

!macro IsPostInstall
	!insertmacro HasFlag /postinstall
!macroend

!macro CanRestart
	!insertmacro HasFlag /norestart
	Pop $0
	${If} $0 == 0
		Push 1
	${Else}
		Push 0
	${EndIf}
!macroend

!macro -PromptReboot un
	SetErrorLevel ${ERROR_SUCCESS_REBOOT_REQUIRED}

	!insertmacro CanRestart
	Pop $0
	${If} $0 == 1
		; Reboot immediately
		Reboot
	${Else}
		; Prompt for reboot
		${IfNot} ${Silent}
			System::Call 'shell32::RestartDialog(p 0, \
				t "Windows will be restarted to complete installation of prerequisite components. Setup will resume after the restart.", \
				i ${EWX_REBOOT})'
		${EndIf}
	${EndIf}
!macroend

!macro RegisterRunOnce flags
	WriteRegStr HKLM "${REGPATH_RUNONCE}" "Legacy Update" '"$INSTDIR\LegacyUpdateSetup.exe" ${flags}'
!macroend

!macro -RebootIfRequired un
	${If} ${RebootFlag}
		!insertmacro IsRunOnce
		Pop $1
		!insertmacro CanRestart
		Pop $2
		${If} $1 == 0
		${AndIf} $2 == 1
			; Create the admin user
			ExecShellWait "" "net" "user /add LegacyUpdateAdmin Legacy_Update0" SW_HIDE
			ExecShellWait "" "net" "localgroup /add Administrators LegacyUpdateAdmin" SW_HIDE

			; Backup autologon keys if any
			ClearErrors
			ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "AutoAdminLogon"
			${IfNot} ${Errors}
				WriteRegStr HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_AutoAdminLogon" $0
			${EndIf}

			ClearErrors
			ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "DefaultDomainName"
			${IfNot} ${Errors}
				WriteRegStr HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultDomainName" $0
			${EndIf}

			ClearErrors
			ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "DefaultUserName"
			${IfNot} ${Errors}
				WriteRegStr HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultUserName" $0
			${EndIf}

			ClearErrors
			ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "DefaultPassword"
			${IfNot} ${Errors}
				WriteRegStr HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultPassword" $0
			${EndIf}

			; Set autologon
			WriteRegStr HKLM "${REGPATH_WINLOGON}" "AutoAdminLogon" "1"
			WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultDomainName" "."
			WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultUserName" "LegacyUpdateAdmin"
			WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultPassword" "Legacy_Update0"

			; Copy to a local path, just in case the installer is on a network share
			CreateDirectory "$INSTDIR"
			CopyFiles /SILENT "$EXEPATH" "$INSTDIR\LegacyUpdateSetup.exe"
		${EndIf}

		!insertmacro RegisterRunOnce "/runonce"
		!insertmacro -PromptReboot "${un}"
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
	; Trick Vista+ winlogon into thinking the shell has started, so it doesn't appear to be stuck at
	; a "Preparing your desktop..." screen
	; https://social.msdn.microsoft.com/Forums/WINDOWS/en-US/ca253e22-1ef8-4582-8710-9cd9c89b15c3
	System::Call 'kernel32::OpenEventW(i ${EVENT_MODIFY_STATE}, i 0, t "ShellDesktopSwitchEvent") i .r0'
	${If} $0 != 0
		System::Call 'kernel32::SetEvent(i $0)'
		System::Call 'kernel32::CloseHandle(i $0)'
	${EndIf}
FunctionEnd

Function CleanUpRunOnce
	; Restore autologon keys
	ClearErrors
	ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_AutoAdminLogon"
	${IfNot} ${Errors}
		WriteRegStr HKLM "${REGPATH_WINLOGON}" "AutoAdminLogon" $0
	${EndIf}

	ClearErrors
	ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultDomainName"
	${IfNot} ${Errors}
		WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultDomainName" $0
	${EndIf}

	ClearErrors
	ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultUserName"
	${IfNot} ${Errors}
		WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultUserName" $0
	${EndIf}

	ClearErrors
	ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultPassword"
	${IfNot} ${Errors}
		WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultPassword" $0
	${EndIf}

	; Register postinstall runonce for the next admin user logon, and log out of the temporary user
	!insertmacro IsRunOnce
	Pop $0
	${If} $0 == 1
		!insertmacro RegisterRunOnce "/postinstall"
		ExecShellWait "" "net" "user /delete LegacyUpdateAdmin" SW_HIDE

		; Be really really sure this is the right user before we nuke their profile
		System::Call 'advapi32::GetUserName(t .r0, *i ${NSIS_MAX_STRLEN} r1) i.r2'
		${If} $0 == "LegacyUpdateAdmin"
			RMDir /r /REBOOTOK "$PROFILE"
		${EndIf}

		System::Call "user32::ExitWindowsEx(i ${EWX_FORCEIFHUNG}, i 0) i .r0"
	${EndIf}
FunctionEnd
