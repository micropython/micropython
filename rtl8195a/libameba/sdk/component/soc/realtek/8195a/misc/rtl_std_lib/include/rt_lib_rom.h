/*
 * rtl_lib.h
 *
 * Definitions for RTL library functions
 */
 
#ifndef _RTL_LIB_ROM_H_
#define	_RTL_LIB_ROM_H_


#include <basic_types.h>
#include <diag.h>

#include <reent.h>

#include "../libc/rom/string/rom_libc_string.h"
#include "../libgloss/rtl8195a/rom/rom_libgloss_retarget.h"

#ifndef _PTR
#define	_PTR		void *
#endif

#ifndef _AND
#define	_AND		,
#endif

#ifndef _NOARGS
#define	_NOARGS		void
#endif

#ifndef _CONST
#define	_CONST		const
#endif

#ifndef _VOLATILE
#define	_VOLATILE	volatile
#endif

#ifndef _SIGNED
#define	_SIGNED		signed
#endif

#ifndef _DOTS
#define	_DOTS		, ...
#endif

#ifndef _VOID
#define _VOID 		void
#endif


//
// RTL library functions in ROM
// 

#define __rtl_memset	__rtl_memset_v1_00
#define __rtl_memchr	__rtl_memchr_v1_00
#define __rtl_memmove	__rtl_memmove_v1_00
#define __rtl_strcmp	__rtl_strcmp_v1_00
#define __rtl_memcpy	__rtl_memcpy_v1_00



extern _LONG_CALL_ void * __rtl_memset_v1_00(void * m , int c , size_t n);
extern _LONG_CALL_ void * __rtl_memchr_v1_00(const void * src_void , int c , size_t length);
extern _LONG_CALL_ void * __rtl_memmove_v1_00( void * dst_void , const void * src_void , size_t length);
extern _LONG_CALL_ int __rtl_strcmp_v1_00(const char *s1 ,	const char *s2);
extern _LONG_CALL_ void * __rtl_memcpy_v1_00(void * __restrict dst0 , const void * __restrict src0 , size_t len0);


//
// rtl eabi functions
// 

#define __rtl_itod 		__rtl_itod_v1_00
#define __rtl_dtoi		__rtl_dtoi_v1_00
#define __rtl_uitof 	__rtl_uitof_v1_00
#define __rtl_uitod 	__rtl_uitod_v1_00



#define __rtl_dcmpeq 	__rtl_dcmpeq_v1_00
#define __rtl_dcmplt 	__rtl_dcmplt_v1_00
#define __rtl_dcmpgt 	__rtl_dcmpgt_v1_00


#define __rtl_dadd 		__rtl_dadd_v1_00
#define __rtl_dsub 		__rtl_dsub_v1_00
#define __rtl_dmul 		__rtl_dmul_v1_00
#define __rtl_ddiv 		__rtl_ddiv_v1_00

extern _LONG_CALL_ double __rtl_itod_v1_00(int lval);
extern _LONG_CALL_ int __rtl_dtoi_v1_00(double d);
extern _LONG_CALL_ float __rtl_uitof_v1_00(unsigned int lval);
extern _LONG_CALL_ double __rtl_uitod_v1_00(unsigned int lval);


extern _LONG_CALL_ int 	  __rtl_dcmpeq_v1_00(double a, double b);
extern _LONG_CALL_ int 	  __rtl_dcmplt_v1_00(double a, double b);
extern _LONG_CALL_ int 	  __rtl_dcmpgt_v1_00(double a, double b);


extern _LONG_CALL_ double __rtl_dadd_v1_00(double a, double b);
extern _LONG_CALL_ double __rtl_dsub_v1_00(double a, double b);
extern _LONG_CALL_ double __rtl_dmul_v1_00(double a, double b);
extern _LONG_CALL_ double __rtl_ddiv_v1_00(double a, double b);


//
// mprec
//

#include <reent.h>


typedef struct _Bigint _Bigint;


#define __rtl_Balloc	__rtl_Balloc_v1_00
#define __rtl_Bfree 	__rtl_Bfree_v1_00
#define __rtl_d2b       __rtl_d2b_v1_00
#define __rtl_i2b		__rtl_i2b_v1_00
#define __rtl_pow5mult	__rtl_pow5mult_v1_00
#define __rtl_multadd	__rtl_multadd_v1_00
#define __rtl_mult		__rtl_mult_v1_00
#define __rtl_hi0bits	__rtl_hi0bits_v1_00
#define __rtl_lshift	__rtl_lshift_v1_00
#define __rtl_cmp		__rtl_cmp_v1_00
#define __rtl_diff		__rtl_diff_v1_00


extern _LONG_CALL_ _Bigint * __rtl_Balloc_v1_00(struct _reent *ptr, int k);

extern _LONG_CALL_ void __rtl_Bfree_v1_00(struct _reent *ptr, _Bigint * v);

extern _LONG_CALL_ _Bigint * __rtl_d2b_v1_00(struct _reent * ptr, double _d, int *e, int *bits);
extern _LONG_CALL_ _Bigint * __rtl_i2b_v1_00(struct _reent *ptr, int i );
extern _LONG_CALL_ _Bigint * __rtl_pow5mult_v1_00(struct _reent * ptr, _Bigint *b, int k);
extern _LONG_CALL_ _Bigint * __rtl_multadd_v1_00(struct _reent *ptr, _Bigint * b, int m, int a);
extern _LONG_CALL_ _Bigint * __rtl_mult_v1_00(struct _reent *ptr, _Bigint *a, _Bigint *b);
extern _LONG_CALL_ int __rtl_hi0bits_v1_00(register __ULong x);
extern _LONG_CALL_ _Bigint *__rtl_lshift_v1_00(struct _reent *ptr, _Bigint *b, int k);
extern _LONG_CALL_ int __rtl_cmp_v1_00(_Bigint *a, _Bigint *b);
extern _LONG_CALL_ _Bigint *__rtl_diff_v1_00(struct _reent* ptr, _Bigint *a, _Bigint *b);

//
// dtoa
//

#define __rtl_dtoa_r	__rtl_dtoa_r_v1_00

extern char * __rtl_dtoa_r_v1_00(struct _reent *ptr, double _d, int mode, int ndigits, int *decpt, int *sign, char **rve);

//
// mallocr
//
#include <sys/config.h>
#include <reent.h>



#define __rom_mallocr_init	__rom_mallocr_init_v1_00

#define __rtl_calloc_r		__rtl_calloc_r_v1_00
#define __rtl_cfree_r 		__rtl_cfree_r_v1_00
#define __rtl_malloc_r		__rtl_malloc_r_v1_00
#define __rtl_free_r		__rtl_free_r_v1_00
#define __rtl_realloc_r		__rtl_realloc_r_v1_00
#define __rtl_memalign_r 	__rtl_memalign_r_v1_00
#define __rtl_valloc_r      __rtl_valloc_r_v1_00
#define __rtl_pvalloc_r     __rtl_pvalloc_r_v1_00


extern _LONG_CALL_ void __rom_mallocr_init_v1_00(void);


#define RARG struct _reent *reent_ptr,
extern _LONG_CALL_ void* __rtl_calloc_r_v1_00(RARG size_t n, size_t elem_size);
extern _LONG_CALL_ void __rtl_cfree_r_v1_00(void *mem);
extern _LONG_CALL_ void* __rtl_malloc_r_v1_00(RARG size_t bytes);
extern _LONG_CALL_ void __rtl_free_r_v1_00(RARG void* mem);
extern _LONG_CALL_ void* __rtl_realloc_r_v1_00(RARG void* oldmem, size_t bytes);
extern _LONG_CALL_ void* __rtl_memalign_r_v1_00(RARG size_t alignment, size_t bytes);
extern _LONG_CALL_ void* __rtl_valloc_r_v1_00(RARG size_t bytes);
extern _LONG_CALL_ void* __rtl_pvalloc_r_v1_00(RARG size_t bytes);


//
// stdio
//
extern int __rtl_errno;

#ifndef _READ_WRITE_RETURN_TYPE
#define _READ_WRITE_RETURN_TYPE 	_ssize_t
#endif

#ifndef _READ_WRITE_BUFSIZE_TYPE
#define _READ_WRITE_BUFSIZE_TYPE 	int
#endif

#define __rtl_sread		__rtl_sread_v1_00
#define __rtl_swrite	__rtl_swrite_v1_00
#define __rtl_seofread	__rtl_seofread_v1_00
#define __rtl_sseek		__rtl_sseek_v1_00
#define __rtl_sclose    __rtl_sclose_v1_00
#define __rtl_sbrk_r 	__rtl_sbrk_r_v1_00

extern _LONG_CALL_ _READ_WRITE_RETURN_TYPE __rtl_sread_v1_00(
       struct _reent *ptr,
       void *cookie,
       char *buf,
       _READ_WRITE_BUFSIZE_TYPE n);

extern _LONG_CALL_ _READ_WRITE_RETURN_TYPE __rtl_swrite_v1_00(
       struct _reent *ptr,
       void *cookie,
       char const *buf,
       _READ_WRITE_BUFSIZE_TYPE n);

extern _LONG_CALL_ _READ_WRITE_RETURN_TYPE __rtl_seofread_v1_00(
       struct _reent *_ptr,
       _PTR cookie,
       char *buf,
       _READ_WRITE_BUFSIZE_TYPE len);

extern _LONG_CALL_ _fpos_t __rtl_sseek_v1_00(
       struct _reent *ptr _AND
       void *cookie _AND
       _fpos_t offset _AND
       int whence);

extern _LONG_CALL_ int __rtl_sclose_v1_00(
       struct _reent *ptr _AND
       void *cookie);

extern _LONG_CALL_ void * __rtl_sbrk_r_v1_00(
     struct _reent *ptr,
     ptrdiff_t incr);

//
// vfprintf
//

#include <stdio.h>
#include <stdarg.h>

#define __rtl_fflush_r 			__rtl_fflush_r_v1_00
#define __rtl_vfprintf_r		__rtl_vfprintf_r_v1_00

extern _LONG_CALL_ int __rtl_fflush_r_v1_00(struct _reent *ptr, register FILE * fp);
extern _LONG_CALL_ int __rtl_vfprintf_r_v1_00(struct _reent *, FILE *, const char *, va_list);


#endif /* _RTL_LIB_ROM_H_ */
