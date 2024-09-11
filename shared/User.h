#pragma once

#include <windows.h>

inline BOOL IsUserAnAdmin() {
	SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;
	PSID adminsSid;
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
