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

// Small libc functions needed by the core that shared/libc/string0.c does not
// provide. The mem/str basics come from string0.c; these fill the gaps.

#include <stddef.h>
#include <stdint.h>
#include "py/runtime.h"

char *strrchr(const char *s, int c) {
    const char *last = NULL;
    do {
        if (*s == (char)c) {
            last = s;
        }
    } while (*s++);
    return (char *)last;
}

void abort(void) {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("abort")));
    for (;;) {
    }
}

static long strtol_impl(const char *nptr, char **endptr, int base, int is_signed) {
    const char *p = nptr;
    while (*p == ' ' || (*p >= '\t' && *p <= '\r')) {
        p++;
    }
    int neg = 0;
    if (*p == '+' || *p == '-') {
        neg = (*p == '-');
        p++;
    }
    if ((base == 0 || base == 16) && p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
        base = 16;
    } else if (base == 0 && p[0] == '0') {
        base = 8;
    } else if (base == 0) {
        base = 10;
    }
    unsigned long acc = 0;
    for (;; p++) {
        int d;
        if (*p >= '0' && *p <= '9') {
            d = *p - '0';
        } else if (*p >= 'a' && *p <= 'z') {
            d = *p - 'a' + 10;
        } else if (*p >= 'A' && *p <= 'Z') {
            d = *p - 'A' + 10;
        } else {
            break;
        }
        if (d >= base) {
            break;
        }
        acc = acc * base + d;
    }
    if (endptr) {
        *endptr = (char *)p;
    }
    (void)is_signed;
    return neg ? -(long)acc : (long)acc;
}

long strtol(const char *nptr, char **endptr, int base) {
    return strtol_impl(nptr, endptr, base, 1);
}

unsigned long strtoul(const char *nptr, char **endptr, int base) {
    return (unsigned long)strtol_impl(nptr, endptr, base, 0);
}
