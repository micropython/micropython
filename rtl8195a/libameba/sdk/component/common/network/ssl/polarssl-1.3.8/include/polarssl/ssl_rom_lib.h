#include <rt_lib_rom.h>
#define memcpy		_memcpy
#define memcmp		_memcmp
#define memset		_memset
#define sscanf		_sscanf
#define strncpy		_strncpy
#define strnlen		_strnlen
#define strlen		_strlen
#define snprintf	DiagSnPrintf

#define strstr		__rtl_strstr
#define vsnprintf	__rtl_vfprintf_r
#define memmove		__rtl_memmove

/* these functions will be move to rom libc */
#ifndef SSL_LIBC_ROM_PATCH
#define SSL_LIBC_ROM_PATCH

//SSL_ROM_TEXT_SECTION
static inline char __rtl_tolower_v1_00(const char c)
{
	return c | 0x20;
}

//SSL_ROM_TEXT_SECTION
static inline int __rtl_strcasecmp_v1_00(const char *s1, const char *s2)
{
	const unsigned char *ucs1 = (const unsigned char *) s1;
	const unsigned char *ucs2 = (const unsigned char *) s2;
	int d = 0;
	for ( ; ; )
	{
		const int c1 = __rtl_tolower_v1_00(*ucs1++);
		const int c2 = __rtl_tolower_v1_00(*ucs2++);
		if (((d = c1 - c2) != 0) || (c2 == '\0'))
			break;
	}
	return d;
}

//SSL_ROM_TEXT_SECTION
static inline int __rtl_strncasecmp_v1_00(const char *s1, const char *s2, size_t n)
{
	const unsigned char *ucs1 = (const unsigned char *) s1;
	const unsigned char *ucs2 = (const unsigned char *) s2;
	int d = 0;
	for ( ; n != 0; n--)
	{
		const int c1 = __rtl_tolower_v1_00(*ucs1++);
		const int c2 = __rtl_tolower_v1_00(*ucs2++);
		if (((d = c1 - c2) != 0) || (c2 == '\0'))
			break;
	}
	return d;
}
#endif
#define strcasecmp	__rtl_strcasecmp_v1_00
#define strncasecmp	__rtl_strncasecmp_v1_00

//#undef POLARSSL_HAVE_UDBL in bignum.h

