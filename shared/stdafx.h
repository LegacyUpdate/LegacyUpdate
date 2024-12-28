#if defined(_MSC_VER)
	#define ALWAYS_INLINE __forceinline
#else
	#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#endif
