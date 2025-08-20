#pragma once

BOOL GetVistaProductInfo(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType);
void BecomeDPIAware();
void IsolationAwareStart(ULONG_PTR *cookie);
void IsolationAwareEnd(ULONG_PTR *cookie);
