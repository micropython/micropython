/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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
