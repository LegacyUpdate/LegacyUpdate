#pragma once

#include <windows.h>

static inline BOOL IsUserAdmin(void) {
	SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;
	PSID adminsSid = NULL;
	BOOL result = FALSE;

	if (!AllocateAndInitializeSid(&authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminsSid)) {
		return FALSE;
	}

	if (!CheckTokenMembership(NULL, adminsSid, &result)) {
		result = FALSE;
	}

	FreeSid(adminsSid);
	return result;
}

static inline BOOL IsElevated(void) {
	HANDLE hToken = NULL;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		return FALSE;
	}

	TOKEN_ELEVATION elevation;
	DWORD size = sizeof(elevation);
	if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &size)) {
		return FALSE;
	}

	BOOL result = elevation.TokenIsElevated;
	CloseHandle(hToken);
	return result;
}

static inline DWORD GetTokenIntegrity(void) {
#if WINVER < _WIN32_WINNT_WIN2K
	return MAXDWORD;
#else
	HANDLE hToken = NULL;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		return MAXDWORD;
	}

	BYTE buffer[sizeof(TOKEN_MANDATORY_LABEL) + SECURITY_MAX_SID_SIZE];
	DWORD size = sizeof(buffer);
	if (!GetTokenInformation(hToken, TokenIntegrityLevel, buffer, size, &size)) {
		CloseHandle(hToken);
		return MAXDWORD;
	}

	CloseHandle(hToken);

	TOKEN_MANDATORY_LABEL *tokenLabel = (TOKEN_MANDATORY_LABEL *)buffer;
	return *GetSidSubAuthority(tokenLabel->Label.Sid, *GetSidSubAuthorityCount(tokenLabel->Label.Sid) - 1);
#endif
}
