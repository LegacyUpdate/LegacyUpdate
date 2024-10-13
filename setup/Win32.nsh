; advapi32
!define GetUserName              'advapi32::GetUserName(t, *i) i'

; kernel32
!define PF_XMMI64_INSTRUCTIONS_AVAILABLE 10

!define ES_CONTINUOUS      0x80000000
!define ES_SYSTEM_REQUIRED 0x00000001

!define GetVersionEx             'kernel32::GetVersionEx(pr) i'
!define IsProcessorFeaturePresent 'kernel32::IsProcessorFeaturePresent(i) i'
!define SetThreadExecutionState  'kernel32::SetThreadExecutionState(i) i'
!define OpenEvent                'kernel32::OpenEvent(i, i, t) i'
!define SetEvent                 'kernel32::SetEvent(i) i'
!define CloseHandle              'kernel32::CloseHandle(i) i'
!define DeleteFile               'kernel32::DeleteFile(t) i'

; ntdll
!define RtlGetNtVersionNumbers   'ntdll::RtlGetNtVersionNumbers(*i, *i, *i)'

; shell32
!define RestartDialog            'shell32::RestartDialog(p, t, i) i'

; user32
!define EWX_REBOOT 0x02

!define GetSystemMetrics         'user32::GetSystemMetrics(i) i'

; winhttp
!define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1   0x00000080
!define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1 0x00000200
!define WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 0x00000800

!define ERROR_INTERNET_OPERATION_CANCELLED  12017

; wininet
!define INTERNET_FLAG_RELOAD          0x80000000
!define INTERNET_FLAG_NO_CACHE_WRITE  0x04000000
!define INTERNET_FLAG_KEEP_CONNECTION 0x00400000
!define INTERNET_FLAG_NO_COOKIES      0x00080000
!define INTERNET_FLAG_NO_UI           0x00000200

!define SECURITY_FLAG_STRENGTH_STRONG 0x20000000

; winlogon
!define SETUP_TYPE_NOREBOOT   2
!define SETUP_SHUTDOWN_REBOOT 1

; wuapi
!define WU_S_ALREADY_INSTALLED 2359302     ; 0x00240006
!define WU_E_NOT_APPLICABLE    -2145124329 ; 0x80240017

!define WU_MU_SERVICE_ID       "7971f918-a847-4430-9279-4a52d1efe18d"
