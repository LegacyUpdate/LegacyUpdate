!define REGPATH_RUNONCE "Software\Microsoft\Windows\CurrentVersion\RunOnce"

!define EWX_REBOOT      0x02
!define EWX_FORCEIFHUNG 0x10

!define RUNONCE_USERNAME "LegacyUpdateTemp"
!define RUNONCE_PASSWORD "Legacy_Update0"

!define IsRunOnce     `"" HasFlag "/runonce"`
!define IsPostInstall `"" HasFlag "/postinstall"`
!define NoRestart     `"" HasFlag "/norestart"`

!macro -PromptReboot un
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

!macro RegisterRunOnce flags
	WriteRegStr HKLM "${REGPATH_RUNONCE}" "Legacy Update" '"$INSTDIR\LegacyUpdateSetup.exe" ${flags}'
!macroend

!macro -RebootIfRequired un
	${If} ${RebootFlag}
		${IfNot} ${IsRunOnce}
		${AndIfNot} ${NoRestart}
			; Create the admin user
			ExecShellWait "" "net" "user /add ${RUNONCE_USERNAME} ${RUNONCE_PASSWORD}" SW_HIDE
			ExecShellWait "" "net" "localgroup /add Administrators ${RUNONCE_USERNAME}" SW_HIDE

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
			WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultUserName" "${RUNONCE_USERNAME}"
			WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultPassword" "${RUNONCE_PASSWORD}"

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
	; Trick winlogon into thinking the shell has started, so it doesn't appear to be stuck at
	; "Welcome" (XP) or "Preparing your desktop..." (Vista+)
	; https://social.msdn.microsoft.com/Forums/WINDOWS/en-US/ca253e22-1ef8-4582-8710-9cd9c89b15c3
	${If} ${AtLeastWinVista}
		StrCpy $0 "ShellDesktopSwitchEvent"
	${Else}
		StrCpy $0 "msgina: ShellReadyEvent"
	${EndIf}

	System::Call 'kernel32::OpenEventW(i ${EVENT_MODIFY_STATE}, i 0, t .r0) p .r1'
	${If} $1 != 0
		System::Call 'kernel32::SetEvent(p $1)'
		System::Call 'kernel32::CloseHandle(p $1)'
	${EndIf}
FunctionEnd

Function CleanUpRunOnce
	; Restore autologon keys
	ClearErrors
	ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_AutoAdminLogon"
	${If} ${Errors}
		DeleteRegValue HKLM "${REGPATH_WINLOGON}" "AutoAdminLogon"
	${Else}
		WriteRegStr HKLM "${REGPATH_WINLOGON}" "AutoAdminLogon" $0
		DeleteRegValue HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_AutoAdminLogon"
	${EndIf}

	ClearErrors
	ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultDomainName"
	${If} ${Errors}
		DeleteRegValue HKLM "${REGPATH_WINLOGON}" "DefaultDomainName"
	${Else}
		WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultDomainName" $0
		DeleteRegValue HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultDomainName"
	${EndIf}

	ClearErrors
	ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultUserName"
	${If} ${Errors}
		DeleteRegValue HKLM "${REGPATH_WINLOGON}" "DefaultUserName"
	${Else}
		WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultUserName" $0
		DeleteRegValue HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultUserName"
	${EndIf}

	ClearErrors
	ReadRegStr $0 HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultPassword"
	${If} ${Errors}
		DeleteRegValue HKLM "${REGPATH_WINLOGON}" "DefaultPassword"
	${Else}
		WriteRegStr HKLM "${REGPATH_WINLOGON}" "DefaultPassword" $0
		DeleteRegValue HKLM "${REGPATH_WINLOGON}" "LegacyUpdate_DefaultPassword"
	${EndIf}

	; Register postinstall runonce for the next admin user logon, and log out of the temporary user
	${If} ${IsRunOnce}
		${IfNot} ${Abort}
			!insertmacro RegisterRunOnce "/postinstall"
		${EndIf}

		ExecShellWait "" "net" "user /delete ${RUNONCE_USERNAME}" SW_HIDE

		; Be really really sure this is the right user before we nuke their profile
		System::Call 'advapi32::GetUserName(t .r0, *i ${NSIS_MAX_STRLEN} r1) i.r2'
		${If} $0 == "${RUNONCE_USERNAME}"
			RMDir /r /REBOOTOK "$PROFILE"
		${EndIf}

		System::Call "user32::ExitWindowsEx(i ${EWX_FORCEIFHUNG}, i 0) i .r0"
	${EndIf}
FunctionEnd
