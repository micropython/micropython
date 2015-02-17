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

#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "py/obj.h"
#include "py/pfenv.h"
#ifdef MICROPY_HAL_H
#include MICROPY_HAL_H
#endif

#if MICROPY_PY_BUILTINS_FLOAT
#include "py/formatfloat.h"
#endif

STATIC void stdout_print_strn(void *dummy_env, const char *str, mp_uint_t len) {
    mp_hal_stdout_tx_strn_cooked(str, len);
}

STATIC const pfenv_t pfenv_stdout = {0, stdout_print_strn};

int printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = pfenv_vprintf(&pfenv_stdout, fmt, ap);
    va_end(ap);
    return ret;
}

int vprintf(const char *fmt, va_list ap) {
    return pfenv_vprintf(&pfenv_stdout, fmt, ap);
}

#if MICROPY_DEBUG_PRINTERS
mp_uint_t mp_verbose_flag = 1;

int DEBUG_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = pfenv_vprintf(&pfenv_stdout, fmt, ap);
    va_end(ap);
    return ret;
}
#endif

// need this because gcc optimises printf("%c", c) -> putchar(c), and printf("a") -> putchar('a')
int putchar(int c) {
    char chr = c;
    stdout_print_strn(0, &chr, 1);
    return chr;
}

// need this because gcc optimises printf("string\n") -> puts("string")
int puts(const char *s) {
    stdout_print_strn(0, s, strlen(s));
    char chr = '\n';
    stdout_print_strn(0, &chr, 1);
    return 1;
}

typedef struct _strn_pfenv_t {
    char *cur;
    size_t remain;
} strn_pfenv_t;

STATIC void strn_print_strn(void *data, const char *str, mp_uint_t len) {
    strn_pfenv_t *strn_pfenv = data;
    if (len > strn_pfenv->remain) {
        len = strn_pfenv->remain;
    }
    memcpy(strn_pfenv->cur, str, len);
    strn_pfenv->cur += len;
    strn_pfenv->remain -= len;
}

int vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
    strn_pfenv_t strn_pfenv;
    strn_pfenv.cur = str;
    strn_pfenv.remain = size;
    pfenv_t pfenv;
    pfenv.data = &strn_pfenv;
    pfenv.print_strn = strn_print_strn;
    int len = pfenv_vprintf(&pfenv, fmt, ap);
    // add terminating null byte
    if (size > 0) {
        if (strn_pfenv.remain == 0) {
            strn_pfenv.cur[-1] = 0;
        } else {
            strn_pfenv.cur[0] = 0;
        }
    }
    return len;
}

int snprintf(char *str, size_t size, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(str, size, fmt, ap);
    va_end(ap);
    return ret;
}
