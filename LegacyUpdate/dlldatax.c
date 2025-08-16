// wrapper for dlldata.c

#include <rpc.h>
#include <rpcproxy.h>

#define REGISTER_PROXY_DLL	// DllRegisterServer, etc.
#define USE_STUBLESS_PROXY	// defined only with MIDL switch /Oicf

#undef ENTRY_PREFIX
#define ENTRY_PREFIX Prx

#include "LegacyUpdate_i.h"
#include "LegacyUpdate_dlldata.c"
#include "LegacyUpdate_p.c"
