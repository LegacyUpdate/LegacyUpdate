#pragma once

#include <windows.h>

BOOL DisableWow64FsRedirection(PVOID *OldValue);
BOOL RevertWow64FsRedirection(PVOID OldValue);
