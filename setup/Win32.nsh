; advapi32
!define SECURITY_BUILTIN_DOMAIN_RID 0x00000020
!define DOMAIN_ALIAS_RID_ADMINS     0x00000220

!define AllocateAndInitializeSid 'advapi32::AllocateAndInitializeSid(i, i, i, i, i, i, i, i, i, i, *i) i'
!define LookupAccountSid         'advapi32::LookupAccountSid(i, i, t, *i, t, *i, *i) i'
!define FreeSid                  'advapi32::FreeSid(i)'
!define GetUserName              'advapi32::GetUserName(t, *i) i'

; kernel32
!define PF_XMMI64_INSTRUCTIONS_AVAILABLE    10

!define ES_CONTINUOUS      0x80000000
!define ES_SYSTEM_REQUIRED 0x00000001

!define GetVersionEx             'kernel32::GetVersionEx(pr) i'
!define GetLogicalProcessorInformationEx 'kernel32::GetLogicalProcessorInformationEx(i, *i, *i) i'
!define SetThreadExecutionState  'kernel32::SetThreadExecutionState(i) i'
!define OpenEvent                'kernel32::OpenEvent(i, i, t) i'
!define SetEvent                 'kernel32::SetEvent(i) i'
!define CloseHandle              'kernel32::CloseHandle(i) i'
!define DeleteFile               'kernel32::DeleteFile(t) i'

; netapi32
!define NetApiBufferFree         'netapi32::NetApiBufferFree(i) i'
!define NetUserGetInfo           'netapi32::NetUserGetInfo(t, t, i, *i) i'
!define NetUserAdd               'netapi32::NetUserAdd(t, i, t, i, *i) i'
!define NetLocalGroupAddMembers  'netapi32::NetLocalGroupAddMembers(t, t, i, *i, i) i'

; ole32
!define CoCreateInstance         'ole32::CoCreateInstance(g, p, i, g, *p) i'
!define CoTaskMemFree            'ole32::CoTaskMemFree(p)'

; shell32
!define RestartDialog            'shell32::RestartDialog(p, t, i) i'

; user32
!define EWX_REBOOT       0x02
!define EWX_FORCE        0x04

!define ExitWindowsEx            'user32::ExitWindowsEx(i, i) i'
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

; wuapi
!define CLSID_UpdateServiceManager "{F8D253D9-89A4-4DAA-87B6-1168369F0B21}"
!define IID_IUpdateServiceManager2 "{B596CC9F-56E5-419E-A622-E01BB457431E}"
!define METHOD_AddService2         0x60030003

!define WU_S_ALREADY_INSTALLED 2359302     ; 0x00240006
!define WU_E_NOT_APPLICABLE    -2145124329 ; 0x80240017

!define WU_MU_SERVICE_ID       "7971f918-a847-4430-9279-4a52d1efe18d"

!define IUpdateServiceManager2_AddService2 '${METHOD_AddService2}(t, i, t)'
