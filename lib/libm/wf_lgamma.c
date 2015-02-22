/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * These math functions are taken from newlib-nano-2, the newlib/libm/math
 * directory, available from https://github.com/32bitmicro/newlib-nano-2.
 *
 * Appropriate copyright headers are reproduced below.
 */

/* wf_lgamma.c -- float version of w_lgamma.c.
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
 *
 */

#include "fdlibm.h"
#define _IEEE_LIBM 1
//#include <reent.h>
//#include <errno.h>

#ifdef __STDC__
	float lgammaf(float x)
#else
	float lgammaf(x)
	float x;
#endif
{
#ifdef _IEEE_LIBM
        int sign;
	return __ieee754_lgammaf_r(x,&sign);
#else
        float y;
	struct exception exc;
        y = __ieee754_lgammaf_r(x,&(_REENT_SIGNGAM(_REENT)));
        if(_LIB_VERSION == _IEEE_) return y;
        if(!finitef(y)&&finitef(x)) {
#ifndef HUGE_VAL 
#define HUGE_VAL inf
	    double inf = 0.0;

	    SET_HIGH_WORD(inf,0x7ff00000);	/* set inf to infinite */
#endif
	    exc.name = "lgammaf";
	    exc.err = 0;
	    exc.arg1 = exc.arg2 = (double)x;
            if (_LIB_VERSION == _SVID_)
               exc.retval = HUGE;
            else
               exc.retval = HUGE_VAL;
	    if(floorf(x)==x&&x<=(float)0.0) {
		/* lgammaf(-integer) */
		exc.type = SING;
		if (_LIB_VERSION == _POSIX_)
		   errno = EDOM;
		else if (!matherr(&exc)) {
		   errno = EDOM;
		}

            } else {
		/* lgammaf(finite) overflow */
		exc.type = OVERFLOW;
                if (_LIB_VERSION == _POSIX_)
		   errno = ERANGE;
                else if (!matherr(&exc)) {
                   errno = ERANGE;
		}
            }
	    if (exc.err != 0)
	       errno = exc.err;
            return (float)exc.retval; 
        } else
            return y;
#endif
}             

#ifdef _DOUBLE_IS_32BITS

#ifdef __STDC__
	double lgamma(double x)
#else
	double lgamma(x)
	double x;
#endif
{
	return (double) lgammaf((float) x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
