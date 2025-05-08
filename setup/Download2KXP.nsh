!insertmacro NeedsSPHandler "W2KSP4"  "Win2000"   2
!insertmacro NeedsSPHandler "XPSP1a"  "WinXP2002" 0
!insertmacro NeedsSPHandler "XPSP3"   "WinXP2002" 2
!insertmacro NeedsSPHandler "XPESP3"  "WinXP2002" 2
!insertmacro NeedsSPHandler "2003SP2" "WinXP2003" 1

!insertmacro NeedsFileVersionHandler "W2KUR1" "kernel32.dll" "5.00.2195.7006"

!insertmacro PatchHandler "W2KSP4"  "Windows 2000 $(SP) 4"      ${PATCH_FLAGS_SHORT} ""
!insertmacro PatchHandler "W2KUR1"  "$(SectionW2KUR1)"          ${PATCH_FLAGS_LONG}  ""
!insertmacro PatchHandler "XPSP1a"  "Windows XP $(SP) 1a"       ${PATCH_FLAGS_SHORT} ""
!insertmacro PatchHandler "XPSP3"   "Windows XP $(SP) 3"        ${PATCH_FLAGS_LONG}  ""
!insertmacro PatchHandler "2003SP2" "Windows XP $(P64)/$(SRV) 2003 $(SP) 2" ${PATCH_FLAGS_LONG} ""
!insertmacro PatchHandler "XPESP3"  "Windows XP $(EMB) $(SP) 3" ${PATCH_FLAGS_LONG}  ""

Function FixW2KUR1
	; Fix idling on multi-CPU systems when Update Rollup 1 is installed
	WriteRegDWORD HKLM "SYSTEM\CurrentControlSet\Control\HAL" "14140000FFFFFFFF" 0x10
FunctionEnd
