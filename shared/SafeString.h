#pragma once

#ifdef __CRT__NO_INLINE
	#include <strsafe.h>
#else
	#define __CRT__NO_INLINE
	#include <strsafe.h>
	#undef __CRT__NO_INLINE
#endif
