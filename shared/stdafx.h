#if defined(_MSC_VER)
	#define ALWAYS_INLINE __forceinline
	#define __builtin_bswap16 _byteswap_ushort
#else
	#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#endif
