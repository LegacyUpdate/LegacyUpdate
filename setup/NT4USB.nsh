; TODO: Check whether machine will support the USB stack, via LegacyUpdateNSIS::IsMultiCPU

Function InstallNT4USB
	Call InstallNT4USBStack
	Call InstallNT4USBHID
	Call InstallNT4USBEdgeport
	Call InstallNT4USBMassStorage
	Call InstallNT4USBImaging

	SetOutPath "${RUNONCEDIR}"
FunctionEnd

Function InstallNT4USBStack
	${DetailPrint} "$(Installing)Inside Out Networks USB Stack..."

	; Files
	SetOutPath "$PROGRAMFILES\Inside Out Networks\Inside Out Networks' USB Peripheral Drivers"
	File "patches\nt4\usb\ReadMe.txt"

	SetOutPath "$WINDIR"
	File "patches\nt4\usb\ionlicense.txt"

	SetOutPath "$WINDIR\system32"
	; File "patches\nt4\usb\awusbcfg.exe"
	File "patches\nt4\usb\UsbTray.exe"
	File "patches\nt4\usb\ionusb.exe"
	File "patches\nt4\usb\UsbShare.dll"
	SetOutPath "$WINDIR\system32\drivers"
	File "patches\nt4\usb\usbd.sys"
	File "patches\nt4\usb\usbhub.sys"
	File "patches\nt4\usb\vusbd.sys"

	; Startup shortcut
	CreateShortcut "$SMSTARTUP\USB Status Utility.lnk" "$WINDIR\system32\UsbTray.exe"

	; Register services
	SetOutPath "$PLUGINSDIR"
	File "patches\nt4\usb\ionsvc.dll"
	${DetailPrint} "$(Installing)USBD service..."
	System::Call 'ionsvc::CreateUSBDSrvc(i 0, i 0, s "$WINDIR\system32\usbd.sys") i .r0'
	${If} $0 != 0
		SetErrorLevel 1
		Abort
	${EndIf}

	${DetailPrint} "$(Installing)VUSBD service..."
	System::Call 'ionsvc::CreateVUSBDSrvc(i 0, i 0, s "$WINDIR\system32\usbd.sys") i .r0'
	${If} $0 != 0
		SetErrorLevel 1
		Abort
	${EndIf}

	${DetailPrint} "$(Installing)USBHUB service..."
	System::Call 'ionsvc::CreateUSBHUBSrvc(i 0, i 0, s "$WINDIR\system32\usbd.sys") i .r0'
	${If} $0 != 0
		SetErrorLevel 1
		Abort
	${EndIf}

	${DetailPrint} "$(Installing)IONUSB service..."
	System::Call 'ionsvc::CreateIONUSBSrvc(i 0, i 0, s "$WINDIR\system32\ionusb.exe") i .r0'
	${If} $0 != 0
		SetErrorLevel 1
		Abort
	${EndIf}
FunctionEnd

; TODO: The rest of the files

; File "patches\nt4\usb\edgeport.exe"
; File "patches\nt4\usb\edgeser.sys"
; File "patches\nt4\usb\ionflash.exe"
; File "patches\nt4\usb\usbhid.sys"
; File "patches\nt4\usb\usbms.sys"
; File "patches\nt4\usb\UsbMsDll.dll"
; File "patches\nt4\usb\usbprint.sys"
; File "patches\nt4\usb\usbrm.sys"
; File "patches\nt4\usb\vcusbnt4.sys"
; File "patches\nt4\usb\vicam_32.dll"

Function InstallNT4USBHID
	${DetailPrint} "$(Installing)Inside Out Networks USB HID Driver..."
FunctionEnd

Function InstallNT4USBMassStorage
	${DetailPrint} "$(Installing)Inside Out Networks USB Mass Storage Driver..."
FunctionEnd

Function InstallNT4USBImaging
	${DetailPrint} "$(Installing)Inside Out Networks USB Imaging Driver..."
FunctionEnd

Function InstallNT4USBEdgeport
	${DetailPrint} "$(Installing)Inside Out Networks USB to Serial (Edgeport) Drivers..."
	SetOutPath "$WINDIR\inf"
	File "patches\nt4\usb\rp_mdm.inf"
FunctionEnd
