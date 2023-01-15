; NSIS WinVer.nsh rewritten to work more like what I expect.

!include LogicLib.nsh
!include Util.nsh

; Defines

!define OSVERSIONINFOW_SIZE   276
!define OSVERSIONINFOEXW_SIZE 284

!define WINVER_2000   0x0500 ;  5.0.2195
!define WINVER_XP     0x0501 ;  5.1.2600
!define WINVER_XP2002 0x0501 ;  5.1.2600
!define WINVER_XP2003 0x0502 ;  5.2.3790
!define WINVER_VISTA  0x0600 ;  6.0.6000
!define WINVER_7      0x0601 ;  6.1.7600
!define WINVER_8      0x0602 ;  6.2.9200
!define WINVER_8.1    0x0603 ;  6.3.9600
!define WINVER_10     0x0A00 ; 10.0.10240

!define WINVER_SERVER_2000   ${WINVER_2000}
!define WINVER_SERVER_2003   ${WINVER_XP2003}
!define WINVER_SERVER_2003R2 ${WINVER_XP2003}
!define WINVER_SERVER_2008   ${WINVER_VISTA}
!define WINVER_SERVER_2008R2 ${WINVER_7}
!define WINVER_SERVER_2012   ${WINVER_8}
!define WINVER_SERVER_2012R2 ${WINVER_8.1}
!define WINVER_SERVER_2016   ${WINVER_10}

!define /ifndef VER_NT_WORKSTATION 1

!define VER_SUITE_BACKOFFICE     0x00000004 ; Microsoft BackOffice
!define VER_SUITE_BLADE          0x00000400 ; Windows Server 2003, Web Edition
!define VER_SUITE_COMPUTE_SERVER 0x00004000 ; Windows Server 2003, Compute Cluster Edition
!define VER_SUITE_DATACENTER     0x00000080 ; Windows Server Datacenter
!define VER_SUITE_ENTERPRISE     0x00000002 ; Windows Server Enterprise
!define VER_SUITE_EMBEDDEDNT     0x00000040 ; Windows Embedded
!define VER_SUITE_PERSONAL       0x00000200 ; Windows Home Edition
!define VER_SUITE_SINGLEUSERTS   0x00000100 ; Single-user Remote Desktop
!define VER_SUITE_SMALLBUSINESS  0x00000001 ; Small Business Server
!define VER_SUITE_SMALLBUSINESS_RESTRICTED 0x00000020 ; Small Business Server (restrictive client license)
!define VER_SUITE_STORAGE_SERVER 0x00002000 ; Windows Storage Server 2003
!define VER_SUITE_TERMINAL       0x00000010 ; Terminal Services (always true)
!define VER_SUITE_WH_SERVER      0x00008000 ; Windows Home Server
!define VER_SUITE_MULTIUSERTS    0x00020000 ; Multi-user Remote Desktop

; Init

!macro __WinVer_Init
	!ifndef __WINVER_VARS_DECLARED
		!define __WINVER_VARS_DECLARED

		Var /GLOBAL __WINVEROS
		Var /GLOBAL __WINVERSP
		Var /GLOBAL __WINVERPROD
		Var /GLOBAL __WINVERSUITE
	!endif

  StrCmp $__WINVEROS "" _winver_noveryet
		Return

	_winver_noveryet:
		GetWinVer $__WINVEROS    NTDDIMajMin
		GetWinVer $__WINVERSP    ServicePack
		GetWinVer $__WINVERPROD  Product

		Push $0
		Push $1
		System::Alloc $0
		System::Call '*(&i ${OSVERSIONINFOEXW_SIZE}) p .r0'
		System::Call '*$0(i ${OSVERSIONINFOEXW_SIZE})'
		System::Call 'kernel32::GetVersionEx(pr .r0) i'
		System::Call '*$0(i .s, i .s, i .s, i .s, i .s, &t128 .s, &i2 .s, &i2 .s, &i2 .r1, &i1 .s, &i1)'
		System::Free $0
		StrCpy $__WINVERSUITE $1
		Pop $1
		Pop $0
!macroend

; Tests

!macro __WinVer_TestOS op num _t _f
	${CallArtificialFunction} __WinVer_Init
	!insertmacro _${op} $__WINVEROS ${num} `${_t}` `${_f}`
!macroend

!macro __WinVer_TestSP op num _t _f
	${CallArtificialFunction} __WinVer_Init
	!insertmacro _${op} $__WINVERSP ${num} `${_t}` `${_f}`
!macroend

!macro __WinVer_TestProduct op num _t _f
	${CallArtificialFunction} __WinVer_Init
	!insertmacro _${op} $__WINVERPROD ${num} `${_t}` `${_f}`
!macroend

!macro __WinVer_TestSuite _a num _t _f
	!insertmacro _LOGICLIB_TEMP
	${CallArtificialFunction} __WinVer_Init
	IntOp $_LOGICLIB_TEMP $__WINVERSUITE & ${num}
	!insertmacro _= $_LOGICLIB_TEMP ${num} `${_t}` `${_f}`
!macroend

; Defines

!define IsClientOS         `=  _WinVer_TestProduct ${VER_NT_WORKSTATION}`
!define IsServerOS         `!= _WinVer_TestProduct ${VER_NT_WORKSTATION}`

!define IsHomeEdition      `"" _WinVer_TestSuite ${VER_SUITE_PERSONAL}`

!define IsServicePack      `=  _WinVer_TestSP`
!define AtLeastServicePack `>= _WinVer_TestSP`
!define AtMostServicePack  `<= _WinVer_TestSP`

!macro __WinVer_DefineClient os
	!define IsWin${os}       `=  _WinVer_TestOS ${WINVER_${os}}`
	!define AtLeastWin${os}  `>= _WinVer_TestOS ${WINVER_${os}}`
	!define AtMostWin${os}   `<= _WinVer_TestOS ${WINVER_${os}}`
!macroend

!macro __WinVer_DefineServer os
	!define IsWin${os}       `=  _WinVer_TestOS ${WINVER_SERVER_${os}}`
	!define AtLeastWin${os}  `>= _WinVer_TestOS ${WINVER_SERVER_${os}}`
	!define AtMostWin${os}   `<= _WinVer_TestOS ${WINVER_SERVER_${os}}`
!macroend

!insertmacro __WinVer_DefineClient 2000
!insertmacro __WinVer_DefineClient XP2002
!insertmacro __WinVer_DefineClient XP2003
!insertmacro __WinVer_DefineClient Vista
!insertmacro __WinVer_DefineClient 7
!insertmacro __WinVer_DefineClient 8
!insertmacro __WinVer_DefineClient 8.1
!insertmacro __WinVer_DefineClient 10

!insertmacro __WinVer_DefineServer 2003
!insertmacro __WinVer_DefineServer 2003R2
!insertmacro __WinVer_DefineServer 2008
!insertmacro __WinVer_DefineServer 2008R2
!insertmacro __WinVer_DefineServer 2012
!insertmacro __WinVer_DefineServer 2012R2
!insertmacro __WinVer_DefineServer 2016
