#pragma once

void Reboot();
BOOL GetVistaProductInfo(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType);

BOOL DisableWow64FsRedirection(PVOID *OldValue);
BOOL RevertWow64FsRedirection(PVOID OldValue);
