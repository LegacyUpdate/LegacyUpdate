#pragma once

#include "stdafx.h"
#include <windows.h>
#include <knownfolders.h>
#include <shlobj.h>
#include "Exec.h"
#include "HResult.h"
#include "VersionInfo.h"

typedef enum LogAction {
	LogActionSystem,
	LogActionLocal,
	LogActionLocalLow,
	LogActionWindowsUpdate
} LogAction;

HRESULT ViewLog(LogAction log, int nCmdShow, BOOL showErrors);
