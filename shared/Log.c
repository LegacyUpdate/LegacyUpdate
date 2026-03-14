#include "stdafx.h"
#include "Log.h"
#include "Version.h"
#include "VersionInfo.h"
#include "User.h"
#include "Wow64.h"

#define LOG_SHARED_NAME L"LegacyUpdateLog"

typedef struct {
	DWORD ownerPid;
	HANDLE hFile;
} LogSharedData;

static const struct {
	DWORD arch;
	LPCWSTR name;
} archNames[] = {
	{PROCESSOR_ARCHITECTURE_INTEL, L"x86"},
	{PROCESSOR_ARCHITECTURE_ARM,   L"ARM"},
	{PROCESSOR_ARCHITECTURE_IA64,  L"IA64"},
	{PROCESSOR_ARCHITECTURE_AMD64, L"x64"},
	{PROCESSOR_ARCHITECTURE_ARM64, L"ARM64"}
};

static const struct {
	DWORD rid;
	LPCWSTR name;
} integrityLevels[] = {
	{SECURITY_MANDATORY_UNTRUSTED_RID, L"untrusted"},
	{SECURITY_MANDATORY_LOW_RID,       L"low"},
	{SECURITY_MANDATORY_MEDIUM_RID,    L"medium"},
	{SECURITY_MANDATORY_HIGH_RID,      L"high"},
	{SECURITY_MANDATORY_SYSTEM_RID,    L"system"},
	{SECURITY_MANDATORY_PROTECTED_PROCESS_RID, L"protected process"},
	{MAXDWORD, L"n/a"}
};

static BOOL g_logInitialized = FALSE;
static HANDLE g_hLogFile  = INVALID_HANDLE_VALUE;
static HANDLE g_hSharedMem = NULL;

void LogInternal(LPCWSTR text) {
	if (g_hLogFile == INVALID_HANDLE_VALUE || text == NULL || wcslen(text) > LOG_LINE_MAXLEN) {
		return;
	}

	SYSTEMTIME time;
	GetLocalTime(&time);

	WCHAR line[LOG_LINE_MAXLEN + 39];
	int length = wsprintf(line, L"%04d-%02d-%02d %02d:%02d:%02d\t%ls[%d]\t%ls\r\n",
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond,
		L"" LOG_NAME, GetCurrentProcessId(),
		text);

	DWORD written = 0;
	WriteFile(g_hLogFile, line, length * sizeof(WCHAR), &written, NULL);
	FlushFileBuffers(g_hLogFile);
}

static void WriteLogBanner() {
	// Divider
	WCHAR divider[41];
	for (size_t i = 0; i < ARRAYSIZE(divider) - 1; i++) {
		divider[i] = L'=';
	}
	divider[ARRAYSIZE(divider) - 1] = L'\0';

	LogInternal(divider);

	// App version
	LogInternal(L"Legacy Update " VERSION_STRING);

	// OS info
	LPCWSTR archName = L"?";
	SYSTEM_INFO sysInfo;
	OurGetNativeSystemInfo(&sysInfo);
	for (size_t i = 0; i < ARRAYSIZE(archNames); i++) {
		if (sysInfo.wProcessorArchitecture == archNames[i].arch) {
			archName = archNames[i].name;
			break;
		}
	}

	OSVERSIONINFOEX *versionInfo = GetVersionInfo();
	LPCWSTR productType = versionInfo->wProductType == VER_NT_WORKSTATION ? L"Workstation" : L"Server";

	LOG(L"OS: %d.%d.%d%ls%ls %ls (%ls, Suite: %05x)",
		versionInfo->dwMajorVersion,
		versionInfo->dwMinorVersion,
		versionInfo->dwBuildNumber,
		versionInfo->szCSDVersion == NULL || lstrlen(versionInfo->szCSDVersion) == 0 ? L"" : L" ",
		versionInfo->szCSDVersion,
		archName,
		productType,
		versionInfo->wSuiteMask);

	// Launch info
	LOG(L"Command line: %ls", GetCommandLine());

	// Integrity
	DWORD integrity = GetTokenIntegrity();
	LPCWSTR integrityName = L"?";
	for (size_t i = 0; i < ARRAYSIZE(integrityLevels); i++) {
		if (integrity <= integrityLevels[i].rid) {
			integrityName = integrityLevels[i].name;
			break;
		}
	}

	LOG(L"%ls, %ls integrity", IsUserAdmin() ? L"Admin" : L"Non-admin", integrityName);
}

HRESULT OpenLog() {
	if (g_logInitialized) {
		return S_OK;
	}

	g_logInitialized = TRUE;

	// Use already-open global log if possible, otherwise try local
	g_hSharedMem = OpenFileMapping(FILE_MAP_READ, FALSE, L"Global\\" LOG_SHARED_NAME);
	if (!g_hSharedMem && GetLastError() == ERROR_FILE_NOT_FOUND) {
		g_hSharedMem = OpenFileMapping(FILE_MAP_READ, FALSE, L"Local\\" LOG_SHARED_NAME);
	}
	if (g_hSharedMem) {
		LogSharedData *sharedData = (LogSharedData *)MapViewOfFile(g_hSharedMem, FILE_MAP_READ, 0, 0, sizeof(LogSharedData));
		if (sharedData) {
			HANDLE hOwner = OpenProcess(PROCESS_DUP_HANDLE, FALSE, sharedData->ownerPid);
			if (hOwner) {
				DuplicateHandle(hOwner, sharedData->hFile, GetCurrentProcess(), &g_hLogFile, 0, FALSE, DUPLICATE_SAME_ACCESS);
				CloseHandle(hOwner);
			}

			UnmapViewOfFile(sharedData);
		}

		if (g_hLogFile != INVALID_HANDLE_VALUE) {
			SetFilePointer(g_hLogFile, 0, NULL, FILE_END);
			WriteLogBanner();
			return S_OK;
		}

		CloseHandle(g_hSharedMem);
		g_hSharedMem = NULL;
	}

	// No other processes have the log open, start it ourselves
	WCHAR logPath[MAX_PATH];
	GetWindowsDirectory(logPath, ARRAYSIZE(logPath));
	lstrcat(logPath, AtLeastWinVista() ? L"\\Logs\\LegacyUpdate.log" : L"\\Temp\\LegacyUpdate.log");

	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = FALSE;

	BOOL isLocalTemp = FALSE;
	g_hLogFile = CreateFile(logPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (g_hLogFile == INVALID_HANDLE_VALUE) {
		// Access denied? Use user temp instead
		GetTempPath(ARRAYSIZE(logPath), logPath);
		lstrcat(logPath, L"LegacyUpdate.log");
		g_hLogFile = CreateFile(logPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		isLocalTemp = TRUE;
	}

	HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
	if (hr == HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)) {
		// Continue previous log file
		SetFilePointer(g_hLogFile, 0, NULL, FILE_END);
	} else {
		CHECK_HR_OR_RETURN(L"OpenLog");

		// Write UTF-16LE BOM to make Notepad happy
		DWORD bom = 0xFEFF;
		DWORD written = 0;
		WriteFile(g_hLogFile, &bom, sizeof(WCHAR), &written, NULL);
	}

	// Publish for other processes to use
	LPCWSTR fileMappingName = isLocalTemp ? L"Local\\" LOG_SHARED_NAME : L"Global\\" LOG_SHARED_NAME;
	g_hSharedMem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(LogSharedData), fileMappingName);
	if (g_hSharedMem) {
		LogSharedData *sharedData = (LogSharedData *)MapViewOfFile(g_hSharedMem, FILE_MAP_WRITE, 0, 0, sizeof(LogSharedData));
		if (sharedData) {
			sharedData->ownerPid = GetCurrentProcessId();
			sharedData->hFile = g_hLogFile;
			UnmapViewOfFile(sharedData);
		}
	}

	WriteLogBanner();
	return S_OK;
}

void CloseLog(void) {
	if (g_hLogFile != INVALID_HANDLE_VALUE) {
		CloseHandle(g_hLogFile);
		g_hLogFile = INVALID_HANDLE_VALUE;
	}
	if (g_hSharedMem) {
		CloseHandle(g_hSharedMem);
		g_hSharedMem = NULL;
	}
}
