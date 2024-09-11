#pragma once

#if _MSC_VER > 1599
	// VC17: Windows XP
	#define WINVER _WIN32_WINNT_WINXP
	#define _WIN32_WINNT _WIN32_WINNT_WINXP
#else
	// VC08: Windows 2000 RTM
	#define WINVER _WIN32_WINNT_WIN2K
	#define _WIN32_WINNT _WIN32_WINNT_WIN2K
#endif
