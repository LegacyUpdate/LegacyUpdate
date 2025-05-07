#if defined(_MSC_VER)
	// MSVC-specific
	#define ALWAYS_INLINE __forceinline
	#define __builtin_bswap16 _byteswap_ushort

	#if defined(_DEBUG) && defined(__cplusplus)
		#define new DEBUG_NEW
	#endif
#else
	// GCC-specific
	#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#endif
