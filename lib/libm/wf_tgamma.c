/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * These math functions are taken from newlib-nano-2, the newlib/libm/math
 * directory, available from https://github.com/32bitmicro/newlib-nano-2.
 *
 * Appropriate copyright headers are reproduced below.
 */

/* w_gammaf.c -- float version of w_gamma.c.
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

#include "math.h"
#include "fdlibm.h"
#ifndef _IEEE_LIBM
#define _IEEE_LIBM 1
#endif

#ifdef __STDC__
	float tgammaf(float x)
#else
	float tgammaf(x)
	float x;
#endif
{
        float y;
	int local_signgam;
	if (!isfinite(x)) {
	  /* special cases: tgammaf(nan)=nan, tgammaf(inf)=inf, tgammaf(-inf)=nan */
	  return x + INFINITY;
	}
	y = expf(__ieee754_lgammaf_r(x,&local_signgam));
	if (local_signgam < 0) y = -y;
#ifdef _IEEE_LIBM
	return y;
#else
	if(_LIB_VERSION == _IEEE_) return y;

	if(!finitef(y)&&finitef(x)) {
	  if(floorf(x)==x&&x<=(float)0.0)
	    /* tgammaf pole */
	    return (float)__kernel_standard((double)x,(double)x,141);
	  else
	    /* tgammaf overflow */
	    return (float)__kernel_standard((double)x,(double)x,140);
	}
	return y;
#endif
}

#ifdef _DOUBLE_IS_32BITS

#ifdef __STDC__
	double tgamma(double x)
#else
	double tgamma(x)
	double x;
#endif
{
	return (double) tgammaf((float) x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
