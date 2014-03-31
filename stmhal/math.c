#include <stdint.h>
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
/*
double sqrt(double x) {
    // TODO
    return 0.0;
}
*/

float sqrtf(float x) {
    asm volatile (
            "vsqrt.f32  %[r], %[x]\n"
            : [r] "=t" (x)
            : [x] "t"  (x));
    return x;
}

// TODO we need import these functions from some library (eg musl or newlib)
float powf(float x, float y) { return 0.0; }
float logf(float x) { return 0.0; }
float log2f(float x) { return 0.0; }
float log10f(float x) { return 0.0; }
float tanhf(float x) { return 0.0; }
float acoshf(float x) { return 0.0; }
float asinhf(float x) { return 0.0; }
float atanhf(float x) { return 0.0; }
float cosf(float x) { return 0.0; }
float sinf(float x) { return 0.0; }
float tanf(float x) { return 0.0; }
float acosf(float x) { return 0.0; }
float asinf(float x) { return 0.0; }
float atanf(float x) { return 0.0; }
float atan2f(float x, float y) { return 0.0; }
float ceilf(float x) { return 0.0; }
float floorf(float x) { return 0.0; }
float truncf(float x) { return 0.0; }
float fmodf(float x, float y) { return 0.0; }
float tgammaf(float x) { return 0.0; }
float lgammaf(float x) { return 0.0; }
float erff(float x) { return 0.0; }
float erfcf(float x) { return 0.0; }
float modff(float x, float *y) { return 0.0; }
float frexpf(float x, int *exp) { return 0.0; }
float ldexpf(float x, int exp) { return 0.0; }
int __fpclassifyf(float x) { return 0; }

/*****************************************************************************/
// from musl-0.9.15 libm.h

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

/*****************************************************************************/
// scalbnf from musl-0.9.15

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
// expf from musl-0.9.15

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
P1 =  1.6666625440e-1f, /*  0xaaaa8f.0p-26 */
P2 = -2.7667332906e-3f; /* -0xb55215.0p-32 */

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
	c = x - xx*(P1+xx*P2);
	y = 1 + (x*c/(2-c) - lo + hi);
	if (k == 0)
		return y;
	return scalbnf(y, k);
}

/*****************************************************************************/
// expm1f from musl-0.9.15

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
// __expo2f from musl-0.9.15

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
// coshf from musl-0.9.15

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
// sinhf from musl-0.9.15

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
