/*
 * OS specific functions
 * Copyright (c) 2005-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef OS_H
#define OS_H

//#include "basic_types.h"
#include <autoconf.h>
#include "osdep_service.h"
#include "utils/rom/rom_wps_os.h"

typedef void* xqueue_handle_t;

typedef long os_time_t;

typedef _timer os_timer;

/**
 * os_sleep - Sleep (sec, usec)
 * @sec: Number of seconds to sleep
 * @usec: Number of microseconds to sleep
 */
void os_sleep(os_time_t sec, os_time_t usec);

struct os_time {
	os_time_t sec;
	os_time_t usec;
};

struct os_reltime {
	os_time_t sec;
	os_time_t usec;
};

/**
 * os_get_time - Get current time (sec, usec)
 * @t: Pointer to buffer for the time
 * Returns: 0 on success, -1 on failure
 */
int os_get_time(struct os_time *t);

int os_get_reltime(struct os_reltime *t);
/* Helper macros for handling struct os_time */
/* (&timeout->time, &tmp->time) */
#define os_time_before(a, b) \
	((a)->sec < (b)->sec || \
	 ((a)->sec == (b)->sec && (a)->usec < (b)->usec))

#define os_time_sub(a, b, res) do { \
	(res)->sec = (a)->sec - (b)->sec; \
	(res)->usec = (a)->usec - (b)->usec; \
	if ((res)->usec < 0) { \
		(res)->sec--; \
		(res)->usec += 1000000; \
	} \
} while (0)

/**
 * os_mktime - Convert broken-down time into seconds since 1970-01-01
 * @year: Four digit year
 * @month: Month (1 .. 12)
 * @day: Day of month (1 .. 31)
 * @hour: Hour (0 .. 23)
 * @min: Minute (0 .. 59)
 * @sec: Second (0 .. 60)
 * @t: Buffer for returning calendar time representation (seconds since
 * 1970-01-01 00:00:00)
 * Returns: 0 on success, -1 on failure
 *
 * Note: The result is in seconds from Epoch, i.e., in UTC, not in local time
 * which is used by POSIX mktime().
 */
int os_mktime(int year, int month, int day, int hour, int min, int sec,
	      os_time_t *t);

struct os_tm {
	int sec; /* 0..59 or 60 for leap seconds */
	int min; /* 0..59 */
	int hour; /* 0..23 */
	int day; /* 1..31 */
	int month; /* 1..12 */
	int year; /* Four digit year */
};

int os_gmtime(os_time_t t, struct os_tm *tm);

/* Helpers for handling struct os_time */

/* Helpers for handling struct os_reltime */

static inline int os_reltime_before(struct os_reltime *a,
				    struct os_reltime *b)
{
	return os_time_before(a,b);
}


static inline void os_reltime_sub(struct os_reltime *a, struct os_reltime *b,
				  struct os_reltime *res)
{
	os_time_sub(a,b,res);
}


static inline void os_reltime_age(struct os_reltime *start,
				  struct os_reltime *age)
{
	struct os_reltime now;

	os_get_time((struct os_time *)&now);
	os_reltime_sub(&now, start, age);
}


static inline int os_reltime_expired(struct os_reltime *now,
				     struct os_reltime *ts,
				     os_time_t timeout_secs)
{
	struct os_reltime age;

	os_reltime_sub(now, ts, &age);
	return (age.sec > timeout_secs) ||
	       (age.sec == timeout_secs && age.usec > 0);
}

/**
 * os_daemonize - Run in the background (detach from the controlling terminal)
 * @pid_file: File name to write the process ID to or %NULL to skip this
 * Returns: 0 on success, -1 on failure
 */
int os_daemonize(const char *pid_file);

/**
 * os_daemonize_terminate - Stop running in the background (remove pid file)
 * @pid_file: File name to write the process ID to or %NULL to skip this
 */
void os_daemonize_terminate(const char *pid_file);

/**
 * os_get_random - Get cryptographically strong pseudo random data
 * @buf: Buffer for pseudo random data
 * @len: Length of the buffer
 * Returns: 0 on success, -1 on failure
 */
int os_get_random(unsigned char *buf, size_t len);

/**
 * os_random - Get pseudo random value (not necessarily very strong)
 * Returns: Pseudo random value
 */
unsigned long os_random(void);

/**
 * os_rel2abs_path - Get an absolute path for a file
 * @rel_path: Relative path to a file
 * Returns: Absolute path for the file or %NULL on failure
 *
 * This function tries to convert a relative path of a file to an absolute path
 * in order for the file to be found even if current working directory has
 * changed. The returned value is allocated and caller is responsible for
 * freeing it. It is acceptable to just return the same path in an allocated
 * buffer, e.g., return strdup(rel_path). This function is only used to find
 * configuration files when os_daemonize() may have changed the current working
 * directory and relative path would be pointing to a different location.
 */
char * os_rel2abs_path(const char *rel_path);

/**
 * os_program_init - Program initialization (called at start)
 * Returns: 0 on success, -1 on failure
 *
 * This function is called when a programs starts. If there are any OS specific
 * processing that is needed, it can be placed here. It is also acceptable to
 * just return 0 if not special processing is needed.
 */
int os_program_init(void);

/**
 * os_program_deinit - Program deinitialization (called just before exit)
 *
 * This function is called just before a program exists. If there are any OS
 * specific processing, e.g., freeing resourced allocated in os_program_init(),
 * it should be done here. It is also acceptable for this function to do
 * nothing.
 */
void os_program_deinit(void);

/**
 * os_setenv - Set environment variable
 * @name: Name of the variable
 * @value: Value to set to the variable
 * @overwrite: Whether existing variable should be overwritten
 * Returns: 0 on success, -1 on error
 *
 * This function is only used for wpa_cli action scripts. OS wrapper does not
 * need to implement this if such functionality is not needed.
 */
int os_setenv(const char *name, const char *value, int overwrite);

/**
 * os_unsetenv - Delete environent variable
 * @name: Name of the variable
 * Returns: 0 on success, -1 on error
 *
 * This function is only used for wpa_cli action scripts. OS wrapper does not
 * need to implement this if such functionality is not needed.
 */
int os_unsetenv(const char *name);

/**
 * os_readfile - Read a file to an allocated memory buffer
 * @name: Name of the file to read
 * @len: For returning the length of the allocated buffer
 * Returns: Pointer to the allocated buffer or %NULL on failure
 *
 * This function allocates memory and reads the given file to this buffer. Both
 * binary and text files can be read with this function. The caller is
 * responsible for freeing the returned buffer with os_free().
 */
char * os_readfile(const char *name, size_t *len);

#if 0
/**
 * os_zalloc - Allocate and zero memory
 * @size: Number of bytes to allocate
 * Returns: Pointer to allocated and zeroed memory or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer with os_free().
 */
void * os_zalloc(size_t size);

/**
 * os_calloc - Allocate and zero memory for an array
 * @nmemb: Number of members in the array
 * @size: Number of bytes in each member
 * Returns: Pointer to allocated and zeroed memory or %NULL on failure
 *
 * This function can be used as a wrapper for os_zalloc(nmemb * size) when an
 * allocation is used for an array. The main benefit over os_zalloc() is in
 * having an extra check to catch integer overflows in multiplication.
 *
 * Caller is responsible for freeing the returned buffer with os_free().
 */
static inline void * os_calloc(size_t nmemb, size_t size)
{
	if (size && nmemb > (~(size_t) 0) / size)
		return NULL;
	return os_zalloc(nmemb * size);
}
#endif

/*
 * The following functions are wrapper for standard ANSI C or POSIX functions.
 * By default, they are just defined to use the standard function name and no
 * os_*.c implementation is needed for them. This avoids extra function calls
 * by allowing the C pre-processor take care of the function name mapping.
 *
 * If the target system uses a C library that does not provide these functions,
 * build_config.h can be used to define the wrappers to use a different
 * function name. This can be done on function-by-function basis since the
 * defines here are only used if build_config.h does not define the os_* name.
 * If needed, os_*.c file can be used to implement the functions that are not
 * included in the C library on the target system. Alternatively,
 * OS_NO_C_LIB_DEFINES can be defined to skip all defines here in which case
 * these functions need to be implemented in os_*.c file for the target system.
 */

#ifdef OS_NO_C_LIB_DEFINES

/**
 * os_malloc - Allocate dynamic memory
 * @size: Size of the buffer to allocate
 * Returns: Allocated buffer or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer with os_free().
 */
void * os_malloc(size_t size);

/**
 * os_realloc - Re-allocate dynamic memory
 * @ptr: Old buffer from os_malloc() or os_realloc()
 * @size: Size of the new buffer
 * Returns: Allocated buffer or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer with os_free().
 * If re-allocation fails, %NULL is returned and the original buffer (ptr) is
 * not freed and caller is still responsible for freeing it.
 */
void * os_realloc(void *ptr, size_t size);

/**
 * os_free - Free dynamic memory
 * @ptr: Old buffer from os_malloc() or os_realloc(); can be %NULL
 */
void os_free(void *ptr);

/**
 * os_memcpy - Copy memory area
 * @dest: Destination
 * @src: Source
 * @n: Number of bytes to copy
 * Returns: dest
 *
 * The memory areas src and dst must not overlap. os_memmove() can be used with
 * overlapping memory.
 */
void * os_memcpy(void *dest, const void *src, size_t n);

/**
 * os_memmove - Copy memory area
 * @dest: Destination
 * @src: Source
 * @n: Number of bytes to copy
 * Returns: dest
 *
 * The memory areas src and dst may overlap.
 */
void *os_memmove(void *dest, const void *src, size_t n);

/**
 * os_memset - Fill memory with a constant byte
 * @s: Memory area to be filled
 * @c: Constant byte
 * @n: Number of bytes started from s to fill with c
 * Returns: s
 */
void *os_memset(void *s, int c, size_t n);

/**
 * os_memcmp - Compare memory areas
 * @s1: First buffer
 * @s2: Second buffer
 * @n: Maximum numbers of octets to compare
 * Returns: An integer less than, equal to, or greater than zero if s1 is
 * found to be less than, to match, or be greater than s2. Only first n
 * characters will be compared.
 */
int os_memcmp(const void *s1, const void *s2, size_t n);

/**
 * os_strdup - Duplicate a string
 * @s: Source string
 * Returns: Allocated buffer with the string copied into it or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer with os_free().
 */
char *os_strdup(const char *s);

/**
 * os_strlen - Calculate the length of a string
 * @s: '\0' terminated string
 * Returns: Number of characters in s (not counting the '\0' terminator)
 */
size_t os_strlen(const char *s);

/**
 * os_strcasecmp - Compare two strings ignoring case
 * @s1: First string
 * @s2: Second string
 * Returns: An integer less than, equal to, or greater than zero if s1 is
 * found to be less than, to match, or be greatred than s2
 */
int os_strcasecmp(const char *s1, const char *s2);

/**
 * os_strncasecmp - Compare two strings ignoring case
 * @s1: First string
 * @s2: Second string
 * @n: Maximum numbers of characters to compare
 * Returns: An integer less than, equal to, or greater than zero if s1 is
 * found to be less than, to match, or be greater than s2. Only first n
 * characters will be compared.
 */
int os_strncasecmp(const char *s1, const char *s2, size_t n);

/**
 * os_strchr - Locate the first occurrence of a character in string
 * @s: String
 * @c: Character to search for
 * Returns: Pointer to the matched character or %NULL if not found
 */
char *os_strchr(const char *s, int c);

/**
 * os_strrchr - Locate the last occurrence of a character in string
 * @s: String
 * @c: Character to search for
 * Returns: Pointer to the matched character or %NULL if not found
 */
char *os_strrchr(const char *s, int c);

/**
 * os_strcmp - Compare two strings
 * @s1: First string
 * @s2: Second string
 * Returns: An integer less than, equal to, or greater than zero if s1 is
 * found to be less than, to match, or be greatred than s2
 */
int os_strcmp(const char *s1, const char *s2);

/**
 * os_strncmp - Compare two strings
 * @s1: First string
 * @s2: Second string
 * @n: Maximum numbers of characters to compare
 * Returns: An integer less than, equal to, or greater than zero if s1 is
 * found to be less than, to match, or be greater than s2. Only first n
 * characters will be compared.
 */
int os_strncmp(const char *s1, const char *s2, size_t n);

/**
 * os_strncpy - Copy a string
 * @dest: Destination
 * @src: Source
 * @n: Maximum number of characters to copy
 * Returns: dest
 */
char *os_strncpy(char *dest, const char *src, size_t n);

/**
 * os_strstr - Locate a substring
 * @haystack: String (haystack) to search from
 * @needle: Needle to search from haystack
 * Returns: Pointer to the beginning of the substring or %NULL if not found
 */
char *os_strstr(const char *haystack, const char *needle);

/**
 * os_snprintf - Print to a memory buffer
 * @str: Memory buffer to print into
 * @size: Maximum length of the str buffer
 * @format: printf format
 * Returns: Number of characters printed (not including trailing '\0').
 *
 * If the output buffer is truncated, number of characters which would have
 * been written is returned. Since some C libraries return -1 in such a case,
 * the caller must be prepared on that value, too, to indicate truncation.
 *
 * Note: Some C library implementations of snprintf() may not guarantee null
 * termination in case the output is truncated. The OS wrapper function of
 * os_snprintf() should provide this guarantee, i.e., to null terminate the
 * output buffer if a C library version of the function is used and if that
 * function does not guarantee null termination.
 *
 * If the target system does not include snprintf(), see, e.g.,
 * http://www.ijs.si/software/snprintf/ for an example of a portable
 * implementation of snprintf.
 */
int os_snprintf(char *str, size_t size, const char *format, ...);

#else /* OS_NO_C_LIB_DEFINES */

#if !defined(CONFIG_PLATFORM_8195A) && !defined(CONFIG_PLATFORM_8711B)
#ifdef CONFIG_MEM_MONITOR
	u8* os_malloc(u32 sz);
	void os_mfree(u8 *pbuf, u32 sz);
	#ifndef os_free
			#define os_free(p, sz) os_mfree(((u8*)(p)), (sz))
	#endif
#else
	#ifndef os_malloc
			#define os_malloc(sz) _rtw_malloc(sz)
	#endif
	#ifndef os_free
			#define os_free(p, sz) _rtw_mfree(((u8*)(p)), (sz))
	#endif
#endif
#endif
	extern void *os_zalloc(size_t size);
	extern char *os_strdup(const char *string_copy_from); 

	#ifndef os_sleep
			#define os_sleep(s, us) rtw_mdelay_os((s)*1000 + (us)/1000)
	#endif
	#ifndef os_memcpy
		#define os_memcpy(d, s, n) rtw_memcpy((void*)(d), ((void*)(s)), (n))
	#endif
	#ifndef os_memmove
		#define os_memmove(d, s, n) memmove((d), (s), (n))
	#endif
	#ifndef os_memset
		#define os_memset(pbuf, c, sz) rtw_memset(pbuf, c, sz)
	#endif
	#ifndef os_memcmp
		#define os_memcmp(s1, s2, n) rtw_memcmp(((void*)(s1)), ((void*)(s2)), (n))
	#endif
	#ifndef os_memcmp_p2p
		#define os_memcmp_p2p(s1, s2, n) memcmp((s1), (s2), (n))
	#endif
	#ifndef os_get_random_bytes
		#define os_get_random_bytes(d,sz) rtw_get_random_bytes(((void*)(d)), (sz))
	#endif	
	#ifndef os_strlen
		#define os_strlen(s) strlen(s)
	#endif
	#ifndef os_strcasecmp
		#ifdef _MSC_VER
			#define os_strcasecmp(s1, s2) _stricmp((s1), (s2))
		#else
			#define os_strcasecmp(s1, s2) strcasecmp((s1), (s2))
		#endif
	#endif
	#ifndef os_strncasecmp
	#ifdef _MSC_VER
		#define os_strncasecmp(s1, s2, n) _strnicmp((s1), (s2), (n))
	#else
		#define os_strncasecmp(s1, s2, n) strncasecmp((s1), (s2), (n))
	#endif
	#endif
	#ifndef os_init_timer
		#define os_init_timer(t, p, f, x, n) rtw_init_timer((t), (p), (f), (x), (n))
	#endif
	#ifndef os_set_timer
		#define os_set_timer(t, d) rtw_set_timer((t), (d))
	#endif
	#ifndef os_cancel_timer
		#define os_cancel_timer(t) rtw_cancel_timer(t)
	#endif
	#ifndef os_del_timer
		#define os_del_timer(t) rtw_del_timer(t)
	#endif
	#ifndef os_atoi
		#define os_atoi(s) rtw_atoi(s)
	#endif

#ifndef os_strchr
#define os_strchr(s, c) strchr((s), (c))
#endif
#ifndef os_strcmp
#define os_strcmp(s1, s2) strcmp((s1), (s2))
#endif
#ifndef os_strncmp
#define os_strncmp(s1, s2, n) strncmp((s1), (s2), (n))
#endif
#ifndef os_strncpy
#define os_strncpy(d, s, n) strncpy((d), (s), (n))
#endif
#ifndef os_strrchr
#define os_strrchr(s, c) strrchr((s), (c))
#endif
#ifndef os_strstr
#define os_strstr(h, n) strstr((h), (n))
#endif

#ifndef os_snprintf
	#ifdef _MSC_VER
		#define os_snprintf _snprintf
	#else
		#define os_snprintf snprintf
	#endif
#endif

#endif /* OS_NO_C_LIB_DEFINES */


static inline void * os_realloc_array(void *ptr, size_t nmemb, size_t size)
{
	if (size && nmemb > (~(size_t) 0) / size)
		return NULL;
	return os_realloc(ptr, nmemb * size, nmemb * size);
}

void *os_xqueue_create(unsigned long uxQueueLength, unsigned long uxItemSize) ;

int os_xqueue_receive(xqueue_handle_t xQueue, void * const pvBuffer, unsigned long xSecsToWait);

void os_xqueue_delete(xqueue_handle_t xQueue );

int os_xqueue_send(xqueue_handle_t xQueue, const void * const pvItemToQueue, unsigned long xSecsToWait);


#endif /* OS_H */
