/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "libm.h"

typedef float float_t;
typedef union {
    float f;
    struct {
        uint64_t m : 23;
        uint64_t e : 8;
        uint64_t s : 1;
    };
} float_s_t;

typedef union {
    double d;
    struct {
        uint64_t m : 52;
        uint64_t e : 11;
        uint64_t s : 1;
    };
} double_s_t;

#if defined(__thumb__)

double __attribute__((pcs("aapcs"))) __aeabi_i2d(int32_t x) {
    return (float)x;
}

// TODO
long long __attribute__((pcs("aapcs"))) __aeabi_f2lz(float x) {
    return (long)x;
}

double __attribute__((pcs("aapcs"))) __aeabi_f2d(float x) {
    float_s_t fx={0};
    double_s_t dx={0};

    fx.f = x;
    dx.s = (fx.s);
    dx.e = (fx.e-127+1023) & 0x7FF;
    dx.m = fx.m;
    dx.m <<=(52-23); // left justify
    return dx.d;
}

float __attribute__((pcs("aapcs"))) __aeabi_d2f(double x) {
    float_s_t fx={0};
    double_s_t dx={0};

    dx.d = x;
    fx.s = (dx.s);
    fx.e = (dx.e-1023+127) & 0xFF;
    fx.m = (dx.m>>(52-23)); // right justify
    return fx.f;
}

double __aeabi_dmul(double x , double y) {
    return 0.0;

}

#endif // defined(__thumb__)

// TODO this needs a better way of testing for Thumb2 FP hardware
#if defined(__thumb2__)

float sqrtf(float x) {
    asm volatile (
            "vsqrt.f32  %[r], %[x]\n"
            : [r] "=t" (x)
            : [x] "t"  (x));
    return x;
}

#endif

#ifndef NDEBUG
float copysignf(float x, float y) {
    float_s_t fx={.f = x};
    float_s_t fy={.f = y};

    // copy sign bit;
    fx.s = fy.s;

    return fx.f;
}
#endif

// some compilers define log2f in terms of logf
#ifdef log2f
#undef log2f
#endif
// some compilers have _M_LN2 defined in math.h, some don't
#ifndef _M_LN2
#define _M_LN2 (0.69314718055994530942)
#endif
float log2f(float x) { return logf(x) / (float)_M_LN2; }

static const float _M_LN10 = 2.30258509299404; // 0x40135d8e
float log10f(float x) { return logf(x) / (float)_M_LN10; }

float tanhf(float x) { return sinhf(x) / coshf(x); }

/*****************************************************************************/
/*****************************************************************************/
// __fpclassifyf from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

int __fpclassifyf(float x)
{
	union {float f; uint32_t i;} u = {x};
	int e = u.i>>23 & 0xff;
	if (!e) return u.i<<1 ? FP_SUBNORMAL : FP_ZERO;
	if (e==0xff) return u.i<<9 ? FP_NAN : FP_INFINITE;
	return FP_NORMAL;
}

/*****************************************************************************/
/*****************************************************************************/
// scalbnf from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

float scalbnf(float x, int n)
{
	union {float f; uint32_t i;} u;
	float_t y = x;

	if (n > 127) {
		y *= 0x1p127f;
		n -= 127;
		if (n > 127) {
			y *= 0x1p127f;
			n -= 127;
			if (n > 127)
				n = 127;
		}
	} else if (n < -126) {
		y *= 0x1p-126f;
		n += 126;
		if (n < -126) {
			y *= 0x1p-126f;
			n += 126;
			if (n < -126)
				n = -126;
		}
	}
	u.i = (uint32_t)(0x7f+n)<<23;
	x = y * u.f;
	return x;
}

/*****************************************************************************/
/*****************************************************************************/
// powf from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

/* origin: FreeBSD /usr/src/lib/msun/src/e_powf.c */
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

static const float
bp[]   = {1.0, 1.5,},
dp_h[] = { 0.0, 5.84960938e-01,}, /* 0x3f15c000 */
dp_l[] = { 0.0, 1.56322085e-06,}, /* 0x35d1cfdc */
two24  =  16777216.0,  /* 0x4b800000 */
huge   =  1.0e30,
tiny   =  1.0e-30,
/* poly coefs for (3/2)*(log(x)-2s-2/3*s**3 */
L1 =  6.0000002384e-01, /* 0x3f19999a */
L2 =  4.2857143283e-01, /* 0x3edb6db7 */
L3 =  3.3333334327e-01, /* 0x3eaaaaab */
L4 =  2.7272811532e-01, /* 0x3e8ba305 */
L5 =  2.3066075146e-01, /* 0x3e6c3255 */
L6 =  2.0697501302e-01, /* 0x3e53f142 */
P1 =  1.6666667163e-01, /* 0x3e2aaaab */
P2 = -2.7777778450e-03, /* 0xbb360b61 */
P3 =  6.6137559770e-05, /* 0x388ab355 */
P4 = -1.6533901999e-06, /* 0xb5ddea0e */
P5 =  4.1381369442e-08, /* 0x3331bb4c */
lg2     =  6.9314718246e-01, /* 0x3f317218 */
lg2_h   =  6.93145752e-01,   /* 0x3f317200 */
lg2_l   =  1.42860654e-06,   /* 0x35bfbe8c */
ovt     =  4.2995665694e-08, /* -(128-log2(ovfl+.5ulp)) */
cp      =  9.6179670095e-01, /* 0x3f76384f =2/(3ln2) */
cp_h    =  9.6191406250e-01, /* 0x3f764000 =12b cp */
cp_l    = -1.1736857402e-04, /* 0xb8f623c6 =tail of cp_h */
ivln2   =  1.4426950216e+00, /* 0x3fb8aa3b =1/ln2 */
ivln2_h =  1.4426879883e+00, /* 0x3fb8aa00 =16b 1/ln2*/
ivln2_l =  7.0526075433e-06; /* 0x36eca570 =1/ln2 tail*/

float powf(float x, float y)
{
	float z,ax,z_h,z_l,p_h,p_l;
	float y1,t1,t2,r,s,sn,t,u,v,w;
	int32_t i,j,k,yisint,n;
	int32_t hx,hy,ix,iy,is;

	GET_FLOAT_WORD(hx, x);
	GET_FLOAT_WORD(hy, y);
	ix = hx & 0x7fffffff;
	iy = hy & 0x7fffffff;

	/* x**0 = 1, even if x is NaN */
	if (iy == 0)
		return 1.0f;
	/* 1**y = 1, even if y is NaN */
	if (hx == 0x3f800000)
		return 1.0f;
	/* NaN if either arg is NaN */
	if (ix > 0x7f800000 || iy > 0x7f800000)
		return x + y;

	/* determine if y is an odd int when x < 0
	 * yisint = 0       ... y is not an integer
	 * yisint = 1       ... y is an odd int
	 * yisint = 2       ... y is an even int
	 */
	yisint  = 0;
	if (hx < 0) {
		if (iy >= 0x4b800000)
			yisint = 2; /* even integer y */
		else if (iy >= 0x3f800000) {
			k = (iy>>23) - 0x7f;         /* exponent */
			j = iy>>(23-k);
			if ((j<<(23-k)) == iy)
				yisint = 2 - (j & 1);
		}
	}

	/* special value of y */
	if (iy == 0x7f800000) {  /* y is +-inf */
		if (ix == 0x3f800000)      /* (-1)**+-inf is 1 */
			return 1.0f;
		else if (ix > 0x3f800000)  /* (|x|>1)**+-inf = inf,0 */
			return hy >= 0 ? y : 0.0f;
		else if (ix != 0)          /* (|x|<1)**+-inf = 0,inf if x!=0 */
			return hy >= 0 ? 0.0f: -y;
	}
	if (iy == 0x3f800000)    /* y is +-1 */
		return hy >= 0 ? x : 1.0f/x;
	if (hy == 0x40000000)    /* y is 2 */
		return x*x;
	if (hy == 0x3f000000) {  /* y is  0.5 */
		if (hx >= 0)     /* x >= +0 */
			return sqrtf(x);
	}

	ax = fabsf(x);
	/* special value of x */
	if (ix == 0x7f800000 || ix == 0 || ix == 0x3f800000) { /* x is +-0,+-inf,+-1 */
		z = ax;
		if (hy < 0)  /* z = (1/|x|) */
			z = 1.0f/z;
		if (hx < 0) {
			if (((ix-0x3f800000)|yisint) == 0) {
				z = (z-z)/(z-z); /* (-1)**non-int is NaN */
			} else if (yisint == 1)
				z = -z;          /* (x<0)**odd = -(|x|**odd) */
		}
		return z;
	}

	sn = 1.0f; /* sign of result */
	if (hx < 0) {
		if (yisint == 0) /* (x<0)**(non-int) is NaN */
			return (x-x)/(x-x);
		if (yisint == 1) /* (x<0)**(odd int) */
			sn = -1.0f;
	}

	/* |y| is huge */
	if (iy > 0x4d000000) { /* if |y| > 2**27 */
		/* over/underflow if x is not close to one */
		if (ix < 0x3f7ffff8)
			return hy < 0 ? sn*huge*huge : sn*tiny*tiny;
		if (ix > 0x3f800007)
			return hy > 0 ? sn*huge*huge : sn*tiny*tiny;
		/* now |1-x| is tiny <= 2**-20, suffice to compute
		   log(x) by x-x^2/2+x^3/3-x^4/4 */
		t = ax - 1;     /* t has 20 trailing zeros */
		w = (t*t)*(0.5f - t*(0.333333333333f - t*0.25f));
		u = ivln2_h*t;  /* ivln2_h has 16 sig. bits */
		v = t*ivln2_l - w*ivln2;
		t1 = u + v;
		GET_FLOAT_WORD(is, t1);
		SET_FLOAT_WORD(t1, is & 0xfffff000);
		t2 = v - (t1-u);
	} else {
		float s2,s_h,s_l,t_h,t_l;
		n = 0;
		/* take care subnormal number */
		if (ix < 0x00800000) {
			ax *= two24;
			n -= 24;
			GET_FLOAT_WORD(ix, ax);
		}
		n += ((ix)>>23) - 0x7f;
		j = ix & 0x007fffff;
		/* determine interval */
		ix = j | 0x3f800000;     /* normalize ix */
		if (j <= 0x1cc471)       /* |x|<sqrt(3/2) */
			k = 0;
		else if (j < 0x5db3d7)   /* |x|<sqrt(3)   */
			k = 1;
		else {
			k = 0;
			n += 1;
			ix -= 0x00800000;
		}
		SET_FLOAT_WORD(ax, ix);

		/* compute s = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) */
		u = ax - bp[k];   /* bp[0]=1.0, bp[1]=1.5 */
		v = 1.0f/(ax+bp[k]);
		s = u*v;
		s_h = s;
		GET_FLOAT_WORD(is, s_h);
		SET_FLOAT_WORD(s_h, is & 0xfffff000);
		/* t_h=ax+bp[k] High */
		is = ((ix>>1) & 0xfffff000) | 0x20000000;
		SET_FLOAT_WORD(t_h, is + 0x00400000 + (k<<21));
		t_l = ax - (t_h - bp[k]);
		s_l = v*((u - s_h*t_h) - s_h*t_l);
		/* compute log(ax) */
		s2 = s*s;
		r = s2*s2*(L1+s2*(L2+s2*(L3+s2*(L4+s2*(L5+s2*L6)))));
		r += s_l*(s_h+s);
		s2 = s_h*s_h;
		t_h = 3.0f + s2 + r;
		GET_FLOAT_WORD(is, t_h);
		SET_FLOAT_WORD(t_h, is & 0xfffff000);
		t_l = r - ((t_h - 3.0f) - s2);
		/* u+v = s*(1+...) */
		u = s_h*t_h;
		v = s_l*t_h + t_l*s;
		/* 2/(3log2)*(s+...) */
		p_h = u + v;
		GET_FLOAT_WORD(is, p_h);
		SET_FLOAT_WORD(p_h, is & 0xfffff000);
		p_l = v - (p_h - u);
		z_h = cp_h*p_h;  /* cp_h+cp_l = 2/(3*log2) */
		z_l = cp_l*p_h + p_l*cp+dp_l[k];
		/* log2(ax) = (s+..)*2/(3*log2) = n + dp_h + z_h + z_l */
		t = (float)n;
		t1 = (((z_h + z_l) + dp_h[k]) + t);
		GET_FLOAT_WORD(is, t1);
		SET_FLOAT_WORD(t1, is & 0xfffff000);
		t2 = z_l - (((t1 - t) - dp_h[k]) - z_h);
	}

	/* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
	GET_FLOAT_WORD(is, y);
	SET_FLOAT_WORD(y1, is & 0xfffff000);
	p_l = (y-y1)*t1 + y*t2;
	p_h = y1*t1;
	z = p_l + p_h;
	GET_FLOAT_WORD(j, z);
	if (j > 0x43000000)          /* if z > 128 */
		return sn*huge*huge;  /* overflow */
	else if (j == 0x43000000) {  /* if z == 128 */
		if (p_l + ovt > z - p_h)
			return sn*huge*huge;  /* overflow */
	} else if ((j&0x7fffffff) > 0x43160000)  /* z < -150 */ // FIXME: check should be  (uint32_t)j > 0xc3160000
		return sn*tiny*tiny;  /* underflow */
	else if (j == 0xc3160000) {  /* z == -150 */
		if (p_l <= z-p_h)
			return sn*tiny*tiny;  /* underflow */
	}
	/*
	 * compute 2**(p_h+p_l)
	 */
	i = j & 0x7fffffff;
	k = (i>>23) - 0x7f;
	n = 0;
	if (i > 0x3f000000) {   /* if |z| > 0.5, set n = [z+0.5] */
		n = j + (0x00800000>>(k+1));
		k = ((n&0x7fffffff)>>23) - 0x7f;  /* new k for n */
		SET_FLOAT_WORD(t, n & ~(0x007fffff>>k));
		n = ((n&0x007fffff)|0x00800000)>>(23-k);
		if (j < 0)
			n = -n;
		p_h -= t;
	}
	t = p_l + p_h;
	GET_FLOAT_WORD(is, t);
	SET_FLOAT_WORD(t, is & 0xffff8000);
	u = t*lg2_h;
	v = (p_l-(t-p_h))*lg2 + t*lg2_l;
	z = u + v;
	w = v - (z - u);
	t = z*z;
	t1 = z - t*(P1+t*(P2+t*(P3+t*(P4+t*P5))));
	r = (z*t1)/(t1-2.0f) - (w+z*w);
	z = 1.0f - (r - z);
	GET_FLOAT_WORD(j, z);
	j += n<<23;
	if ((j>>23) <= 0)  /* subnormal output */
		z = scalbnf(z, n);
	else
		SET_FLOAT_WORD(z, j);
	return sn*z;
}

/*****************************************************************************/
/*****************************************************************************/
// expf from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

/* origin: FreeBSD /usr/src/lib/msun/src/e_expf.c */
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

static const float
half[2] = {0.5,-0.5},
ln2hi   = 6.9314575195e-1f,  /* 0x3f317200 */
ln2lo   = 1.4286067653e-6f,  /* 0x35bfbe8e */
invln2  = 1.4426950216e+0f,  /* 0x3fb8aa3b */
/*
 * Domain [-0.34568, 0.34568], range ~[-4.278e-9, 4.447e-9]:
 * |x*(exp(x)+1)/(exp(x)-1) - p(x)| < 2**-27.74
 */
expf_P1 =  1.6666625440e-1f, /*  0xaaaa8f.0p-26 */
expf_P2 = -2.7667332906e-3f; /* -0xb55215.0p-32 */

float expf(float x)
{
	float_t hi, lo, c, xx, y;
	int k, sign;
	uint32_t hx;

	GET_FLOAT_WORD(hx, x);
	sign = hx >> 31;   /* sign bit of x */
	hx &= 0x7fffffff;  /* high word of |x| */

	/* special cases */
	if (hx >= 0x42aeac50) {  /* if |x| >= -87.33655f or NaN */
		if (hx >= 0x42b17218 && !sign) {  /* x >= 88.722839f */
			/* overflow */
			x *= 0x1p127f;
			return x;
		}
		if (sign) {
			/* underflow */
			FORCE_EVAL(-0x1p-149f/x);
			if (hx >= 0x42cff1b5)  /* x <= -103.972084f */
				return 0;
		}
	}

	/* argument reduction */
	if (hx > 0x3eb17218) {  /* if |x| > 0.5 ln2 */
		if (hx > 0x3f851592)  /* if |x| > 1.5 ln2 */
			k = invln2*x + half[sign];
		else
			k = 1 - sign - sign;
		hi = x - k*ln2hi;  /* k*ln2hi is exact here */
		lo = k*ln2lo;
		x = hi - lo;
	} else if (hx > 0x39000000) {  /* |x| > 2**-14 */
		k = 0;
		hi = x;
		lo = 0;
	} else {
		/* raise inexact */
		FORCE_EVAL(0x1p127f + x);
		return 1 + x;
	}

	/* x is now in primary range */
	xx = x*x;
	c = x - xx*(expf_P1+xx*expf_P2);
	y = 1 + (x*c/(2-c) - lo + hi);
	if (k == 0)
		return y;
	return scalbnf(y, k);
}

/*****************************************************************************/
/*****************************************************************************/
// expm1f from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

/* origin: FreeBSD /usr/src/lib/msun/src/s_expm1f.c */
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

static const float
o_threshold = 8.8721679688e+01, /* 0x42b17180 */
ln2_hi      = 6.9313812256e-01, /* 0x3f317180 */
ln2_lo      = 9.0580006145e-06, /* 0x3717f7d1 */
//invln2      = 1.4426950216e+00, /* 0x3fb8aa3b */
/*
 * Domain [-0.34568, 0.34568], range ~[-6.694e-10, 6.696e-10]:
 * |6 / x * (1 + 2 * (1 / (exp(x) - 1) - 1 / x)) - q(x)| < 2**-30.04
 * Scaled coefficients: Qn_here = 2**n * Qn_for_q (see s_expm1.c):
 */
Q1 = -3.3333212137e-2, /* -0x888868.0p-28 */
Q2 =  1.5807170421e-3; /*  0xcf3010.0p-33 */

float expm1f(float x)
{
	float_t y,hi,lo,c,t,e,hxs,hfx,r1,twopk;
	union {float f; uint32_t i;} u = {x};
	uint32_t hx = u.i & 0x7fffffff;
	int k, sign = u.i >> 31;

	/* filter out huge and non-finite argument */
	if (hx >= 0x4195b844) {  /* if |x|>=27*ln2 */
		if (hx > 0x7f800000)  /* NaN */
			return x;
		if (sign)
			return -1;
		if (x > o_threshold) {
			x *= 0x1p127f;
			return x;
		}
	}

	/* argument reduction */
	if (hx > 0x3eb17218) {           /* if  |x| > 0.5 ln2 */
		if (hx < 0x3F851592) {       /* and |x| < 1.5 ln2 */
			if (!sign) {
				hi = x - ln2_hi;
				lo = ln2_lo;
				k =  1;
			} else {
				hi = x + ln2_hi;
				lo = -ln2_lo;
				k = -1;
			}
		} else {
			k  = invln2*x + (sign ? -0.5f : 0.5f);
			t  = k;
			hi = x - t*ln2_hi;      /* t*ln2_hi is exact here */
			lo = t*ln2_lo;
		}
		x = hi-lo;
		c = (hi-x)-lo;
	} else if (hx < 0x33000000) {  /* when |x|<2**-25, return x */
		if (hx < 0x00800000)
			FORCE_EVAL(x*x);
		return x;
	} else
		k = 0;

	/* x is now in primary range */
	hfx = 0.5f*x;
	hxs = x*hfx;
	r1 = 1.0f+hxs*(Q1+hxs*Q2);
	t  = 3.0f - r1*hfx;
	e  = hxs*((r1-t)/(6.0f - x*t));
	if (k == 0)  /* c is 0 */
		return x - (x*e-hxs);
	e  = x*(e-c) - c;
	e -= hxs;
	/* exp(x) ~ 2^k (x_reduced - e + 1) */
	if (k == -1)
		return 0.5f*(x-e) - 0.5f;
	if (k == 1) {
		if (x < -0.25f)
			return -2.0f*(e-(x+0.5f));
		return 1.0f + 2.0f*(x-e);
	}
	u.i = (0x7f+k)<<23;  /* 2^k */
	twopk = u.f;
	if (k < 0 || k > 56) {   /* suffice to return exp(x)-1 */
		y = x - e + 1.0f;
		if (k == 128)
			y = y*2.0f*0x1p127f;
		else
			y = y*twopk;
		return y - 1.0f;
	}
	u.i = (0x7f-k)<<23;  /* 2^-k */
	if (k < 23)
		y = (x-e+(1-u.f))*twopk;
	else
		y = (x-(e+u.f)+1)*twopk;
	return y;
}

/*****************************************************************************/
/*****************************************************************************/
// __expo2f from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

/* k is such that k*ln2 has minimal relative error and x - kln2 > log(FLT_MIN) */
static const int k = 235;
static const float kln2 = 0x1.45c778p+7f;

/* expf(x)/2 for x >= log(FLT_MAX), slightly better than 0.5f*expf(x/2)*expf(x/2) */
float __expo2f(float x)
{
	float scale;

	/* note that k is odd and scale*scale overflows */
	SET_FLOAT_WORD(scale, (uint32_t)(0x7f + k/2) << 23);
	/* exp(x - k ln2) * 2**(k-1) */
	return expf(x - kln2) * scale * scale;
}

/*****************************************************************************/
/*****************************************************************************/
// logf from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

/* origin: FreeBSD /usr/src/lib/msun/src/e_logf.c */
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

static const float
/* |(log(1+s)-log(1-s))/s - Lg(s)| < 2**-34.24 (~[-4.95e-11, 4.97e-11]). */
Lg1 = 0xaaaaaa.0p-24, /* 0.66666662693 */
Lg2 = 0xccce13.0p-25, /* 0.40000972152 */
Lg3 = 0x91e9ee.0p-25, /* 0.28498786688 */
Lg4 = 0xf89e26.0p-26; /* 0.24279078841 */

float logf(float x)
{
	union {float f; uint32_t i;} u = {x};
	float_t hfsq,f,s,z,R,w,t1,t2,dk;
	uint32_t ix;
	int k;

	ix = u.i;
	k = 0;
	if (ix < 0x00800000 || ix>>31) {  /* x < 2**-126  */
		if (ix<<1 == 0)
			return -1/(x*x);  /* log(+-0)=-inf */
		if (ix>>31)
			return (x-x)/0.0f; /* log(-#) = NaN */
		/* subnormal number, scale up x */
		k -= 25;
		x *= 0x1p25f;
		u.f = x;
		ix = u.i;
	} else if (ix >= 0x7f800000) {
		return x;
	} else if (ix == 0x3f800000)
		return 0;

	/* reduce x into [sqrt(2)/2, sqrt(2)] */
	ix += 0x3f800000 - 0x3f3504f3;
	k += (int)(ix>>23) - 0x7f;
	ix = (ix&0x007fffff) + 0x3f3504f3;
	u.i = ix;
	x = u.f;

	f = x - 1.0f;
	s = f/(2.0f + f);
	z = s*s;
	w = z*z;
	t1= w*(Lg2+w*Lg4);
	t2= z*(Lg1+w*Lg3);
	R = t2 + t1;
	hfsq = 0.5f*f*f;
	dk = k;
	return s*(hfsq+R) + dk*ln2_lo - hfsq + f + dk*ln2_hi;
}

/*****************************************************************************/
/*****************************************************************************/
// coshf from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

float coshf(float x)
{
	union {float f; uint32_t i;} u = {.f = x};
	uint32_t w;
	float t;

	/* |x| */
	u.i &= 0x7fffffff;
	x = u.f;
	w = u.i;

	/* |x| < log(2) */
	if (w < 0x3f317217) {
		if (w < 0x3f800000 - (12<<23)) {
			FORCE_EVAL(x + 0x1p120f);
			return 1;
		}
		t = expm1f(x);
		return 1 + t*t/(2*(1+t));
	}

	/* |x| < log(FLT_MAX) */
	if (w < 0x42b17217) {
		t = expf(x);
		return 0.5f*(t + 1/t);
	}

	/* |x| > log(FLT_MAX) or nan */
	t = __expo2f(x);
	return t;
}

/*****************************************************************************/
/*****************************************************************************/
// sinhf from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

float sinhf(float x)
{
	union {float f; uint32_t i;} u = {.f = x};
	uint32_t w;
	float t, h, absx;

	h = 0.5;
	if (u.i >> 31)
		h = -h;
	/* |x| */
	u.i &= 0x7fffffff;
	absx = u.f;
	w = u.i;

	/* |x| < log(FLT_MAX) */
	if (w < 0x42b17217) {
		t = expm1f(absx);
		if (w < 0x3f800000) {
			if (w < 0x3f800000 - (12<<23))
				return x;
			return h*(2*t - t*t/(t+1));
		}
		return h*(t + t/(t+1));
	}

	/* |x| > logf(FLT_MAX) or nan */
	t = 2*h*__expo2f(absx);
	return t;
}

/*****************************************************************************/
/*****************************************************************************/
// ceilf, floorf and truncf from musl-0.9.15
/*****************************************************************************/
/*****************************************************************************/

float ceilf(float x)
{
	union {float f; uint32_t i;} u = {x};
	int e = (int)(u.i >> 23 & 0xff) - 0x7f;
	uint32_t m;

	if (e >= 23)
		return x;
	if (e >= 0) {
		m = 0x007fffff >> e;
		if ((u.i & m) == 0)
			return x;
		FORCE_EVAL(x + 0x1p120f);
		if (u.i >> 31 == 0)
			u.i += m;
		u.i &= ~m;
	} else {
		FORCE_EVAL(x + 0x1p120f);
		if (u.i >> 31)
			u.f = -0.0;
		else if (u.i << 1)
			u.f = 1.0;
	}
	return u.f;
}

float floorf(float x)
{
	union {float f; uint32_t i;} u = {x};
	int e = (int)(u.i >> 23 & 0xff) - 0x7f;
	uint32_t m;

	if (e >= 23)
		return x;
	if (e >= 0) {
		m = 0x007fffff >> e;
		if ((u.i & m) == 0)
			return x;
		FORCE_EVAL(x + 0x1p120f);
		if (u.i >> 31)
			u.i += m;
		u.i &= ~m;
	} else {
		FORCE_EVAL(x + 0x1p120f);
		if (u.i >> 31 == 0)
			u.i = 0;
		else if (u.i << 1)
			u.f = -1.0;
	}
	return u.f;
}

float truncf(float x)
{
	union {float f; uint32_t i;} u = {x};
	int e = (int)(u.i >> 23 & 0xff) - 0x7f + 9;
	uint32_t m;

	if (e >= 23 + 9)
		return x;
	if (e < 9)
		e = 1;
	m = -1U >> e;
	if ((u.i & m) == 0)
		return x;
	FORCE_EVAL(x + 0x1p120f);
	u.i &= ~m;
	return u.f;
}
