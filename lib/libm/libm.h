/*****************************************************************************/
/*****************************************************************************/
// portions extracted from musl-0.9.15 libm.h
/*****************************************************************************/
/*****************************************************************************/

/* origin: FreeBSD /usr/src/lib/msun/src/math_private.h */
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

#include <stdint.h>
#include <math.h>

#define LIBM_FUNC_WRAP(fun) mp_libm_##fun

#define FLT_EVAL_METHOD 0

#define FORCE_EVAL(x) do {                        \
	if (sizeof(x) == sizeof(float)) {         \
		volatile float __x;               \
		__x = (x);                        \
                (void)__x;                        \
	} else if (sizeof(x) == sizeof(double)) { \
		volatile double __x;              \
		__x = (x);                        \
                (void)__x;                        \
	} else {                                  \
		volatile long double __x;         \
		__x = (x);                        \
                (void)__x;                        \
	}                                         \
} while(0)

/* Get a 32 bit int from a float.  */
#define GET_FLOAT_WORD(w,d)                       \
do {                                              \
  union {float f; uint32_t i;} __u;               \
  __u.f = (d);                                    \
  (w) = __u.i;                                    \
} while (0)

/* Set a float from a 32 bit int.  */
#define SET_FLOAT_WORD(d,w)                       \
do {                                              \
      union {float f; uint32_t i;} __u;           \
      __u.i = (w);                                \
      (d) = __u.f;                                \
} while (0)

float LIBM_FUNC_WRAP(acosf)(float x);
float LIBM_FUNC_WRAP(acoshf)(float x);
float LIBM_FUNC_WRAP(asinf)(float x);
float LIBM_FUNC_WRAP(asinhf)(float x);
float LIBM_FUNC_WRAP(atan2f)(float y, float x);
float LIBM_FUNC_WRAP(atanf)(float x);
float LIBM_FUNC_WRAP(atanhf)(float x);
float LIBM_FUNC_WRAP(ceilf)(float x);
float LIBM_FUNC_WRAP(cosf)(float x);
float LIBM_FUNC_WRAP(coshf)(float x);
float LIBM_FUNC_WRAP(erfcf)(float x);
float LIBM_FUNC_WRAP(erff)(float x);
float LIBM_FUNC_WRAP(expf)(float x);
float LIBM_FUNC_WRAP(expm1f)(float x);
float LIBM_FUNC_WRAP(floorf)(float x);
float LIBM_FUNC_WRAP(fmodf)(float x, float y);
float LIBM_FUNC_WRAP(frexpf)(float x, int *exp);
float LIBM_FUNC_WRAP(ldexpf)(float x, int exp);
float LIBM_FUNC_WRAP(lgammaf)(float x);
float LIBM_FUNC_WRAP(lgammaf)(float x);
float LIBM_FUNC_WRAP(log10f)(float x);
float LIBM_FUNC_WRAP(log1pf)(float x);
float LIBM_FUNC_WRAP(log2f)(float x);
float LIBM_FUNC_WRAP(logf)(float x);
float LIBM_FUNC_WRAP(modff)(float x, float *iptr);
float LIBM_FUNC_WRAP(nearbyintf)(float x);
float LIBM_FUNC_WRAP(powf)(float x, float y);
float LIBM_FUNC_WRAP(sinf)(float x);
float LIBM_FUNC_WRAP(sinhf)(float x);
float LIBM_FUNC_WRAP(sqrtf)(float x);
float LIBM_FUNC_WRAP(tanf)(float x);
float LIBM_FUNC_WRAP(tanhf)(float x);
float LIBM_FUNC_WRAP(tgammaf)(float x);
float LIBM_FUNC_WRAP(truncf)(float x);

#ifndef NDEBUG
float LIBM_FUNC_WRAP(copysignf)(float x, float y);
#else
static inline float LIBM_FUNC_WRAP(copysignf)(float x, float y) {
    return copysignf(x, y);
}
#endif

static inline float LIBM_FUNC_WRAP(fabsf)(float x) {
    return fabsf(x);
}

static inline float LIBM_FUNC_WRAP(nanf)(const char *tagp) {
    return nanf(tagp);
}
