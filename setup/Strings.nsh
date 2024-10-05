!insertmacro MUI_LANGUAGE "English"

; Dialog
MiscButtonText "Back" "Next" "Cancel" "Close"

; Log
LangString ^ExecShell  ${LANG_ENGLISH} "Execute: "
LangString ^Completed  ${LANG_ENGLISH} "Done"
LangString Downloading ${LANG_ENGLISH} "Downloading: "
LangString Extracting  ${LANG_ENGLISH} "Extracting: "
LangString Installing  ${LANG_ENGLISH} "Installing: "

LangString AlreadyInstalled ${LANG_ENGLISH} "Installation skipped - already installed"
LangString NotApplicable    ${LANG_ENGLISH} "Installation skipped - not applicable"

; Download
LangString DownloadStatusSingle ${LANG_ENGLISH} "{TIMEREMAINING} left - {RECVSIZE} of {FILESIZE} ({SPEED})"
LangString DownloadStatusMulti  ${LANG_ENGLISH} "{TIMEREMAINING} left - {TOTALRECVSIZE} of {TOTALFILESIZE} ({SPEED})"

; Error dialogs
FileErrorText \
	'Unable to write to "$0".$\r$\n\
	$\r$\n\
	If Internet Explorer is open, close it and click Retry.' \
	'Unable to write to "$0".$\r$\n\
	$\r$\n\
	If Internet Explorer is open, close it and click Retry.'

LangString MsgBoxUsage ${LANG_ENGLISH} \
	"Usage: setup.exe [/S] [/norestart]$\r$\n\
	$\r$\n\
	/S$\tExecutes Legacy Update setup silently.$\r$\n\
	/norestart$\tDisables automatic restart during installation.$\r$\n\
	$\r$\n\
	If no flags are passed, Legacy Update will launch its full user interface.$\r$\n\
	For more information on these flags, visit legacyupdate.net."

LangString MsgBoxElevationRequired ${LANG_ENGLISH} \
	"Log on as an administrator to install Legacy Update."

LangString MsgBoxOldWinVersion ${LANG_ENGLISH} \
	"Legacy Update requires Windows 2000 or later.$\r$\n\
	$\r$\n\
	You might be interested in Windows Update Restored instead.$\r$\n\
	Would you like to go to http://windowsupdaterestored.com/ now?"

LangString MsgBoxBetaOS ${LANG_ENGLISH} \
	"The current version of Windows is a beta build. Legacy Update may not work correctly on this version of Windows.$\r$\n\
	$\r$\n\
	Continue anyway?"

LangString MsgBoxDownloadAbort ${LANG_ENGLISH} \
	"Cancelling will terminate Legacy Update setup."

LangString MsgBoxWES09NotSSE2 ${LANG_ENGLISH} \
	"Your processor does not support the Streaming SIMD Extensions 2 (SSE2) instruction set, which is required to install Windows Embedded 2009 updates released after May 2018. Processors that initially implemented SSE2 instructions include the Intel Pentium 4, Pentium M, and AMD Athlon 64.$\r$\n\
	$\r$\n\
	To protect your Windows installation from becoming corrupted by incompatible updates, this option will be disabled."

LangString MsgBoxActivateXP2002NotSP3 ${LANG_ENGLISH} \
	"Windows XP must be updated to Service Pack 3 to activate over the internet. The Service Pack 3 update action will be enabled."

LangString MsgBoxActivateXP2003NotSP2 ${LANG_ENGLISH} \
	"Windows XP Professional x64 Edition or Windows Server 2003 must be updated to Service Pack 2 to activate over the internet. The Service Pack 2 update action will be enabled."

LangString MsgBoxMUFailed ${LANG_ENGLISH} \
	"Failed to enable Microsoft Update.$\r$\n$\r$\n$0"

; Statuses
LangString StatusRestartingWUAU ${LANG_ENGLISH} "Restarting Windows Update service..."
LangString StatusCheckingSSL    ${LANG_ENGLISH} "Checking SSL connectivity..."

; Sections
LangString IE       ${LANG_ENGLISH} "Internet Explorer"
LangString PRO      ${LANG_ENGLISH} "Professional"
LangString P64      ${LANG_ENGLISH} "Professional x64 Edition"
LangString EMB      ${LANG_ENGLISH} "Embedded"
LangString SRV      ${LANG_ENGLISH} "Server"
LangString SP       ${LANG_ENGLISH} "Service Pack"
LangString SSU      ${LANG_ENGLISH} "Servicing Stack Update"
LangString WUA      ${LANG_ENGLISH} "Windows Update Agent"
LangString SecUpd   ${LANG_ENGLISH} "Security Update"
LangString Update   ${LANG_ENGLISH} "Update"
LangString PrepTool ${LANG_ENGLISH} "Preparation Tool"
LangString PUS      ${LANG_ENGLISH} "Platform Update Supplement"
LangString SHA2     ${LANG_ENGLISH} "SHA-2 Code Signing Support Update"

LangString SectionWES09     ${LANG_ENGLISH} "Enable Windows Embedded 2009 updates"
LangString SectionWHS2011U4 ${LANG_ENGLISH} "Windows Home Server 2011 Update Rollup 4"
LangString SectionSSU       ${LANG_ENGLISH} "Windows Servicing Stack update"
LangString SectionWUA       ${LANG_ENGLISH} "Windows Update Agent update"
LangString SectionROOTCERTS ${LANG_ENGLISH} "Root certificates store update"
LangString SectionENABLEMU  ${LANG_ENGLISH} "Enable Microsoft Update"
LangString SectionACTIVATE  ${LANG_ENGLISH} "Activate Windows"

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
LangString SectionVISTASP2Desc  ${LANG_ENGLISH} \
	"Updates Windows Vista or Windows Server 2008 to Service Pack 2, as required to install the Windows Update Agent. $(SectionReboots) $(SectionMSLT)"
LangString SectionVISTASSUDesc  ${LANG_ENGLISH} \
	"Updates Windows Vista or Windows Server 2008 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n$(SectionReboots)"
LangString SectionVISTAIE9Desc  ${LANG_ENGLISH} \
	"Updates Internet Explorer to 9.0.$\r$\n$(SectionReboots) $(SectionMSLT)"
LangString SectionWIN7SP1Desc   ${LANG_ENGLISH} \
	"Updates Windows 7 or Windows Server 2008 R2 to Service Pack 1, as required to install the Windows Update Agent. $(SectionReboots) $(SectionMSLT)"
LangString SectionWIN7SSUDesc   ${LANG_ENGLISH} \
	"Updates Windows 7 or Windows Server 2008 R2 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n$(SectionReboots)"
LangString SectionWIN8SSUDesc   ${LANG_ENGLISH} \
	"Updates Windows 8 or Windows Server 2012 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n$(SectionReboots)"
LangString SectionWIN81U1Desc ${LANG_ENGLISH} \
	"Updates Windows 8.1 to Update 1, as required to resolve issues with the Windows Update Agent. Also required to upgrade to Windows 10.$\r$\n$(SectionReboots)"
LangString SectionWIN81SSUDesc  ${LANG_ENGLISH} \
	"Updates Windows 8.1 or Windows Server 2012 R2 with additional updates required to resolve issues with the Windows Update Agent.$\r$\n$(SectionReboots)"
LangString SectionWHS2011U4Desc ${LANG_ENGLISH} \
	"Updates Windows Home Server 2011 to Update Rollup 4 to resolve issues with the Windows Update Agent. Also fixes data corruption problems.$\r$\n$(SectionReboots)"
LangString SectionWUADesc       ${LANG_ENGLISH} \
	"Updates the Windows Update Agent to the latest version, as required for Legacy Update."
LangString SectionROOTCERTSDesc ${LANG_ENGLISH} \
	"Updates the root certificate store to the latest from Microsoft, and enables additional modern security features. Root certificates are used to verify the security of encrypted (https) connections. This fixes connection issues with some websites."
LangString SectionWIN7MUDesc    ${LANG_ENGLISH} \
	"Configures Windows to install updates for Microsoft Office and other Microsoft software."
LangString SectionACTIVATEDesc  ${LANG_ENGLISH} \
	"Your copy of Windows is not activated. If you update the root certificates store, Windows Product Activation can be completed over the internet. Legacy Update can start the activation wizard after installation so you can activate your copy of Windows."
LangString SectionACTIVEX2KXPDesc ${LANG_ENGLISH} \
	"Installs the Legacy Update ActiveX control, enabling access to the full Windows Update interface via the legacyupdate.net website."
LangString SectionACTIVEXVista78Desc ${LANG_ENGLISH} \
	"Installs the Legacy Update ActiveX control, enabling access to the classic Windows Update interface via the legacyupdate.net website. Not required if you want to use the built-in Windows Update Control Panel."
LangString SectionACTIVEXWin10Desc ${LANG_ENGLISH} \
	"Installs the Legacy Update ActiveX control, enabling access to the classic Windows Update interface via the legacyupdate.net website."

; LangString SectionLEGACYUPDATEDesc ${LANG_ENGLISH} \
; 	"Installs Legacy Update, enabling access to Windows Update."
; LangString SectionWUSERVERDesc     ${LANG_ENGLISH} \
; 	"Configures Windows Update to use the Legacy Update proxy server, resolving connection issues to the official Microsoft Windows Update service."
