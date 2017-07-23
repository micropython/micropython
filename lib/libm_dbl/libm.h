// Portions of this file are extracted from musl-1.1.16 src/internal/libm.h

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

/* Get two 32 bit ints from a double.  */
#define EXTRACT_WORDS(hi,lo,d)                    \
do {                                              \
  union {double f; uint64_t i;} __u;              \
  __u.f = (d);                                    \
  (hi) = __u.i >> 32;                             \
  (lo) = (uint32_t)__u.i;                         \
} while (0)

/* Get the more significant 32 bit int from a double.  */
#define GET_HIGH_WORD(hi,d)                       \
do {                                              \
  union {double f; uint64_t i;} __u;              \
  __u.f = (d);                                    \
  (hi) = __u.i >> 32;                             \
} while (0)

/* Get the less significant 32 bit int from a double.  */
#define GET_LOW_WORD(lo,d)                        \
do {                                              \
  union {double f; uint64_t i;} __u;              \
  __u.f = (d);                                    \
  (lo) = (uint32_t)__u.i;                         \
} while (0)

/* Set a double from two 32 bit ints.  */
#define INSERT_WORDS(d,hi,lo)                     \
do {                                              \
  union {double f; uint64_t i;} __u;              \
  __u.i = ((uint64_t)(hi)<<32) | (uint32_t)(lo);  \
  (d) = __u.f;                                    \
} while (0)

/* Set the more significant 32 bits of a double from an int.  */
#define SET_HIGH_WORD(d,hi)                       \
do {                                              \
  union {double f; uint64_t i;} __u;              \
  __u.f = (d);                                    \
  __u.i &= 0xffffffff;                            \
  __u.i |= (uint64_t)(hi) << 32;                  \
  (d) = __u.f;                                    \
} while (0)

/* Set the less significant 32 bits of a double from an int.  */
#define SET_LOW_WORD(d,lo)                        \
do {                                              \
  union {double f; uint64_t i;} __u;              \
  __u.f = (d);                                    \
  __u.i &= 0xffffffff00000000ull;                 \
  __u.i |= (uint32_t)(lo);                        \
  (d) = __u.f;                                    \
} while (0)

#define DBL_EPSILON 2.22044604925031308085e-16

int __rem_pio2(double, double*);
int __rem_pio2_large(double*, double*, int, int, int);
double __sin(double, double, int);
double __cos(double, double);
double __tan(double, double, int);
double __expo2(double);
