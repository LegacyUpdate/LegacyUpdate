#pragma once

EXTERN_C BOOL WINAPI PrxDllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved);
EXTERN_C STDAPI PrxDllCanUnloadNow(void);
EXTERN_C STDAPI PrxDllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv);
EXTERN_C STDAPI PrxDllRegisterServer(void);
EXTERN_C STDAPI PrxDllUnregisterServer(void);
