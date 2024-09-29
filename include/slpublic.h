#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef GUID SLID;
typedef PVOID HSLC;

DEFINE_GUID(WINDOWS_SLID, 0x55c92734, 0xd682, 0x4d71, 0x98, 0x3e, 0xd6, 0xec, 0x3f, 0x16, 0x05, 0x9f);

typedef enum _tagSLLICENSINGSTATUS {
	SL_LICENSING_STATUS_UNLICENSED,
	SL_LICENSING_STATUS_LICENSED,
	SL_LICENSING_STATUS_IN_GRACE_PERIOD,
	SL_LICENSING_STATUS_NOTIFICATION,
	SL_LICENSING_STATUS_LAST
} SLLICENSINGSTATUS;

typedef struct _tagSL_LICENSING_STATUS {
	SLID SkuId;
	SLLICENSINGSTATUS eStatus;
	DWORD dwGraceTime;
	DWORD dwTotalGraceDays;
	HRESULT hrReason;
	UINT64 qwValidityExpiration;
} SL_LICENSING_STATUS;

#ifdef __cplusplus
}
#endif
