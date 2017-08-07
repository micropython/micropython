/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * This file is adapted from from newlib-nano-2, the newlib/libm/common/fdlib.h,
 * available from https://github.com/32bitmicro/newlib-nano-2.  The main change
 * is removal of anything to do with double precision.
 *
 * Appropriate copyright headers are reproduced below.
 */

/* @(#)fdlibm.h 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#include <math.h>

/* Default to XOPEN_MODE.  */
#define _XOPEN_MODE

/* Most routines need to check whether a float is finite, infinite, or not a
   number, and many need to know whether the result of an operation will
   overflow.  These conditions depend on whether the largest exponent is
   used for NaNs & infinities, or whether it's used for finite numbers.  The
   macros below wrap up that kind of information:

   FLT_UWORD_IS_FINITE(X)
	True if a positive float with bitmask X is finite.

   FLT_UWORD_IS_NAN(X)
	True if a positive float with bitmask X is not a number.

   FLT_UWORD_IS_INFINITE(X)
	True if a positive float with bitmask X is +infinity.

   FLT_UWORD_MAX
	The bitmask of FLT_MAX.

   FLT_UWORD_HALF_MAX
	The bitmask of FLT_MAX/2.

   FLT_UWORD_EXP_MAX
	The bitmask of the largest finite exponent (129 if the largest
	exponent is used for finite numbers, 128 otherwise).

   FLT_UWORD_LOG_MAX
	The bitmask of log(FLT_MAX), rounded down.  This value is the largest
	input that can be passed to exp() without producing overflow.

   FLT_UWORD_LOG_2MAX
	The bitmask of log(2*FLT_MAX), rounded down.  This value is the
	largest input than can be passed to cosh() without producing
	overflow.

   FLT_LARGEST_EXP
	The largest biased exponent that can be used for finite numbers
	(255 if the largest exponent is used for finite numbers, 254
	otherwise) */

#ifdef _FLT_LARGEST_EXPONENT_IS_NORMAL
#define FLT_UWORD_IS_FINITE(x) 1
#define FLT_UWORD_IS_NAN(x) 0
#define FLT_UWORD_IS_INFINITE(x) 0
#define FLT_UWORD_MAX 0x7fffffff
#define FLT_UWORD_EXP_MAX 0x43010000
#define FLT_UWORD_LOG_MAX 0x42b2d4fc
#define FLT_UWORD_LOG_2MAX 0x42b437e0
#define HUGE ((float)0X1.FFFFFEP128)
#else
#define FLT_UWORD_IS_FINITE(x) ((x)<0x7f800000L)
#define FLT_UWORD_IS_NAN(x) ((x)>0x7f800000L)
#define FLT_UWORD_IS_INFINITE(x) ((x)==0x7f800000L)
#define FLT_UWORD_MAX 0x7f7fffffL
#define FLT_UWORD_EXP_MAX 0x43000000
#define FLT_UWORD_LOG_MAX 0x42b17217
#define FLT_UWORD_LOG_2MAX 0x42b2d4fc
#define HUGE ((float)3.40282346638528860e+38)
#endif
#define FLT_UWORD_HALF_MAX (FLT_UWORD_MAX-(1L<<23))
#define FLT_LARGEST_EXP (FLT_UWORD_MAX>>23)

/* Many routines check for zero and subnormal numbers.  Such things depend
   on whether the target supports denormals or not:

   FLT_UWORD_IS_ZERO(X)
	True if a positive float with bitmask X is +0.	Without denormals,
	any float with a zero exponent is a +0 representation.	With
	denormals, the only +0 representation is a 0 bitmask.

   FLT_UWORD_IS_SUBNORMAL(X)
	True if a non-zero positive float with bitmask X is subnormal.
	(Routines should check for zeros first.)

   FLT_UWORD_MIN
	The bitmask of the smallest float above +0.  Call this number
	REAL_FLT_MIN...

   FLT_UWORD_EXP_MIN
	The bitmask of the float representation of REAL_FLT_MIN's exponent.

   FLT_UWORD_LOG_MIN
	The bitmask of |log(REAL_FLT_MIN)|, rounding down.

   FLT_SMALLEST_EXP
	REAL_FLT_MIN's exponent - EXP_BIAS (1 if denormals are not supported,
	-22 if they are).
*/

#ifdef _FLT_NO_DENORMALS
#define FLT_UWORD_IS_ZERO(x) ((x)<0x00800000L)
#define FLT_UWORD_IS_SUBNORMAL(x) 0
#define FLT_UWORD_MIN 0x00800000
#define FLT_UWORD_EXP_MIN 0x42fc0000
#define FLT_UWORD_LOG_MIN 0x42aeac50
#define FLT_SMALLEST_EXP 1
#else
#define FLT_UWORD_IS_ZERO(x) ((x)==0)
#define FLT_UWORD_IS_SUBNORMAL(x) ((x)<0x00800000L)
#define FLT_UWORD_MIN 0x00000001
#define FLT_UWORD_EXP_MIN 0x43160000
#define FLT_UWORD_LOG_MIN 0x42cff1b5
#define FLT_SMALLEST_EXP -22
#endif

#ifdef __STDC__
#undef __P
#define	__P(p)	p
#else
#define	__P(p)	()
#endif

/* 
 * set X_TLOSS = pi*2**52, which is possibly defined in <values.h>
 * (one may replace the following line by "#include <values.h>")
 */

#define X_TLOSS		1.41484755040568800000e+16 

/* Functions that are not documented, and are not in <math.h>.  */

/* Undocumented float functions.  */
#ifdef _SCALB_INT
extern float scalbf __P((float, int));
#else
extern float scalbf __P((float, float));
#endif
extern float significandf __P((float));

/* ieee style elementary float functions */
extern float __ieee754_sqrtf __P((float));			
extern float __ieee754_acosf __P((float));			
extern float __ieee754_acoshf __P((float));			
extern float __ieee754_logf __P((float));			
extern float __ieee754_atanhf __P((float));			
extern float __ieee754_asinf __P((float));			
extern float __ieee754_atan2f __P((float,float));			
extern float __ieee754_expf __P((float));
extern float __ieee754_coshf __P((float));
extern float __ieee754_fmodf __P((float,float));
extern float __ieee754_powf __P((float,float));
extern float __ieee754_lgammaf_r __P((float,int *));
extern float __ieee754_gammaf_r __P((float,int *));
extern float __ieee754_log10f __P((float));
extern float __ieee754_sinhf __P((float));
extern float __ieee754_hypotf __P((float,float));
extern float __ieee754_j0f __P((float));
extern float __ieee754_j1f __P((float));
extern float __ieee754_y0f __P((float));
extern float __ieee754_y1f __P((float));
extern float __ieee754_jnf __P((int,float));
extern float __ieee754_ynf __P((int,float));
extern float __ieee754_remainderf __P((float,float));
extern __int32_t __ieee754_rem_pio2f __P((float,float*));
#ifdef _SCALB_INT
extern float __ieee754_scalbf __P((float,int));
#else
extern float __ieee754_scalbf __P((float,float));
#endif

/* float versions of fdlibm kernel functions */
extern float __kernel_sinf __P((float,float,int));
extern float __kernel_cosf __P((float,float));
extern float __kernel_tanf __P((float,float,int));
extern int   __kernel_rem_pio2f __P((float*,float*,int,int,int,const __int32_t*));

/* A union which permits us to convert between a float and a 32 bit
   int.  */

typedef union
{
  float value;
  __uint32_t word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

/* Macros to avoid undefined behaviour that can arise if the amount
   of a shift is exactly equal to the size of the shifted operand.  */

#define SAFE_LEFT_SHIFT(op,amt)					\
  (((amt) < 8 * sizeof(op)) ? ((op) << (amt)) : 0)

#define SAFE_RIGHT_SHIFT(op,amt)				\
  (((amt) < 8 * sizeof(op)) ? ((op) >> (amt)) : 0)
