// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// Math functions the core/libm_dbl reference that lib/libm_dbl does not provide
// (they are normally compiler builtins / single instructions). Implemented via
// clang builtins so they lower to native fabs/copysign on x86-64 and aarch64.

double fabs(double x) {
    return __builtin_fabs(x);
}

double copysign(double x, double y) {
    return __builtin_copysign(x, y);
}

double nan(const char *tagp) {
    (void)tagp;
    return __builtin_nan("");
}

// log2 is not in lib/libm_dbl (only log10 is). Derive it from the natural log.
double log(double x);
double log2(double x) {
    return log(x) * 1.4426950408889634; // 1 / ln(2)
}
