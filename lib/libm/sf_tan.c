/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * These math functions are taken from newlib-nano-2, the newlib/libm/math
 * directory, available from https://github.com/32bitmicro/newlib-nano-2.
 *
 * Appropriate copyright headers are reproduced below.
 */

/* sf_tan.c -- float version of s_tan.c.
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

#include "fdlibm.h"

#ifdef __STDC__
	float tanf(float x)
#else
	float tanf(x)
	float x;
#endif
{
	float y[2],z=0.0;
	__int32_t n,ix;

	GET_FLOAT_WORD(ix,x);

    /* |x| ~< pi/4 */
	ix &= 0x7fffffff;
	if(ix <= 0x3f490fda) return __kernel_tanf(x,z,1);

    /* tan(Inf or NaN) is NaN */
	else if (!FLT_UWORD_IS_FINITE(ix)) return x-x;		/* NaN */

    /* argument reduction needed */
	else {
	    n = __ieee754_rem_pio2f(x,y);
	    return __kernel_tanf(y[0],y[1],1-((n&1)<<1)); /*   1 -- n even
							      -1 -- n odd */
	}
}

#ifdef _DOUBLE_IS_32BITS

#ifdef __STDC__
	double tan(double x)
#else
	double tan(x)
	double x;
#endif
{
	return (double) tanf((float) x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
