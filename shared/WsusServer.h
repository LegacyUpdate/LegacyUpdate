#pragma once

#include <windows.h>
#include <oleauto.h>

// TODO: This is messy
#if __has_include("LegacyUpdate_i.h")
#include "LegacyUpdate_i.h"
#else
typedef enum WsusServer {
	e_none = 0,
	e_other = 1,
	e_legacyUpdateProxy = 2,
	e_legacyUpdateCustom = 3
} WsusServer;
#endif

EXTERN_C HRESULT IsUsingWsusServer(BOOL *retval);
EXTERN_C HRESULT GetWsusServerUrl(BSTR *retval);
EXTERN_C HRESULT GetWsusStatusServerUrl(BSTR *retval);

EXTERN_C HRESULT GetWsusServer(WsusServer *retval);
EXTERN_C HRESULT SetWsusServer(WsusServer server);

EXTERN_C BOOL IsUpdateSiteHttps(void);
