#pragma once

#if _MSC_VER > 1599
	// VC17: Windows XP
	#define WINVER 0x0501
	#define _WIN32_WINNT 0x0501
#else
	// VC08: Windows 2000 RTM
	#define WINVER 0x0500
	#define _WIN32_WINNT 0x0500
#endif
