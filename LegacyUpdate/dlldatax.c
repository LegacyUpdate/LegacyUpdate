// wrapper for dlldata.c

#include <windows.h>

#define REGISTER_PROXY_DLL
#define USE_STUBLESS_PROXY
#define PROXY_DELEGATION
#define ENTRY_PREFIX Prx

#include <rpcproxy.h>

#include "LegacyUpdate_i.h"
#include "LegacyUpdate_dlldata.c"
#include "LegacyUpdate_p.c"
