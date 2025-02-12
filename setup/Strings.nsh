!insertmacro MUI_LANGUAGE "English"

; Dialog
MiscButtonText "Back" "Next" "Cancel" "Close"

; Log
LangString ^ExecShell  ${LANG_ENGLISH} "Execute: "
LangString ^Completed  ${LANG_ENGLISH} "Done"
LangString Downloading ${LANG_ENGLISH} "Downloading "
LangString Extracting  ${LANG_ENGLISH} "Extracting "
LangString Installing  ${LANG_ENGLISH} "Installing "
LangString ExitCode    ${LANG_ENGLISH} "Exit code: "

LangString RestartRequired  ${LANG_ENGLISH} "Success - restart required"
LangString AlreadyInstalled ${LANG_ENGLISH} "Installation skipped - already installed"
LangString NotApplicable    ${LANG_ENGLISH} "Installation skipped - not applicable"

; Download
LangString DownloadStatusSingle ${LANG_ENGLISH} "{TIMEREMAINING} left - {RECVSIZE} of {FILESIZE} ({SPEED})"
LangString DownloadStatusMulti  ${LANG_ENGLISH} "{TIMEREMAINING} left - {TOTALRECVSIZE} of {TOTALFILESIZE} ({SPEED})"

; Startup errors
LangString MsgBoxUsage ${LANG_ENGLISH} \
	"Usage: setup.exe [/S] [/passive] [/norestart]$\r$\n\
	$\r$\n\
	/S$\tExecute Legacy Update setup silently.$\r$\n\
	/v$\tDisplay verbose details during installation.$\r$\n\
	/passive$\tInstall without user interaction.$\r$\n\
	/norestart$\tDisable automatic restart during installation.$\r$\n\
	$\r$\n\
	If no flags are passed, Legacy Update will launch its full user interface.$\r$\n\
	For more information on these flags, visit legacyupdate.net."

LangString MsgBoxElevationRequired ${LANG_ENGLISH} \
	"Log on as an administrator to install Legacy Update."

LangString MsgBoxOldWinVersion ${LANG_ENGLISH} \
	"Legacy Update requires Windows 2000 or later.$\r$\n\
	$\r$\n\
	You might be interested in Windows Update Restored instead.$\r$\n\
	Would you like to go to ${WUR_WEBSITE} now?"

LangString MsgBoxNeedsNT4 ${LANG_ENGLISH} \
	"Legacy Update NT is intended only for use on Windows NT 4.0.$\r$\n\
	$\r$\n\
	For other versions of Windows, use the regular version of Legacy Update.$\r$\n\
	Would you like to go to ${WEBSITE} now?"

LangString MsgBoxBetaOS ${LANG_ENGLISH} \
	"The current version of Windows is a beta build. Legacy Update may not work correctly on this version of Windows.$\r$\n\
	$\r$\n\
	Continue anyway?"

LangString MsgBoxCompatMode ${LANG_ENGLISH} \
	"A compatibility mode has been set on this program. Legacy Update will not work correctly in compatibility mode.$\r$\n\
	$\r$\n\
	Disable it in the Properties dialog of this file and try again."

; Install errors
FileErrorText \
	'Unable to write to "$0".$\r$\n\
	$\r$\n\
	If Internet Explorer is open, close it and click Retry.' \
	'Unable to write to "$0".$\r$\n\
	$\r$\n\
	If Internet Explorer is open, close it and click Retry.'

LangString MsgBoxDownloadAbort ${LANG_ENGLISH} \
	"Cancelling will terminate Legacy Update setup."

LangString MsgBoxDownloadFailed ${LANG_ENGLISH} \
	"$2 failed to download.$\r$\n\
	$\r$\n\
	$0 ($1)"

LangString MsgBoxInstallFailed ${LANG_ENGLISH} \
	"$2 failed to install.$\r$\n\
	$\r$\n\
	$1 ($0)"

LangString MsgBoxPatchNotFound ${LANG_ENGLISH} \
	"$0 failed to install.$\r$\n\
	$\r$\n\
	The installed Windows language and/or architecture is not supported."

LangString MsgBoxMUFailed ${LANG_ENGLISH} \
	"Failed to enable Microsoft Update.$\r$\n\
	$\r$\n\
	$1 ($0)"

; Warnings
LangString MsgBoxWES09NotSSE2 ${LANG_ENGLISH} \
	"Your processor does not support the Streaming SIMD Extensions 2 (SSE2) instruction set, which is required to install Windows Embedded 2009 updates released after May 2018.$\r$\n\
	Processors that initially implemented SSE2 instructions include the Intel Pentium 4, Pentium M, and AMD Athlon 64.$\r$\n\
	$\r$\n\
	To protect your Windows installation from becoming corrupted by incompatible updates, this option will be disabled."

LangString MsgBoxActivateXP2002NotSP3 ${LANG_ENGLISH} \
	"Windows XP must be updated to Service Pack 3 to activate over the internet. The Service Pack 3 update action will be enabled."

LangString MsgBoxActivateXP2003NotSP2 ${LANG_ENGLISH} \
	"Windows XP Professional x64 Edition or Windows Server 2003 must be updated to Service Pack 2 to activate over the internet. The Service Pack 2 update action will be enabled."

LangString MsgBoxVistaSPInstall ${LANG_ENGLISH} \
	"Your computer will restart several times to install Windows Vista Service Pack 2. Your screen may appear blank for an extended period of time. Do not turn off your computer during this process."

LangString MsgBoxWUA2000Datacenter ${LANG_ENGLISH} \
	"Windows Update Agent is not supported on Windows 2000 Datacenter Server. The Legacy Update action will be disabled."

; Statuses
LangString StatusRestarting     ${LANG_ENGLISH} "Restarting..."
LangString StatusRestartingWUAU ${LANG_ENGLISH} "Restarting Windows Update service..."
LangString StatusCheckingSSL    ${LANG_ENGLISH} "Checking SSL connectivity..."
LangString StatusCbsInstalling  ${LANG_ENGLISH} "Waiting for updates to install..."

; Products
LangString DX       ${LANG_ENGLISH} "DirectX"
LangString IE       ${LANG_ENGLISH} "Internet Explorer"
LangString MSI      ${LANG_ENGLISH} "Windows Installer"
LangString VB       ${LANG_ENGLISH} "Visual Basic"
LangString VC       ${LANG_ENGLISH} "Visual C/C++"
LangString WMP      ${LANG_ENGLISH} "Windows Media Player"
LangString Runtime  ${LANG_ENGLISH} "Runtime"
LangString Runtimes ${LANG_ENGLISH} "Runtimes"

; Editions
LangString PRO      ${LANG_ENGLISH} "Professional"
LangString P64      ${LANG_ENGLISH} "Professional x64 Edition"
LangString EMB      ${LANG_ENGLISH} "Embedded"
LangString SRV      ${LANG_ENGLISH} "Server"
LangString WTS      ${LANG_ENGLISH} "Terminal Server"

; Update types
LangString Setup    ${LANG_ENGLISH} "Setup"
LangString Update   ${LANG_ENGLISH} "Update"
LangString Updates  ${LANG_ENGLISH} "Updates"
LangString SP       ${LANG_ENGLISH} "Service Pack"
LangString PostSP   ${LANG_ENGLISH} "Post-Service Pack"
LangString SecUpd   ${LANG_ENGLISH} "Security Update"
LangString SSU      ${LANG_ENGLISH} "Servicing Stack Update"
LangString WUA      ${LANG_ENGLISH} "Windows Update Agent"
LangString PrepTool ${LANG_ENGLISH} "Preparation Tool"
LangString PUS      ${LANG_ENGLISH} "Platform Update Supplement"
LangString SRP      ${LANG_ENGLISH} "Security Rollup Package"
LangString SHA2     ${LANG_ENGLISH} "SHA-2 Code Signing Support Update"
LangString CTL      ${LANG_ENGLISH} "Certificate Trust List"
LangString Unofficial ${LANG_ENGLISH} "(Unofficial)"

LangString SectionWES09     ${LANG_ENGLISH} "Enable Windows Embedded 2009 updates"
LangString SectionWHS2011U4 ${LANG_ENGLISH} "Windows Home Server 2011 Update Rollup 4"
LangString SectionSSU       ${LANG_ENGLISH} "Windows Servicing Stack update"
LangString SectionWUA       ${LANG_ENGLISH} "Windows Update Agent update"
LangString SectionRootCerts ${LANG_ENGLISH} "Root certificates store update"
LangString SectionEnableMU  ${LANG_ENGLISH} "Enable Microsoft Update"
LangString SectionActivate  ${LANG_ENGLISH} "Activate Windows"

LangString SectionReboots   ${LANG_ENGLISH} \
	"Your computer will restart automatically to complete installation."
LangString SectionSupEULA   ${LANG_ENGLISH} \
	"By installing, you are agreeing to the Supplemental End User License Agreement for this update."
LangString SectionMSLT      ${LANG_ENGLISH} \
	"By installing, you are agreeing to the Microsoft Software License Terms for this update."

LangString SectionW2KSP4Desc    ${LANG_ENGLISH} \
	"Updates Windows 2000 to Service Pack 4, as required to install the Windows Update Agent.$\r$\n$(SectionReboots) $(SectionSupEULA)"
LangString SectionIE6SP1Desc    ${LANG_ENGLISH} \
	"Updates Internet Explorer to 6.0 SP1, as required for Legacy Update.$\r$\n$(SectionReboots) $(SectionSupEULA)"
LangString SectionXPSP3Desc     ${LANG_ENGLISH} \
	"Updates Windows XP to Service Pack 3. Required if you would like to activate Windows online. $(SectionReboots) $(SectionSupEULA)"
LangString SectionXPESP3Desc    ${LANG_ENGLISH} \
	"Updates Windows XP Embedded to Service Pack 3. Required if you would like to activate Windows online. $(SectionReboots) $(SectionSupEULA)"
LangString SectionWES09Desc     ${LANG_ENGLISH} \
	"Configures Windows to appear as Windows Embedded POSReady 2009 to Windows Update, enabling access to Windows XP security updates released between 2014 and 2019. Please note that Microsoft officially advises against doing this."
LangString Section2003SP2Desc   ${LANG_ENGLISH} \
	"Updates Windows XP Professional x64 Edition or Windows Server 2003 to Service Pack 2. Required if you would like to activate Windows online. $(SectionReboots) $(SectionSupEULA)"
LangString SectionVistaSP2Desc  ${LANG_ENGLISH} \
	"Updates Windows Vista or Windows Server 2008 to Service Pack 2, as required to install the Windows Update Agent. $(SectionReboots) $(SectionMSLT)"
LangString SectionVistaSSUDesc  ${LANG_ENGLISH} \
	"Updates Windows Vista or Windows Server 2008 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n$(SectionReboots)"
LangString SectionVistaIE9Desc  ${LANG_ENGLISH} \
	"Updates Internet Explorer to 9.0.$\r$\n$(SectionReboots) $(SectionMSLT)"
LangString SectionWin7SP1Desc   ${LANG_ENGLISH} \
	"Updates Windows 7 or Windows Server 2008 R2 to Service Pack 1, as required to install the Windows Update Agent. $(SectionReboots) $(SectionMSLT)"
LangString SectionWin7SSUDesc   ${LANG_ENGLISH} \
	"Updates Windows 7 or Windows Server 2008 R2 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n$(SectionReboots)"
LangString SectionWin8SSUDesc   ${LANG_ENGLISH} \
	"Updates Windows 8 or Windows Server 2012 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n$(SectionReboots)"
LangString SectionWin81U1Desc   ${LANG_ENGLISH} \
	"Updates Windows 8.1 to Update 1, as required to resolve issues with the Windows Update Agent. Also required to upgrade to Windows 10.$\r$\n$(SectionReboots)"
LangString SectionWin81SSUDesc  ${LANG_ENGLISH} \
	"Updates Windows 8.1 or Windows Server 2012 R2 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n$(SectionReboots)"
LangString SectionWHS2011U4Desc ${LANG_ENGLISH} \
	"Updates Windows Home Server 2011 to Update Rollup 4 to resolve issues with the Windows Update Agent. Also fixes data corruption problems.$\r$\n$(SectionReboots)"
LangString SectionWUADesc       ${LANG_ENGLISH} \
	"Updates the Windows Update Agent to the latest version, as required for Legacy Update."
LangString SectionRootCertsDesc ${LANG_ENGLISH} \
	"Updates the root certificate store to the latest from Microsoft, and enables additional modern security features. Root certificates are used to verify the security of encrypted (https) connections. This fixes connection issues with some websites."
LangString SectionWin7MUDesc    ${LANG_ENGLISH} \
	"Configures Windows to install updates for Microsoft Office and other Microsoft software."
LangString SectionActivateDesc  ${LANG_ENGLISH} \
	"Your copy of Windows is not activated. If you update the root certificates store, Windows Product Activation can be completed over the internet. Legacy Update can start the activation wizard after installation so you can activate your copy of Windows."
LangString SectionActiveX2KXPDesc ${LANG_ENGLISH} \
	"Installs the Legacy Update ActiveX control, enabling access to the full Windows Update interface via the legacyupdate.net website."
LangString SectionActiveXVista78Desc ${LANG_ENGLISH} \
	"Installs the Legacy Update ActiveX control, enabling access to the classic Windows Update interface via the legacyupdate.net website. Not required if you want to use the built-in Windows Update Control Panel."
LangString SectionActiveXWin10Desc ${LANG_ENGLISH} \
	"Installs the Legacy Update ActiveX control, enabling access to the classic Windows Update interface via the legacyupdate.net website."

; LangString SectionLegacyUpdateDesc ${LANG_ENGLISH} \
; 	"Installs Legacy Update, enabling access to Windows Update."
; LangString SectionWUServerDesc     ${LANG_ENGLISH} \
; 	"Configures Windows Update to use the Legacy Update proxy server, resolving connection issues to the official Microsoft Windows Update service."

; NT 4.0
LangString MsgBoxNT4PostInstall ${LANG_ENGLISH} \
	"Legacy Update is complete. Windows is up to date."

LangString SectionWDU           ${LANG_ENGLISH} \
	"Windows Desktop Update"
LangString SectionWDUYes        ${LANG_ENGLISH} \
	"Install Windows 98-style Explorer"
LangString SectionWDUNo         ${LANG_ENGLISH} \
	"Keep Windows 95-style Explorer"

LangString SectionNT4USB        ${LANG_ENGLISH} \
	"Inside Out Networks USB Peripheral Drivers"
LangString SectionSPCleanup     ${LANG_ENGLISH} \
	"Delete update uninstall data"

LangString SectionNT4WDUDesc ${LANG_ENGLISH} \
	"Installs the updated Windows 98-style Explorer experience. This choice can not be changed later. Requires installing Internet Explorer 6."
LangString SectionNT4WDUYesDesc ${LANG_ENGLISH} \
	"$(SectionNT4WDUDesc)"
LangString SectionNT4WDUNoDesc  ${LANG_ENGLISH} \
	"Keeps the Windows 95-style Explorer experience. This choice can not be changed later."

LangString SectionNT4SP6ADesc   ${LANG_ENGLISH} \
	"Updates Windows NT 4.0 to Service Pack 6a with 128-bit encryption support.$\r$\n$(SectionReboots) $(SectionSupEULA)"
LangString SectionNT4RollupDesc ${LANG_ENGLISH} \
	"Installs Windows NT 4.0 Service Pack 6a Security Rollup Package.$\r$\n$(SectionReboots) $(SectionSupEULA)"
LangString SectionNT4PostSPDesc ${LANG_ENGLISH} \
	"Installs Windows NT 4.0 updates released after the Security Rollup Package.$\r$\n$(SectionReboots) $(SectionSupEULA)"

LangString SectionNT4MSIDesc    ${LANG_ENGLISH} \
	"Installs Windows Installer, required to install newer applications.$\r$\n$(SectionSupEULA)"
LangString SectionNT4VCRTDesc   ${LANG_ENGLISH} \
	"Installs updated versions of the Microsoft Visual C, Visual C++, and Visual Basic runtimes, required to run newer applications.$\r$\n$(SectionSupEULA)"
LangString SectionNT4IE6SP1Desc ${LANG_ENGLISH} \
	"Installs Internet Explorer 6. Required for Windows Desktop Update.$\r$\n$(SectionSupEULA)"
LangString SectionNT4WMP64Desc  ${LANG_ENGLISH} \
	"Installs Windows Media Player 6.4.$\r$\n$(SectionSupEULA)"
LangString SectionNT4DX5Desc    ${LANG_ENGLISH} \
	"Installs DirectX 5. This is an unofficial patch. Microsoft officially supports only up to DirectX 3 on Windows NT 4.0.$\r$\n$(SectionSupEULA)"

LangString SectionNT4USBDesc    ${LANG_ENGLISH} \
	"Installs the Inside Out Networks USB Peripheral Drivers, enabling access to USB devices.$\r$\nBy installing, you are agreeing to the Inside Out Networks Edgeport for NT 4.0 License Agreement."
LangString SectionSPCleanupDesc ${LANG_ENGLISH} \
	"Deletes backup files created during installation of updates. This frees up disk space, but you will not be able to uninstall updates."
