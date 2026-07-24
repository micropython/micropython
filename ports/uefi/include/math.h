// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

/* Minimal <math.h> for the UEFI freestanding build: declarations so includes
 * resolve, backed by the bundled lib/libm_dbl. */
#ifndef MPY_UEFI_MATH_H
#define MPY_UEFI_MATH_H

typedef double double_t;
typedef float float_t;

#define M_PI   3.14159265358979323846
#define M_E    2.7182818284590452354

#define HUGE_VAL  __builtin_huge_val()
#define INFINITY  __builtin_inff()
#define NAN       __builtin_nanf("")

// Classification: backed by lib/libm_dbl's __fpclassifyd / __signbitd (the
// double-precision libm uses 'd'-suffixed internal names).
#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4
int __fpclassifyd(double x);
int __signbitd(double x);
#define fpclassify(x) __fpclassifyd((double)(x))
#define isnan(x)      (__fpclassifyd((double)(x)) == FP_NAN)
#define isinf(x)      (__fpclassifyd((double)(x)) == FP_INFINITE)
#define isfinite(x)   (__fpclassifyd((double)(x)) >= FP_ZERO)
#define signbit(x)    __signbitd((double)(x))

double nan(const char *tagp);
float nanf(const char *tagp);

double fabs(double x);
double sqrt(double x);
double pow(double x, double y);
double exp(double x);
double log(double x);
double log2(double x);
double log10(double x);
double sin(double x);
double cos(double x);
double tan(double x);
double asin(double x);
double acos(double x);
double atan(double x);
double atan2(double y, double x);
double sinh(double x);
double cosh(double x);
double tanh(double x);
double asinh(double x);
double acosh(double x);
double atanh(double x);
double floor(double x);
double ceil(double x);
double trunc(double x);
double round(double x);
double rint(double x);
double scalbn(double x, int n);
double fmod(double x, double y);
double ldexp(double x, int exp);
double frexp(double x, int *exp);
double modf(double x, double *iptr);
double copysign(double x, double y);
double nearbyint(double x);
double expm1(double x);
double log1p(double x);
double cbrt(double x);
double tgamma(double x);
double lgamma(double x);
double erf(double x);
double erfc(double x);

float fabsf(float x);
float sqrtf(float x);

#endif
