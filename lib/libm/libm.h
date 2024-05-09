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

// These lines verify that FLT_EVAL_METHOD==0, MicroPython's libm requires this.
// If compilation fails here then check the host compiler's FLT_EVAL_METHOD.
typedef float float_t;
typedef double double_t;

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
