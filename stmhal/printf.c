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

#include "mpconfig.h"
#include "std.h"
#include "misc.h"
#include "systick.h"
#include "qstr.h"
#include "obj.h"
#include "pfenv.h"
#include "mpsnprintf.h"
#if 0
#include "lcd.h"
#endif
#include "uart.h"
#include "usb.h"
#include "pybstdio.h"

#if MICROPY_PY_BUILTINS_FLOAT
#include "formatfloat.h"
#endif

int pfenv_vprintf(const pfenv_t *pfenv, const char *fmt, va_list args);

STATIC void stdout_print_strn(void *dummy_env, const char *str, unsigned int len) {
    stdout_tx_strn_cooked(str, len);
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
int DEBUG_printf(const char *fmt, ...) {
    (void)stream;
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

int snprintf(char *str, size_t size, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = mp_vsnprintf(str, size, fmt, ap);
    va_end(ap);
    return ret;
}
