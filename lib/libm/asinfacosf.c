/*****************************************************************************/
/*****************************************************************************/
// asinf from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

/* origin: FreeBSD /usr/src/lib/msun/src/e_asinf.c */
/*
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */
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

#include "libm.h"

// dpgeorge: pio2 was double in original implementation of asinf
static const float
pio2_hi = 1.5707962513e+00f, /* 0x3fc90fda */
pio2_lo = 7.5497894159e-08f; /* 0x33a22168 */

static const float
/* coefficients for R(x^2) */
pS0 =  1.6666586697e-01f,
pS1 = -4.2743422091e-02f,
pS2 = -8.6563630030e-03f,
qS1 = -7.0662963390e-01f;

static float R(float z)
{
	float_t p, q;
	p = z*(pS0+z*(pS1+z*pS2));
	q = 1.0f+z*qS1;
	return p/q;
}

float asinf(float x)
{
	// dpgeorge: s was double in original implementation
	float s,z;
	uint32_t hx,ix;

	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;
	if (ix >= 0x3f800000) {  /* |x| >= 1 */
		if (ix == 0x3f800000)  /* |x| == 1 */
			return x*pio2_hi + 0x1p-120f;  /* asin(+-1) = +-pi/2 with inexact */
		return 0/(x-x);  /* asin(|x|>1) is NaN */
	}
	if (ix < 0x3f000000) {  /* |x| < 0.5 */
		/* if 0x1p-126 <= |x| < 0x1p-12, avoid raising underflow */
		if (ix < 0x39800000 && ix >= 0x00800000)
			return x;
		return x + x*R(x*x);
	}
	/* 1 > |x| >= 0.5 */
	z = (1 - fabsf(x))*0.5f;
	s = sqrtf(z);
	x = pio2_hi - (2*(s+s*R(z)) - pio2_lo); // dpgeorge: use pio2_hi and pio2_lo
	if (hx >> 31)
		return -x;
	return x;
}

/*****************************************************************************/
/*****************************************************************************/
// acosf from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

/* origin: FreeBSD /usr/src/lib/msun/src/e_acosf.c */
/*
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */
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

float acosf(float x)
{
	float z,w,s,c,df;
	uint32_t hx,ix;

	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;
	/* |x| >= 1 or nan */
	if (ix >= 0x3f800000) {
		if (ix == 0x3f800000) {
			if (hx >> 31)
				return 2*pio2_hi + 0x1p-120f;
			return 0;
		}
		return 0/(x-x);
	}
	/* |x| < 0.5 */
	if (ix < 0x3f000000) {
		if (ix <= 0x32800000) /* |x| < 2**-26 */
			return pio2_hi + 0x1p-120f;
		return pio2_hi - (x - (pio2_lo-x*R(x*x)));
	}
	/* x < -0.5 */
	if (hx >> 31) {
		z = (1+x)*0.5f;
		s = sqrtf(z);
		w = R(z)*s-pio2_lo;
		return 2*(pio2_hi - (s+w));
	}
	/* x > 0.5 */
	z = (1-x)*0.5f;
	s = sqrtf(z);
	GET_FLOAT_WORD(hx,s);
	SET_FLOAT_WORD(df,hx&0xfffff000);
	c = (z-df*df)/(s+df);
	w = R(z)*s+c;
	return 2*(df+w);
}
