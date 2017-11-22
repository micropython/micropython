/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * These math functions are taken from newlib-nano-2, the newlib/libm/math
 * directory, available from https://github.com/32bitmicro/newlib-nano-2.
 *
 * Appropriate copyright headers are reproduced below.
 */

/* sf_frexp.c -- float version of s_frexp.c.
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
static const float
#else
static float
#endif
two25 =  3.3554432000e+07; /* 0x4c000000 */

#ifdef __STDC__
	float frexpf(float x, int *eptr)
#else
	float frexpf(x, eptr)
	float x; int *eptr;
#endif
{
	__int32_t hx, ix;
	GET_FLOAT_WORD(hx,x);
	ix = 0x7fffffff&hx;
	*eptr = 0;
	if(!FLT_UWORD_IS_FINITE(ix)||FLT_UWORD_IS_ZERO(ix)) return x;	/* 0,inf,nan */
	if (FLT_UWORD_IS_SUBNORMAL(ix)) {		/* subnormal */
	    x *= two25;
	    GET_FLOAT_WORD(hx,x);
	    ix = hx&0x7fffffff;
	    *eptr = -25;
	}
	*eptr += (ix>>23)-126;
	hx = (hx&0x807fffff)|0x3f000000;
	SET_FLOAT_WORD(x,hx);
	return x;
}

#ifdef _DOUBLE_IS_32BITS

#ifdef __STDC__
	double frexp(double x, int *eptr)
#else
	double frexp(x, eptr)
	double x; int *eptr;
#endif
{
	return (double) frexpf((float) x, eptr);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
